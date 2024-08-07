#include "InOut.h"
#include "infra/include/Timestamp.h"
#include "infra/include/TimeDelta.h"
#include "infra/include/Logger.h"
#include "infra/include/thread/WorkThreadPool.h"

InOut::InOut() {
}

InOut::~InOut() {
}

bool InOut::init() {
    oac_client_->init(shared_from_this());
    if (!oac_client_->start()) {
        return false;
    }
    if (!initAlg()) {
        errorf("init alg faild\n");
        return false;
    }
    return infra::Thread::start();
}

bool InOut::initAlg() {

    ulu_best::EnableDebug(1);
    ulu_best::SdkUninit();
    ulu_best::SdkInit();

    ulu_best::EReturn_Code ret = CreateMixDetect(&MixDetect_, ulu_best::EMixType_CarPlate);
    if (ret != ulu_best::EC_OK || !MixDetect_) {
        errorf("alg sdk init failed\n");
        return false;
    }

    ulu_best::SUbMixDetectCfg mixCfg;
    memset(&mixCfg, 0, sizeof(mixCfg));
    int anchors[6 * 3] = {37, 18, 46, 37, 73, 50, 94, 79,142, 100, 170, 157, 196, 238, 344, 249, 294, 398};
    strcpy(mixCfg.model_path, "/app/fs/models/ulu_4cls_v5n_no.rknn");
    memcpy(&mixCfg.anchors[0], &anchors[0], sizeof(anchors));
    mixCfg.anchor_cols = 6;
    mixCfg.anchor_rows = 3;
    mixCfg.nms = 0.3;
    mixCfg.nc = 2;
    mixCfg.cls1_id = 0;  // 车子id
    mixCfg.cls1_conf = 0.45;
    mixCfg.cls2_id = 1;  // 车牌id
    mixCfg.cls2_conf = 0.5;
    mixCfg.track_cls_id = 0;

    ret = MixDetect_->SetDetectCfg((void *)(&mixCfg));
    if (ret != ulu_best::EC_OK) {
        errorf("pMixDetect->SetDetectCfg(%d) fail\n", (int)ret);
        DestroyMixDetect(&MixDetect_);
        return false;
    }

    ulu_best::SUbTrackCfg trackCfg;
    memset(&trackCfg, 0, sizeof(trackCfg));
    trackCfg.max_age = 25;
    trackCfg.critical_distance = 40.0;
    trackCfg.min_hits = 3;
    trackCfg.width = 640;  // -10
    trackCfg.height = 480; // -10

    ret = MixDetect_->SetTrackCfg((void *)(&trackCfg));
    if (ret != ulu_best::EC_OK) {
        errorf("pMixDetect->SetTrackCfg(%d) fail\n", (int)ret);
        DestroyMixDetect(&MixDetect_);
        return false;
    }

    ulu_best::IObjDetect *pDetect = (ulu_best::IObjDetect *)MixDetect_->GetMixUnit(ulu_best::EMindUnit_ObjDetect);
    if (pDetect) {
        pDetect->SetClsScoreMode(mixCfg.cls2_id, ulu_best::EScore_BoxArea); // 车牌设置按面积算得分
        pDetect->SetClsAreaIou(mixCfg.cls1_id, 0.001);                      // 车设置检测区域重合度
        pDetect->SetClsAreaIou(mixCfg.cls2_id, 0.001);                      // 车设置检测区域重合度
        pDetect->DisableClsLine(mixCfg.cls1_id);                  // 工位车子不开启相交线检测  
        pDetect->DisableClsLine(mixCfg.cls2_id);                  // 工位车子不开启相交线检测  
    }

    std::vector<ulu_best::SUbPoint> pts;
    pts.push_back({0.1, 0.5});
    pts.push_back({0.5, 0.5});
    pts.push_back({0.5, 0.9});
    pts.push_back({0.1, 0.9});
    pDetect->SetRegion(pts, ulu_best::ERK_IN_AREA1);
    region_point1_.push_back(pts);

    pts.resize(2);
    pDetect->SetLine(pts, ulu_best::ERK_IN_LINE1);

    infof("alg init succ\n");
    return true;
}

void InOut::run() {
    infof("start oac client test thread\n");
    int32_t count = 0;
    infra::Timestamp last_getstate_time = infra::Timestamp::now();
    infra::Timestamp last_statistic_fps_time = infra::Timestamp::now();
    uint32_t last_frame_number = 0;
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        //infof("oac client use image index:%d, pts:%lld, number:%d, data:%p\n", image.index, image.timestamp, image.frame_number, image.data);
        if (last_frame_number != image.frame_number - 1) {
            warnf("skip frame %u\n", last_frame_number + 1);
        }
        last_frame_number = image.frame_number;

        ulu_best::SUbImg cv_img;
        cv_img.pixel_format = ulu_best::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.buffer[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.time = image.timestamp;

        std::vector<ulu_best::SUbLossInfo> loss_objects;
        std::vector<ulu_best::SUbMixInfo> objects;
        
        infra::Timestamp t1 = infra::Timestamp::now();
        int ret = MixDetect_->UpdateEx(cv_img, objects);
        if (ret > 0) {
            MixDetect_->LossData(loss_objects);
        }
        infra::Timestamp t2 = infra::Timestamp::now();
        infra::TimeDelta delta = t2 - t1;
        //tracef("cal %lld\n", delta);
        oac_client_->releaseImageFrame(image);

        for (auto it = objects.begin(); it != objects.end(); ++it) {
            if (it->track_obj.objid < 1) {
                // 测试说 id = -1 的会数据重复；算法说 id < 1 的都不处理，过滤掉即可
                //warnf("skip id = %d\n", it->objid);
                continue;
            }
            //tracef("id:%d, second_cls_thre:%0.f, [(%0.2f, %0.2f),(%0.2f, %0.2f)]\n", 
            //    it->second_clsid, it->second_cls_thre, it->second_bbox[0], it->second_bbox[1], it->second_bbox[2], it->second_bbox[3]);
        }

        count++;
        if (count >= 100) {
            infra::Timestamp now = infra::Timestamp::now();
            tracef("alg fps: %0.1f\n", count * 1000.0f / (now - last_statistic_fps_time).millis());
            count = 0;
            last_statistic_fps_time = now;

            pushDetectRegion(region_point1_);
        }

        pushDetectTarget(image, objects);
    }
}


std::vector<float> lowPassFilter(const std::vector<float>& input, int windowSize) {
    if (windowSize <= 0 || input.empty()) {
        return input;
    }
    std::vector<float> output(input.size(), 0.0);
    std::vector<float> window(windowSize, 0.0);
    double sum = 0.0;

    for (size_t i = 0; i < input.size(); ++i) {
        // 更新窗口
        sum -= window[i % windowSize];
        window[i % windowSize] = input[i];
        sum += window[i % windowSize];

        // 计算平均值
        output[i] = sum / windowSize;
    }
    return output;
}

static void filter(ulu_best::SUbObjInfo& obj) {
    static std::map<int, std::vector<ulu_best::SUbObjInfo>> s_obj_map;
    auto it = s_obj_map.find(obj.objid);
    if (it == s_obj_map.end()) {
        std::vector<ulu_best::SUbObjInfo> obj_list;
        obj_list.push_back(obj);
        s_obj_map[obj.objid] = obj_list;
    } else {
        auto &obj_list = it->second;

        float a = 0.8f;
        float b = 0.2f;
        float c = 0.0f;
        float d = 0.0f;

        //infof(" [%.2f, %.2f, %.2f, %.2f]\n", obj.bbox[0], obj.bbox[1], obj.bbox[2], obj.bbox[3]);

        if (obj_list.size() == 1) {
            a = 0.8f; b = 0.2f;
            obj.bbox[0] = obj.bbox[0] * a + obj_list[0].bbox[0] * b;
            obj.bbox[1] = obj.bbox[1] * a + obj_list[0].bbox[1] * b;
            obj.bbox[2] = obj.bbox[2] * a + obj_list[0].bbox[2] * b;
            obj.bbox[3] = obj.bbox[3] * a + obj_list[0].bbox[3] * b;
            obj_list.push_back(obj);
        } else if (obj_list.size() == 2) {
            a = 0.5f; b = 0.3f; c = 0.2f;
            obj.bbox[0] = obj.bbox[0] * a + obj_list[1].bbox[0] * b + obj_list[0].bbox[0] * c;
            obj.bbox[1] = obj.bbox[1] * a + obj_list[1].bbox[1] * b + obj_list[0].bbox[1] * c;
            obj.bbox[2] = obj.bbox[2] * a + obj_list[1].bbox[2] * b + obj_list[0].bbox[2] * c;
            obj.bbox[3] = obj.bbox[3] * a + obj_list[1].bbox[3] * b + obj_list[0].bbox[3] * c;
            obj_list.push_back(obj);
        } else if (obj_list.size() == 3) {
            a = 0.4f; b = 0.3f; c = 0.2f; d = 0.1f;
            obj.bbox[0] = obj.bbox[0] * a + obj_list[2].bbox[0] * b + obj_list[1].bbox[0] * c + obj_list[0].bbox[0] * d;
            obj.bbox[1] = obj.bbox[1] * a + obj_list[2].bbox[1] * b + obj_list[1].bbox[1] * c + obj_list[0].bbox[1] * d;
            obj.bbox[2] = obj.bbox[2] * a + obj_list[2].bbox[2] * b + obj_list[1].bbox[2] * c + obj_list[0].bbox[2] * d;
            obj.bbox[3] = obj.bbox[3] * a + obj_list[2].bbox[3] * b + obj_list[1].bbox[3] * c + obj_list[0].bbox[3] * d;
        }

        //tracef("[%.2f, %.2f, %.2f, %.2f] [%.2f, %.2f, %.2f, %.2f] [%.2f, %.2f, %.2f, %.2f]\n\n", 
        //    obj.bbox[0], obj.bbox[1], obj.bbox[2], obj.bbox[3],
        //    obj_list[1].bbox[0], obj_list[1].bbox[1], obj_list[1].bbox[2], obj_list[1].bbox[3],
        //    obj_list[0].bbox[0], obj_list[0].bbox[1], obj_list[0].bbox[2], obj_list[0].bbox[3]);
        //
        size_t i = 1;
        for (i = 1; i < obj_list.size(); i++) {
            obj_list[i - 1] = obj_list[i];
        }
        obj_list[i - 1] = obj;
    }
}

void InOut::pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_best::SUbMixInfo> &objects) {
    CurrentDetectResult result;
    result.timestamp = image.timestamp;

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        auto &obj = it->track_obj;

        filter(obj);

        Target target;
        target.type = E_TargetType_body;
        target.id = obj.objid;
        target.shap_type = E_TargetShapType_rect_pose;
        target.rect.x = obj.bbox[0] / image.width;     //left
        target.rect.y = obj.bbox[1] / image.height;    //top
        target.rect.w = (obj.bbox[2] - obj.bbox[0]) / image.width;  //right - left
        target.rect.h = (obj.bbox[3] - obj.bbox[1]) / image.height; //bottom - top

        result.targets.push_back(target);
        
        //infof("[%dx%d] [%.2f, %.2f, %.2f, %.2f] -> [%.2f, %.2f, %.2f, %.2f]\n", image.width, image.height, 
        //    it->box.bbox[0], it->box.bbox[1], it->box.bbox[2], it->box.bbox[3],
        //    target.rect.x, target.rect.y, target.rect.w, target.rect.h);
        // 车牌框
        if (it->second_clsid > 0) {
            Target target;
            target.type = E_TargetType_body;
            target.id = obj.objid;
            target.shap_type = E_TargetShapType_rect_pose;
            target.rect.x = it->second_bbox[0] / image.width;     //left
            target.rect.y = it->second_bbox[1] / image.height;    //top
            target.rect.w = (it->second_bbox[2] - it->second_bbox[0]) / image.width;  //right - left
            target.rect.h = (it->second_bbox[3] - it->second_bbox[1]) / image.height; //bottom - top
            result.targets.push_back(target);
        }
    }
    infra::WorkThreadPool::instance()->async([this, result]() mutable {
        oac_client_->pushCurrentDetectTarget(result);
    });
}

void InOut::pushDetectRegion(std::vector<std::vector<ulu_best::SUbPoint>> &regions) {
    std::vector<DetectRegion> detect_region;
    for (auto i = 0; i < regions.size(); i++) {
        DetectRegion region;
        region.id = std::to_string(i);
        for (auto &point: regions[i]) {
            region.points.push_back({point.x, point.y});
        }
        detect_region.push_back(region);
    }

    infra::WorkThreadPool::instance()->async([this, detect_region]() mutable {
        oac_client_->pushDetectRegion(detect_region);
    });
}

std::string InOut::version() {
    return "1.0.0.1";
}

std::string InOut::sdkVersion() {
    return alg_sdk_version_;
}