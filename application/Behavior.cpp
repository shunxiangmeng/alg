#include "Behavior.h"
#include "infra/include/Timestamp.h"
#include "infra/include/TimeDelta.h"
#include "infra/include/Logger.h"

Behavior::Behavior() {
}

Behavior::~Behavior() {
}

bool Behavior::init() {
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

bool Behavior::initAlg() {
    char alg_sdk_version[16] = {0};
    ulu_face::GetVersion(alg_sdk_version);
    infof("alg version: %s\n", alg_sdk_version);
    alg_sdk_version_ = alg_sdk_version;

    if (ulu_face::SDKInit("") != ulu_face::E_OK) {
        errorf("alg sdk init failed\n");
        return false;
    }

    ulu_face::SetDefaultPixelFormat(ulu_face::EPIX_FMT_RGB888);
    ulu_face::SetTrackMaxAge(7);
    ulu_face::SetTrackTentativeAge(3);
    ulu_face::SetFaceDetectThreshold(0.5); // default = 0.5
    ulu_face::SetFacePedestrianThreshold(0.5); // default = 0.4

    ulu_face::EReturn_Code ret = CreatePerson(&person_);
    if (ret != ulu_face::E_OK) {
        errorf("CreatePerson failed, ret:%d\n", ret);
        return false;
    }

	person_->SetEScene(ulu_face::ES_CommonStore, true);
    person_->SetPedMaxMergeThreshould(0.7);
    person_->SetMinHitTimes(8);
    person_->SetMinLifeSeconds(1);
    person_->SetMaxMergeSeconds(60);
    person_->SetMinMoveDistance(30);

    std::string reid_model_full_path = "/app/fs/models/reid/reid.rknn";
    size_t reid_model_size = 0;
    std::shared_ptr<unsigned char> model_data2 = read_model_2_buff(reid_model_full_path.c_str(), reid_model_size);
    tracef("reid_model_size = %d\n", reid_model_size);
    ret = person_->Init(ulu_face::E_MT_ReID, model_data2.get(), reid_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }
    person_->SetModule(ulu_face::EPF_ReID, true);

    // init pose detect
    if (!initPoseDetect()) {
        return false;
    }

    //init attr detect
    if (!initAttrDetect()) {
        return false;
    }

    infof("init alg succ\n");
    return true;
}

bool Behavior::initPoseDetect() {
    auto ret = CreatePose(&pose_);
    if( ret != ulu_face::E_OK) {
        errorf("ulu_pose create object fail, ret = %d!\n", ret);
        return false;
    }
	pose_->SetPoseKeyPointType(ulu_face::E_PKP_PersonPose_COCO18);
	pose_->SetPoseAlgorithmType(ulu_face::E_PA_PersonPose_Other2);

    std::string pose_model_full_path = "/app/fs/models/person_pose/person_pose_rknn.ulu";
    size_t pose_model_size = 0;
    std::shared_ptr<unsigned char> pose_model_data = read_model_2_buff(pose_model_full_path.c_str(), pose_model_size);
    tracef("pose_model_size = %d\n", pose_model_size);
    ret = pose_->Init(ulu_face::E_MT_PersonPose, pose_model_data.get(), pose_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init pose detect fail, ret = %d\n", ret);
        DestroyPose(&pose_);
        return false;
    }
    pose_->SetThreshould(0.65);
    return true;
}

bool Behavior::initAttrDetect() {
    ulu_best::EnableDebug(3);
    ulu_best::SdkUninit();
    ulu_best::SdkInit();

    std::string pose_model_full_path = "/app/fs/models/attr.rknn";
    CreateAttrExtract(ulu_best::E_Attr_Person, &attr_extract_);
    if(!attr_extract_) {
        errorf("ulu_attr create object fail!\n");
        return false;
    }

    auto ret = attr_extract_->Init(pose_model_full_path.c_str());
    if (ret != ulu_best::EC_OK) {
        DestroyAttrExtract(&attr_extract_);
        attr_extract_ == nullptr;
        errorf("attr_extract init error, ret:%d\n", ret);
        return false;
    }
    return true;
}

void Behavior::run() {
    infof("start oac client test thread\n");
    int32_t count = 0;
    infra::Timestamp last_getstate_time = infra::Timestamp::now();
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        //infof("oac client use image index:%d, pts:%lld, wxh[%dx%d], size:%d, data:%p\n", image.index, image.timestamp, image.width, image.height, image.size, image.data);

        ulu_face::STimestamp tv;
        ulu_face::GetCurrentTimestamp(tv);

        ulu_face::SULUImage cv_img;
        cv_img.pixel_format = ulu_face::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.vir_addrs[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.data_sizes[0] = image.size;
        cv_img.time_stamp.tv_sec = tv.tv_sec;
        cv_img.time_stamp.tv_usec = tv.tv_usec;

        std::vector<ulu_face::SPoseEstimation> current_pose;
        pose_->GetPose(cv_img, current_pose);

        //infof("time_diff:%lld, detect_result.size():%d\n", delta.millis(), detect_result.size());

        oac_client_->releaseImageFrame(image);

        //tracef("current_pose.size():%d\n", current_pose.size());
        for (auto it = current_pose.begin(); it != current_pose.end(); ++it) {
            for (int i = 0; i < it->keypoint_num; i++) {
                //tracef("p:%d, score:%0.2f, point(%02.f, %02.f)\n", i, it->scores[i], it->keypoints[i * 2], it->keypoints[i * 2 + 1]);
            }
        }

        pushDetectTarget(image, current_pose);


        /*infra::Timestamp now = infra::Timestamp::now();
        auto get_state_diff = infra::Timestamp::now() - last_getstate_time;
        if (get_state_diff.millis() > 2000) {
            last_getstate_time = now;
            std::vector<ulu_face::SLostPersonInfo> lost_person;
            ulu_face::SPersonStat person_stat;
            infra::Timestamp t1 = infra::Timestamp::now();
            int32_t result = person_->GetStat(lost_person, person_stat, tv);
            infra::Timestamp t2 = infra::Timestamp::now();
            infra::TimeDelta delta = t2 - t1;
            infof("getstat used time:%lld\n", delta.millis());
            if (result) {

            }
        }*/

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::string Behavior::version() {
    return "1.0.0.1";
}

std::string Behavior::sdkVersion() {
    return alg_sdk_version_;
}

void Behavior::pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_face::SPoseEstimation> &poses) {
    CurrentDetectResult result;
    result.timestamp = image.timestamp;

    for (auto it = poses.begin(); it != poses.end(); ++it) {
        Target target;
        target.type = E_TargetType_body;
        target.id = 0;
        target.rect.x = it->box.lt_x() / image.width;
        target.rect.y = it->box.lt_y() / image.height;
        target.rect.w = it->box.width() / image.width;
        target.rect.h = it->box.height()  / image.height;
        result.targets.push_back(target);
        //infof("[%dx%d] [%.2f, %.2f, %.2f, %.2f] -> [%.2f, %.2f, %.2f, %.2f]\n", image.width, image.height, 
        //    it->box.bbox[0], it->box.bbox[1], it->box.bbox[2], it->box.bbox[3],
        //    target.rect.x, target.rect.y, target.rect.w, target.rect.h);

        for (int i = 0; i < it->keypoint_num; i++) {
            //tracef("p:%d, score:%0.2f, point(%02.f, %02.f)\n", i, it->scores[i], it->keypoints[i * 2], it->keypoints[i * 2 + 1]);
        }

    }



    oac_client_->pushCurrentDetectTarget(result);
}
