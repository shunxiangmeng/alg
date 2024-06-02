/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Client.h
 * Author      :  mengshunxiang 
 * Data        :  2024-06-02 17:53:08
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "infra/include/thread/Thread.h"
#include "common/middleware/oac/include/OacClient.h"
#include "common/middleware/private/include/IPrivClient.h"

#include "IULUPose.h"
#include "IULUPerson.h"
#include "IULUBaseType.h"
#include "IULUFace.h"
#include "Uluai.h"

class Client : public infra::Thread {
public:

    Client();

    bool init();
    bool initAlg();
private:
    virtual void run() override;
private:
    oac::IOacClient *oac_client_;
    ulu_face::IULUPerson *person_ = nullptr;
};
