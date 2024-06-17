/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Fmix.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-02 17:53:08
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <string>
#include "infra/include/thread/Thread.h"
#include "Uluai.h"

class Fmix : public oac::IOacAlg, public IUluai, public infra::Thread {
public:
    Fmix();

    bool init() override;
    bool initAlg();
private:
    virtual void run() override;
    
    virtual std::string version() override;
    virtual std::string sdkVersion() override;

private:
    ulu_face::IULUPerson *person_ = nullptr;
    std::string alg_sdk_version_;
};
