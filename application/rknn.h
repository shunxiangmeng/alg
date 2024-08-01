#pragma once
#include <string>
#include "infra/include/thread/Thread.h"
#include "Uluai.h"
#include "rknn/include/rknn_api.h"

class Rknn : public oac::IOacAlg, public IUluai, public infra::Thread {
public:
    Rknn();
    virtual ~Rknn() override;

    bool init() override;
    bool initAlg();
private:
    virtual void run() override;
    
    virtual std::string version() override;
    virtual std::string sdkVersion() override;

private:
    rknn_context rknn_context_ = 0;
    std::string alg_sdk_version_;
};
