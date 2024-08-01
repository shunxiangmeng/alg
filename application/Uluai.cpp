/************************************************************************
 * Copyright(c) 2024 shanghai ulucu technology
 * 
 * File        :  Uluai.cpp
 * Author      :  mengshunxiang 
 * Data        :  2024-06-17 15:05:11
 * Description :  None
 * Note        : 
 ************************************************************************/
#include <fstream>
#include "Uluai.h"
#include "Fmix.h"
#include "Behavior.h"
#include "InOut.h"
#include "rknn.h"

std::shared_ptr<IUluai> IUluai::create(E_ALG_TYPE alg_type) {
    switch (alg_type) {
        case E_ALG_Fmix:
            return std::make_shared<Fmix>();
        case E_ALG_XW:
            return std::make_shared<Behavior>();
        case E_ALG_CRK:
            return std::make_shared<InOut>();
        case E_ALG_RKNN:
            return std::make_shared<Rknn>();
        default:
            return nullptr;
    }
}

IUluai::IUluai() : oac_client_(oac::IOacClient::instance()) {
}

std::shared_ptr<unsigned char> read_model_2_buff(const char *file_path, size_t& model_size) {
    std::shared_ptr<unsigned char> p_buff;
    do {
        std::ifstream ifs_model(file_path, std::ios::binary);
        if (!ifs_model.good()) {
            break;
        }
        ifs_model.seekg(0, ifs_model.end);
        model_size = ifs_model.tellg();
        p_buff = std::shared_ptr<unsigned char>((unsigned char*)::malloc(model_size), ::free);
        ifs_model.seekg(0, ifs_model.beg);
        ifs_model.read((char*)(p_buff.get()), model_size);
        ifs_model.close();
    } while (0);
    return p_buff;
}

infra::Buffer loadModelFile(const char *file_path) {
    infra::Buffer buffer;
    do {
        std::ifstream ifs_model(file_path, std::ios::binary);
        if (!ifs_model.good()) {
            break;
        }
        ifs_model.seekg(0, ifs_model.end);
        int32_t model_size = (int32_t)ifs_model.tellg();
        if (!buffer.ensureCapacity(model_size)) {
            ifs_model.close();
            break;
        }
        ifs_model.seekg(0, ifs_model.beg);
        ifs_model.read((char*)(buffer.data()), model_size);
        ifs_model.close();
        buffer.setSize(model_size);
    } while (0);
    return buffer;
}