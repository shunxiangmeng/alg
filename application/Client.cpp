#include "Client.h"
#include "infra/include/Logger.h"

Client::Client() : oac_client_(oac::IOacClient::instance()) {
}

bool Client::init() {
    if (!oac_client_->start()) {
        return false;
    }

    initAlg();

    return infra::Thread::start();
}

bool Client::initAlg() {
    char alg_sdk_version[16] = {0};
    ulu_face::GetVersion(alg_sdk_version);
    infof("alg version: %s\n", alg_sdk_version);

    if (ulu_face::SDKInit("") != ulu_face::E_OK) {
        errorf("alg sdk init failed\n");
        return false;
    }
    infof("000\n");
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

    person_->SetEScene(ulu_face::ES_CommonStore);
    person_->SetPersonDetectAlgorithm(ulu_face::E_PDA_2); 

    std::string detect_model_full_path = "/app/fs/models/detect/FaceDetect_bosideng.rknn";
    size_t detect_model_size = 0;
    std::shared_ptr<unsigned char> model_data = read_model_2_buff(detect_model_full_path.c_str(), detect_model_size);
    tracef("detect_model_size = %d\n", detect_model_size);

    ret = person_->Init(ulu_face::E_MT_FaceDetect, model_data.get(), detect_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }

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

	std::string pose_model_full_path = "/app/fs/models/align_pose/LandmarkPose.rknn";
    size_t pose_model_size = 0;
    std::shared_ptr<unsigned char> model_data3 = read_model_2_buff(pose_model_full_path.c_str(), pose_model_size);
    tracef("pose_model_size = %d\n", pose_model_size);
    ret = person_->Init(ulu_face::E_MT_FacePose, model_data3.get(), pose_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }

    std::string reg_model_full_path = "/app/fs/models/feature_extract/FeatureExtractRS.rknn";
    size_t reg_model_size = 0;
    std::shared_ptr<unsigned char> model_data4 = read_model_2_buff(reg_model_full_path.c_str(), reg_model_size);
    tracef("reg_model_size = %d\n", reg_model_size);
    ret = person_->Init(ulu_face::E_MT_FaceReg, model_data4.get(), reg_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }

    std::string attr_model_full_path = "/app/fs/models/feature_extract/FeatureExtractRS.rknn";
    size_t attr_model_size = 0;
    std::shared_ptr<unsigned char> model_data5 = read_model_2_buff(attr_model_full_path.c_str(), attr_model_size);
    tracef("attr_model_size = %d\n", attr_model_size);
    ret = person_->Init(ulu_face::E_MT_FaceAttr, model_data5.get(), attr_model_size);
    if (ret != ulu_face::E_OK) {
        errorf("ulu_fs Init face detect fail, ret = %d\n", ret);
        DestroyPerson(&person_);
        return false;
    }

    person_->SetPedMaxMergeThreshould(0.85);
    person_->SetMinHitTimes(1);
    person_->SetMinLifeSeconds(0);
    person_->SetMaxMergeSeconds(0);
    person_->SetRegionIOU(0.8);
    person_->SetMinMoveDistance(0);
    person_->SetFaceMaxMergeThreshould(0.85);

    person_->SetEScene(ulu_face::ES_CommonStore);

    person_->SetModule(ulu_face::EPF_FaceDetect | ulu_face::EPF_PedestrianDetect | ulu_face::EPF_PassengerFlow, true);
    person_->SetModule(ulu_face::EPF_ReID | ulu_face::EPF_HotZone | ulu_face::EPF_FaceVerify, false);

    infof("init alg version: %s succeed\n", alg_sdk_version);
    return true;
}

void Client::run() {
    infof("start oac client test thread\n");
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        infof("oac client use image index:%d, pts:%lld, wxh[%dx%d], size:%d, data:%p\n", 
            image.index, image.timestamp, image.width, image.height, image.size, image.data);

        ulu_face::SULUImage cv_img;
        cv_img.pixel_format = ulu_face::EPIX_FMT_RGB888;
        cv_img.width = image.width;
        cv_img.height = image.height;
        cv_img.vir_addrs[0]= (ulu_face::ulu_uchat_t*)image.data;
        cv_img.data_sizes[0] = image.size;

        std::vector<ulu_face::SPersonInfo> out_current_info;

        tracef("alg update+++\n");
        person_->Update(cv_img, out_current_info);
        tracef("alg update---\n");
        tracef("out_current_info.size():%d\n", out_current_info.size());
        
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        oac_client_->releaseImageFrame(image);

    }

}