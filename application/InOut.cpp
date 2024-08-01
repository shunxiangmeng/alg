#include "InOut.h"
#include "infra/include/Timestamp.h"
#include "infra/include/TimeDelta.h"
#include "infra/include/Logger.h"

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
    mixCfg.box_conf = 0.48;
    mixCfg.cls_conf = 0.3;
    mixCfg.nms = 0.3;

    mixCfg.nc = 2;
    mixCfg.cls1_id = 0;
    mixCfg.cls2_id = 1;
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
    }
    infof("alg init succ\n");
    return true;
}

void InOut::run() {
    infof("start oac client test thread\n");
    int32_t count = 0;
    infra::Timestamp last_getstate_time = infra::Timestamp::now();
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        //infof("oac client use image index:%d, pts:%lld, wxh[%dx%d], data:%p\n", image.index, image.timestamp, image.width, image.height, image.data);

        ulu_best::SUbImg cv_img;
        cv_img.pixel_format = ulu_best::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.buffer[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.time = image.timestamp;

        std::vector<ulu_best::SUbLossInfo> loss_objects;
        std::vector<ulu_best::SUbObjInfo> objects;
        
        int ret = MixDetect_->Update(cv_img, objects);
        if (ret > 0) {
            MixDetect_->LossData(loss_objects);
        }
        oac_client_->releaseImageFrame(image);

        for (auto it = objects.begin(); it != objects.end(); ++it) {
            if (it->objid < 1) {
                // 测试说 id = -1 的会数据重复；算法说 id < 1 的都不处理，过滤掉即可
                //warnf("skip id = %d\n", it->objid);
                continue;
            }
        }

        pushDetectTarget(image, objects);
    }
}

void InOut::pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_best::SUbObjInfo> &objects) {
    CurrentDetectResult result;
    result.timestamp = image.timestamp;

    for (auto it = objects.begin(); it != objects.end(); ++it) {
        Target target;
        target.type = E_TargetType_body;
        target.id = it->objid;
        target.shap_type = E_TargetShapType_rect_pose;
        target.rect.x = it->bbox[0] / image.width;     //left
        target.rect.y = it->bbox[1] / image.height;    //top
        target.rect.w = (it->bbox[2] - it->bbox[0]) / image.width;  //right - left
        target.rect.h = (it->bbox[3] - it->bbox[1]) / image.height; //bottom - top
        
        //infof("[%dx%d] [%.2f, %.2f, %.2f, %.2f] -> [%.2f, %.2f, %.2f, %.2f]\n", image.width, image.height, 
        //    it->box.bbox[0], it->box.bbox[1], it->box.bbox[2], it->box.bbox[3],
        //    target.rect.x, target.rect.y, target.rect.w, target.rect.h);

        result.targets.push_back(target);
    }
    oac_client_->pushCurrentDetectTarget(result);
}

std::string InOut::version() {
    return "1.0.0.1";
}

std::string InOut::sdkVersion() {
    return alg_sdk_version_;
}