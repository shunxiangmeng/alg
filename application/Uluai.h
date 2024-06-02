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
#include <memory>

class IUluai {
public:
    virtual ~IUluai() = default;

    virtual bool init() = 0;
}; 

std::shared_ptr<unsigned char> read_model_2_buff(const char *file_path, size_t& model_size);