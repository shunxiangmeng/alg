/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  InOut.h
 * Author      :  mengshunxiang 
 * Data        :  2024-07-31 13:53:08
 * Description :  出入口算法
 * Note        : 
 ************************************************************************/
#pragma once
#include <string>
#include "infra/include/thread/Thread.h"
#include "Uluai.h"
#include "UbCommon.h"
#include "UbSdk.h"

class InOut : public oac::IOacAlg, public IUluai, public infra::Thread {
public:
    InOut();
    virtual ~InOut() override;

    virtual bool init() override;
    bool initAlg();
private:
    virtual void run() override;
    
    virtual std::string version() override;
    virtual std::string sdkVersion() override;

    void pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_best::SUbObjInfo> &objects);

private:
    ulu_face::IULUPerson *person_ = nullptr;
    std::string alg_sdk_version_;

    ulu_best::IMixDetect* MixDetect_ = nullptr;
};