#ifndef IUBMixDetecter_h
#define IUBMixDetecter_h

#include "UbCommon.h"

#ifdef USE_MIX_DETECT
#include <vector>

namespace ulu_best{

    enum EDetect_MixType
    {
        EMixType_CarPlate = 1,              // cls1-车 cls2-车牌
        EMixType_PersonFace = 2,            // cls1-人 cls2-人脸
        EMixType_Max,            // 
    };

    struct SUbMixDetectCfg
    {
        char model_path[UB_MAX_PATH_LEN];      // 模型路径
        int gpu_id;                            // 设备id 
        // 设置瞄定框参数
        int anchors[18];                         
        int anchor_cols;
        int anchor_rows; 
        float nms;                              // 非极大抑值
        int nc;                                 // 检测类别数量
        int cls1_id;                            // 类别1，例如车类别id
        float cls1_conf;                        // 类别1的置信度
        int cls2_id;                            // 类别2，例如车牌类别id
        float cls2_conf;                        // 类别2的置信度
        int track_cls_id;                       // 需要跟踪的类别id 
    };

    struct SUbMixEvalCfg
    {
        // 置信度总分  【15.0】  
        float conf_score;
        // 达标的置信度 【0.5】
        float good_conf;
        // 宽高比总分  【25.0】
        float scale_score;
        // 好的宽高比上限 【3.8】
        float good_up_scale;
        // 好的宽高比下限 【2.8】
        float good_down_scale;
        // 图片质量总分 【50.0】
        float quality_score;
        // 面积总分 【5.0】
        float area_score;
        // 达标的面积大小 【7500.0】
        float good_area;
        // 达标的宽大小 【40.0】
        float good_width;        
        // 达标的高大小 【100.0】
        float good_height;                     
        // 跟踪总分 【5.0】
        float track_score;
        // 加分项总分<比如有人脸> 【8.0】
        float pluse_score;
        // 比如人脸大小 【3000.0】
        float good_pluse;
    };

    struct SUbMixInfo {
        SUbObjInfo track_obj;
        int second_clsid;         // 车牌框类别id
        float second_cls_thre;    // class threshold (0 - 1)         
        float second_bbox[4];     // x1,y1,x2,y2
    };

    class IMixDetect
    {
    public:
        IMixDetect() {}
        virtual ~IMixDetect() {}

        // 获取单元部件
        virtual void* GetMixUnit(EMindUnit_type eUnit) = 0;
        // 设置检测参数
        virtual EReturn_Code SetDetectCfg(void* cfg) = 0;
        // 设置跟踪参数
        virtual EReturn_Code SetTrackCfg(void* cfg) = 0;
        // 设置分数评估参数
        virtual EReturn_Code SetEvalCfg(void* cfg) = 0;
        // 检测跟踪图片
        virtual int Update(SUbImg &img, std::vector<SUbObjInfo> &objs) = 0;
        virtual int UpdateEx(SUbImg &img, std::vector<SUbMixInfo> &objs) = 0;
        // 获取丢失的目标id
        virtual EReturn_Code Loss(std::vector<int> &loss_ids) = 0;
        // 获取丢失的目标数据
        virtual EReturn_Code LossData(std::vector<SUbLossInfo>& loss_datas) = 0;
    };

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code CreateMixDetect(IMixDetect ** ppMix, EDetect_MixType type);
    EReturn_Code DestroyMixDetect(IMixDetect ** ppMix);

#ifdef __cplusplus
}
#endif /* __cplusplus */    

};

#endif

#endif /* IUBMixDetecter_h */