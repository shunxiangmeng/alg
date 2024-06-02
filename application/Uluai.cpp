#include <fstream>
#include "Uluai.h"


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
