//
//  IULUBaseType.h
//  FaceLib
//
//  Created by harris on 2022/7/11.
//  Copyright © 2022年 harris. All rights reserved.
//

#ifndef IULUBaseType_h
#define IULUBaseType_h

#include <string.h>
#include <vector>

namespace ulu_face{
    enum EModel_Type{
        E_MT_FaceDetect = 1,
        E_MT_FacePose = 2,
        E_MT_FaceAttr = 3,
        E_MT_FaceReg = 4,
        E_MT_PersonDetect = 5,
        E_MT_ReID = 6,
        E_MT_PersonPose = 7,
        E_MT_PersonAttr = 8,
        
        E_MT_CascadeClassifier =  31,
        
        E_MT_ObjectDetect_YOLOv5 = 101,
        
        E_MT_CarPlateDetect = 201,
        E_MT_CarPlateReg = 202,
        E_MT_CarPlateType = 206,
        
        E_MT_SPEECH_REC = 301,
        E_MT_SPEECH_REC_LM = 302,
        E_MT_SPEECH_REC_LM_VOCABULARY = 303,
        
        E_MT_OCRDetect = 401,
        E_MT_OCRCls = 402,
        E_MT_OCRReg = 403,
    };

    enum EReturn_Code{
        E_OK = 0,
        E_FAIL = -1,//运行失败，内部错误
        E_SYSTEM_UNSUPPORTED = -2,//所依赖的操作系统或者环境不支持此SDK
        E_UNSUPPORTED = -3,//不支持的函数调用方式
        E_UNIMPLEMENT = -4,//暂未实现的函数调用方式
        E_INVALIDARG = -5,//无效参数
        E_OUTOFMEMORY = -6,//内存不足
        E_DELNOTFOUND = -7,//定义缺失
        E_INVALID_PIXEL_FORMAT = -8,//不支持的图像格式
        E_RESOURCE_NOT_FOUND = -9,//资源不存在
        E_INVALID_RESOURCE_FORMAT = -10,//模型格式不正确导致加载失败
        E_RESOURCE_EXPIRE = -11,//模型文件过期
        E_AUTH_INVALID = -12,//license 不合法
        E_AUTH_EXPIRE = -13,//license已过期
    };

    enum EPixelFormat{
        EPIX_FMT_UNDEFINE = 0,
        EPIX_FMT_GRAY8 = 1,//CV_8UC1
        EPIX_FMT_BGR888 = 2,//CV_8UC3&bgr
        EPIX_FMT_RGB888 = 3,//CV_8UC3&rgb
        
        EPIX_FMT_BGRA8888 = 4,//CV_8UC4&bgra
        EPIX_FMT_RGBA8888 = 5,//CV_8UC4&rgba
        
        //YUV 4:2:0 family
        EPIX_FMT_YUV420P = 6,//YV12//yyyyvvuu
        EPIX_FMT_YUV420P_YV12 = EPIX_FMT_YUV420P,//yyyyvvuu
        EPIX_FMT_YUV420P_I420 = 7,
        EPIX_FMT_YUV420P_YU12 = EPIX_FMT_YUV420P_I420,//yyyyuuvv
        EPIX_FMT_YUV420SP = 8,
        EPIX_FMT_YUV420SP_NV21 = EPIX_FMT_YUV420SP,//yyyyvuvu
        EPIX_FMT_YUV420SP_NV12 = 9,//yyyyuvuv
        
        EFMT_AUDIO_FP32 = 10,
        EFMT_AUDIO_INT8 = 11,
        
        EFMT_COMMON_FP32 = EFMT_AUDIO_FP32,
        EFMT_COMMON_INT8 = EFMT_AUDIO_INT8,
        
        EFMT_MAX,
    };

#define MAX_ROUTE_NUM 4//最多支持四路并行

    struct STimestamp {
        long int tv_sec;//second
        long int tv_usec;//microsecond
        //1 second=1000 millisecond
        //1 millisecond=1000 microsecond
        STimestamp();
        bool empty()const;
        bool operator < (const STimestamp& cmp)const;
        bool operator == (const STimestamp& cmp)const;
        bool operator >= (const STimestamp& cmp)const;
        bool operator <= (const STimestamp& cmp)const;
    };

    typedef unsigned long long ulu_u64_t;
    typedef unsigned char ulu_uchat_t;

    struct SULUPoint{
        float x;
        float y;
        SULUPoint();
        SULUPoint(float x1,float y1);
    };

    struct SULUSize{
        float width;
        float height;
        SULUSize();
        SULUSize(float w1,float h1);
        bool is_valid() const;
    };

    struct SULURect{
        float x;
        float y;
        float width;
        float height;
        SULURect();
        SULURect(float x1,float y1,float w1,float h1);
        bool is_valid() const;
        void scale(float w_scale_rate,float h_scale_rate);
    };

    struct SULUImage{
        EPixelFormat pixel_format = EPIX_FMT_UNDEFINE;

        int height = -1;
        int width = -1;
        
        int stride = -1;
        STimestamp time_stamp;
        int route_id = 0;
        
        int data_owner_flag = 0;
        
        //该参数表示有效的目标区域
        SULURect target_rect;
        
        //如果是rgb或者gbr格式，则只需要将vir_data_array[0]赋值为数据块的指针即可
        //如果是yuv420p，则需要将vir_data_array[0]、vir_data_array[1]、vir_data_array[2]分别赋值为Y\U\V的数据块的指针
        //如果是yuv420sp，则需要将vir_data_array[0]、vir_data_array[1]分别赋值为Y\UV的数据块的指针
        int data_sizes[3] = {0,0,0};
        ulu_uchat_t * vir_addrs[3] = {0,0,0};
        ulu_u64_t phy_addrs[3] = {0,0,0};
        
        SULUImage();
        //此构造函数会分配内存空间用来存放数据
        SULUImage(EPixelFormat fmt,int h,int w);
        ~SULUImage();
        bool empty() const;
        
        //在vector等容器中，使用retain来赋值
        void retain(SULUImage& img);
        //仅仅只是引用img
        void refer(const SULUImage& img);
        void clone(const SULUImage& img);
        
        bool is_valid()const;
        //判断两个img是不是同一个
        bool is_same(const SULUImage& img)const;
        
        void reset();
        SULUImage(const SULUImage& img);
        SULUImage& operator=(const SULUImage& img);
    };

    struct SBBox
    {
        float score;
        float bbox[4];//y1,x1,y2,x2
        SBBox();
        float width() const;
        float height() const;
        float area() const;
        float lt_x() const;
        float lt_y() const;

        SULUPoint get_center()const ;

        bool operator < (const SBBox& cmp)const;
    };

    struct SDetectObjectInfo
    {
        SBBox box;
        float clarity;//清晰度，范围0-1
        unsigned int trace_id;
        unsigned int attr;
        SDetectObjectInfo();
        float width() const;
        float height() const;
        float area() const;
        float lt_x() const;
        float lt_y() const;
        SULUPoint get_center() const;
        bool operator < (const SDetectObjectInfo& cmp)const;
    };

#define MAX_AREA_LIMIT_COUNT 8
    enum EDetectObjectAttr{
        //最多可设置八个区域
        EDOA_IN_AREA = 0X01,
        EDOA_IN_AREA1 = 0X01,
        EDOA_IN_AREA2 = 0X02,
        EDOA_IN_AREA3 = 0X04,
        EDOA_IN_AREA4 = 0X08,
        EDOA_IN_AREA5 = 0X10,
        EDOA_IN_AREA6 = 0X20,
        EDOA_IN_AREA7 = 0X40,
        EDOA_IN_AREA8 = 0X80,
    };

    struct SFeature
    {
        int feature_cnt;
#define Max_ULU_Feature_COUNT 2048
        float features[Max_ULU_Feature_COUNT];
        SFeature();
    };

    enum ECVAlgor_Type{
        ECVAT_Base = 0,
        ECVAT_Hardware = 1,
        ECVAT_Ext1 = 2,
        ECVAT_Max,
    };

    extern "C" EReturn_Code GetVersion(char szVersion[16]);
    extern "C" EReturn_Code SDKInit(const char *config_file);
    extern "C" EReturn_Code AddLicense(const char *license_file);
    
    extern "C" EReturn_Code EnableDebugInfo();

    extern "C" EReturn_Code SetDefaultPixelFormat(EPixelFormat fmt);
    extern "C" EReturn_Code SetTrackMaxAge(int age);
    extern "C" EReturn_Code SetTrackTentativeAge(int age);
    extern "C" EReturn_Code SetLogo(const char * logo_file);
    extern "C" EReturn_Code AddWatermark(const SULUImage& image,SULUImage& out_image,float weight=0.4);
    
    extern "C" EReturn_Code SetModelPath(const char* model_path);
    extern "C" EReturn_Code SetModelName(EModel_Type mt,const char* model_name);
    extern "C" EReturn_Code SetModel(EModel_Type mt,const unsigned char * buffer,size_t buffer_len);

    extern "C" EReturn_Code PadImage(const SULUImage& image,SULUImage& out_image,int pad_top,int pad_bottom,int pad_left,int pad_right);
    extern "C" EReturn_Code CropImage(const SULUImage& image,SULUImage& out_cropped,int x,int y,int w,int h);
    extern "C" EReturn_Code ResizeImage(const SULUImage& image,SULUImage& out_resized,int w,int h);
    extern "C" float CalcImageQuality(const SULUImage& image);
    extern "C" EReturn_Code CalcImageQualitys(const SULUImage& image,std::vector<SBBox>& in_boxes,std::vector<float>& out_claritys);

    extern "C" void GetCurrentTimestamp(STimestamp& tv);
    extern "C" float GetExpireSeconds(const STimestamp& t_start,const STimestamp& t_end);
    extern "C" EReturn_Code SetCVAlgor(ECVAlgor_Type type);
};

#endif /* IULUBaseType_h */
