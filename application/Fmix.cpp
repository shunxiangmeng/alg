#include "Fmix.h"
#include "infra/include/Timestamp.h"
#include "infra/include/TimeDelta.h"
#include "infra/include/Logger.h"

Fmix::Fmix() {
}

bool Fmix::init() {
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

bool Fmix::initAlg() {
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

    //person_->SetEScene(ulu_face::ES_GE60DegreeObliqueStore);
    person_->SetFaceDetectAlgorithm(ulu_face::E_PDA_2);

    std::string detect_model_full_path = "/app/fs/models/detect/FaceDetect_bosideng.rknn";
    size_t detect_model_size = 0;
    std::shared_ptr<unsigned char> model_data = read_model_2_buff(detect_model_full_path.c_str(), detect_model_size);
    tracef("detect_model_size = %d\n", detect_model_size);
    ret = person_->Init(ulu_face::E_MT_FaceDetect, model_data.get(), detect_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("alg Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }

    /*std::string reid_model_full_path = "/app/fs/models/reid/reid.rknn";
    size_t reid_model_size = 0;
    std::shared_ptr<unsigned char> model_data2 = read_model_2_buff(reid_model_full_path.c_str(), reid_model_size);
    tracef("reid_model_size = %d\n", reid_model_size);
    ret = person_->Init(ulu_face::E_MT_ReID, model_data2.get(), reid_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }*/

	/*std::string pose_model_full_path = "/app/fs/models/align_pose/LandmarkPose.rknn";
    size_t pose_model_size = 0;
    std::shared_ptr<unsigned char> model_data3 = read_model_2_buff(pose_model_full_path.c_str(), pose_model_size);
    tracef("pose_model_size = %d\n", pose_model_size);
    ret = person_->Init(ulu_face::E_MT_FacePose, model_data3.get(), pose_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }*/

    /*std::string reg_model_full_path = "/app/fs/models/feature_extract/FeatureExtractRS.rknn";
    size_t reg_model_size = 0;
    std::shared_ptr<unsigned char> model_data4 = read_model_2_buff(reg_model_full_path.c_str(), reg_model_size);
    tracef("reg_model_size = %d\n", reg_model_size);
    ret = person_->Init(ulu_face::E_MT_FaceReg, model_data4.get(), reg_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }*/

    /*std::string attr_model_full_path = "/app/fs/models/Attrib/GenderAge.rknn";
    size_t attr_model_size = 0;
    std::shared_ptr<unsigned char> model_data5 = read_model_2_buff(attr_model_full_path.c_str(), attr_model_size);
    tracef("attr_model_size = %d\n", attr_model_size);
    ret = person_->Init(ulu_face::E_MT_FaceAttr, model_data5.get(), attr_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }*/

    person_->SetPedMaxMergeThreshould(0.85);
    person_->SetMinHitTimes(1);
    person_->SetMinLifeSeconds(0);
    person_->SetMaxMergeSeconds(0);
    person_->SetRegionIOU(0);
    person_->SetMinMoveDistance(0);
    //person_->SetFaceMaxMergeThreshould(0.85);

    person_->SetEScene(ulu_face::ES_CommonStore);

    person_->SetModule(ulu_face::EPF_FaceDetect | ulu_face::EPF_PedestrianDetect, true);
    person_->SetModule(ulu_face::EPF_ReID | ulu_face::EPF_HotZone | ulu_face::EPF_FaceVerify, false);

    infof("init alg version: %s succeed\n", alg_sdk_version);
    return true;
}

void Fmix::run() {
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

        std::vector<ulu_face::SPersonInfo> detect_result;
        infra::Timestamp t1 = infra::Timestamp::now();
        int ret = person_->Update(cv_img, detect_result);
        infra::Timestamp t2 = infra::Timestamp::now();
        infra::TimeDelta delta = t2 - t1;

        //infof("time_diff:%lld, detect_result.size():%d\n", delta.millis(), detect_result.size());

        oac_client_->releaseImageFrame(image);

        pushDetectTarget(image, detect_result);


        infra::Timestamp now = infra::Timestamp::now();
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
        }

        //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::string Fmix::version() {
    return "1.0.0.1";
}

std::string Fmix::sdkVersion() {
    return alg_sdk_version_;
}
