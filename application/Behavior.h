/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Behavior.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-30 17:53:08
 * Description :  行为分析算法
 * Note        : 
 ************************************************************************/
#pragma once
#include <string>
#include "infra/include/thread/Thread.h"
#include "Uluai.h"
#include "UbCommon.h"
#include "UbSdk.h"

class Behavior : public oac::IOacAlg, public IUluai, public infra::Thread {
public:
    Behavior();
    virtual ~Behavior() override;

    virtual bool init() override;
    bool initAlg();
private:
    virtual void run() override;
    
    virtual std::string version() override;
    virtual std::string sdkVersion() override;

    bool initPoseDetect();
    bool initAttrDetect();

    void pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_face::SPoseEstimation> &poses);

private:
    ulu_face::IULUPerson *person_ = nullptr;
    ulu_face::IULUPose *pose_ = nullptr;
    ulu_best::IAttrExtract *attr_extract_ = nullptr;
    std::string alg_sdk_version_;
};
