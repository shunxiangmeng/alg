#include "rknn.h"
#include "infra/include/Timestamp.h"
#include "infra/include/TimeDelta.h"
#include "infra/include/Logger.h"

Rknn::Rknn() {
}

Rknn::~Rknn() {
}

bool Rknn::init() {
    oac_client_->init(shared_from_this());
    if (!oac_client_->start()) {
        return false;
    }
    if (!initAlg()) {
        errorf("init alg faild\n");
        return false;
    }
    return infra::Thread::start();
}

inline const char* get_type_string(rknn_tensor_type type) {
    switch (type) {
    case RKNN_TENSOR_FLOAT32:
        return "FP32";
    case RKNN_TENSOR_FLOAT16:
        return "FP16";
    case RKNN_TENSOR_INT8:
        return "INT8";
    case RKNN_TENSOR_UINT8:
        return "UINT8";
    case RKNN_TENSOR_INT16:
        return "INT16";
    default:
        return "UNKNOW";
    }
}

inline const char* get_qnt_type_string(rknn_tensor_qnt_type type) {
    switch (type) {
    case RKNN_TENSOR_QNT_NONE:
        return "NONE";
    case RKNN_TENSOR_QNT_DFP:
        return "DFP";
    case RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC:
        return "AFFINE";
    default:
        return "UNKNOW";
    }
}

inline const char* get_format_string(rknn_tensor_format fmt) {
    switch (fmt) {
    case RKNN_TENSOR_NCHW:
        return "NCHW";
    case RKNN_TENSOR_NHWC:
        return "NHWC";
    default:
        return "UNKNOW";
    }
}

static void dump_tensor_attr(rknn_tensor_attr* attr) {
    printf("index=%d, name=%s, n_dims=%d, dims=[%d, %d, %d, %d], n_elems=%d, size=%d, fmt=%s, type=%s, qnt_type=%s, zp=%d, scale=%f\n",
            attr->index, attr->name, attr->n_dims, attr->dims[3], attr->dims[2], attr->dims[1], attr->dims[0],
            attr->n_elems, attr->size, get_format_string(attr->fmt), get_type_string(attr->type),
            get_qnt_type_string(attr->qnt_type), attr->zp, attr->scale);
}

bool Rknn::initAlg() {
#if 0
    const char* model_name = "/app/fs/models/ulu_4cls_v5n_no.rknn";
    infra::Buffer model = loadModelFile(model_name);
    if (model.empty()) {
        errorf("load model %s failed\n", model_name);
        return false;
    }
    infof("model size = %d\n", model.size());
    int ret = rknn_init(&rknn_context_, (void*)model.data(), (uint32_t)model.size(), 0);
    if (ret < 0) {
        errorf("rknn_init ret = %d\n", ret);
        return false;
    }
#else
    //std::string model_full_path = "/app/fs/models/ulu_4cls_v5n_no.rknn";
    //std::string model_full_path = "/app/fs/models/uluhf_5n.rknn";
    std::string model_full_path = "./models/FaceDetect_bosideng_v0.0.0.4_0922.ulu";
    size_t model_size = 0;
    std::shared_ptr<unsigned char> model_data = read_model_2_buff(model_full_path.c_str(), model_size);
    tracef("pose_model_size = %d\n", model_size);
    int ret = rknn_init(&rknn_context_, (void*)model_data.get(), (uint32_t)model_size, 0);
    if (ret < 0) {
        errorf("rknn_init ret = %d\n", ret);
        return false;
    }
#endif

    rknn_sdk_version version;
    ret = rknn_query(rknn_context_, RKNN_QUERY_SDK_VERSION, &version, sizeof(rknn_sdk_version));
    if (ret < 0) {
        errorf("rknn_init error ret=%d\n", ret);
        return false;
    }
    infof("rknn_api_version:%s, rknn_drv_version:%s\n", version.api_version, version.drv_version);

    rknn_input_output_num io_num;
    ret = rknn_query(rknn_context_, RKNN_QUERY_IN_OUT_NUM, &io_num, sizeof(io_num));
    if (ret < 0) {
        errorf("rknn_init error ret=%d\n", ret);
        return false;
    }
    infof("model input num: %d, output num: %d\n", io_num.n_input, io_num.n_output);

    rknn_tensor_attr input_attrs[io_num.n_input];
    memset(input_attrs, 0, sizeof(input_attrs));
    for (int i = 0; i < io_num.n_input; i++) {
        input_attrs[i].index = i;
        ret = rknn_query(rknn_context_, RKNN_QUERY_INPUT_ATTR, &(input_attrs[i]), sizeof(rknn_tensor_attr));
        if (ret < 0) {
            errorf("rknn_init error ret=%d\n", ret);
            return false;
        }
        dump_tensor_attr(&(input_attrs[i]));
    }

    rknn_tensor_attr output_attrs[io_num.n_output];
    memset(output_attrs, 0, sizeof(output_attrs));
    for (int i = 0; i < io_num.n_output; i++) {
        output_attrs[i].index = i;
        ret = rknn_query(rknn_context_, RKNN_QUERY_OUTPUT_ATTR, &(output_attrs[i]), sizeof(rknn_tensor_attr));
        dump_tensor_attr(&(output_attrs[i]));
        if (output_attrs[i].qnt_type != RKNN_TENSOR_QNT_AFFINE_ASYMMETRIC || output_attrs[i].type != RKNN_TENSOR_UINT8) {
            errorf("The Demo required for a Affine asymmetric u8 quantized rknn model, but output quant type is %s, output data type is %s\n",
                get_qnt_type_string(output_attrs[i].qnt_type), get_type_string(output_attrs[i].type));
            return false;
        }
    }

    int channel = 3;
    int width   = 0;
    int height  = 0;
    if (input_attrs[0].fmt == RKNN_TENSOR_NCHW) {
        infof("model is NCHW input fmt\n");
        width  = input_attrs[0].dims[0];
        height = input_attrs[0].dims[1];
    } else {
        infof("model is NHWC input fmt\n");
        width  = input_attrs[0].dims[1];
        height = input_attrs[0].dims[2];
    }
    infof("model input height=%d, width=%d, channel=%d\n", height, width, channel);
    return true;
}

void Rknn::run() {
    infof("start oac client test thread\n");
    int32_t count = 0;
    infra::Timestamp last_getstate_time = infra::Timestamp::now();
    while (running()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));

        oac::ImageFrame image;
        oac_client_->getImageFrame(image);
        //infof("oac client use image index:%d, pts:%lld, wxh[%dx%d], data:%p\n", image.index, image.timestamp, image.width, image.height, image.data);

        oac_client_->releaseImageFrame(image);
    }
}

std::string Rknn::version() {
    return "1.0.0.1";
}

std::string Rknn::sdkVersion() {
    return alg_sdk_version_;
}