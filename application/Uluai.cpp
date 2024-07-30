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

std::shared_ptr<IUluai> IUluai::create(E_ALG_TYPE alg_type) {
    switch (alg_type) {
        case E_ALG_Fmix:
            return std::make_shared<Fmix>();
        case E_ALG_XW:
            return std::make_shared<Behavior>();
        default:
            return nullptr;
    }
}

IUluai::IUluai() : oac_client_(oac::IOacClient::instance()) {
}

void IUluai::pushDetectTarget(oac::ImageFrame &image, std::vector<ulu_face::SPersonInfo> &detect_result) {
    CurrentDetectResult result;
    result.timestamp = image.timestamp;

    for (int i = 0; i < detect_result.size(); i++) {
        auto &it = detect_result[i];
        //tracef("i:%d, face_score:%0.3f, box[%0.1f, %0.1f, %0.1f, %0.1f], ped_score:%0.3f, box[%0.1f, %0.1f, %0.1f, %0.1f]\n", 
        //    it.face_score, it.face_info.box.lt_x(), it.face_info.box.lt_y(), it.face_info.box.width(), it.face_info.box.height(),
        //    it.pedestrian_score, it.pedestrian_info.box.lt_x(), it.pedestrian_info.box.lt_y(), it.pedestrian_info.box.width(), it.pedestrian_info.box.height());

        if (it.face_score > 0.1) {
            Target target;
            target.type = E_TargetType_face;
            target.id = it.face_info.trace_id;
            target.rect.x = it.face_info.box.lt_x() / image.width;
            target.rect.y = it.face_info.box.lt_y() / image.height;
            target.rect.w = it.face_info.box.width() / image.width;
            target.rect.h = it.face_info.box.height() / image.height;
            result.targets.push_back(target);
        }

        if (it.pedestrian_score > 0.1) {
            Target target;
            target.type = E_TargetType_body;
            target.id = it.pedestrian_info.trace_id;
            target.rect.x = it.pedestrian_info.box.lt_x() / image.width;
            target.rect.y = it.pedestrian_info.box.lt_y() / image.height;
            target.rect.w = it.pedestrian_info.box.width() / image.width;
            target.rect.h = it.pedestrian_info.box.height() / image.height;
            result.targets.push_back(target);
        }

        oac_client_->pushCurrentDetectTarget(result);
    }
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
