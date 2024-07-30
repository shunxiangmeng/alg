/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  uluai.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-25 16:40:32
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include <string>
#include <memory>
#include "common/middleware/oac/include/OacClient.h"
#include "common/middleware/private/include/IPrivClient.h"

#include "IULUPose.h"
#include "IULUPerson.h"
#include "IULUBaseType.h"
#include "IULUFace.h"

typedef enum {
    E_ALG_Fmix = 0,
    E_ALG_XW
} E_ALG_TYPE;

class IUluai {
public:
    IUluai();
    virtual ~IUluai() = default;
    static std::shared_ptr<IUluai> create(E_ALG_TYPE alg_type);

    virtual bool init() = 0;

    void pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_face::SPersonInfo> &detect_result);

protected:
    oac::IOacClient *oac_client_ = nullptr;
private:
}; 

std::shared_ptr<unsigned char> read_model_2_buff(const char *file_path, size_t& model_size);