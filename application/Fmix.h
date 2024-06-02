/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  alg.h
 * Author      :  mengshunxiang 
 * Data        :  2024-05-25 16:37:37
 * Description :  None
 * Note        : 
 ************************************************************************/
#pragma once
#include "Uluai.h"

#include "IULUPose.h"
#include "IULUPerson.h"
#include "IULUBaseType.h"
#include "IULUFace.h"

class Fmix : public IUluai {
public:
    Fmix();
    virtual ~Fmix();

    virtual bool init() override;

private:
    ulu_face::IULUPerson *person_ = nullptr;
};