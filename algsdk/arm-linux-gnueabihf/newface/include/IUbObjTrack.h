#ifndef IUBObjTrack_h
#define IUBObjTrack_h

#include "UbCommon.h"
#include <vector>

namespace ulu_best{

    enum ETrack_Type{
        E_Track_Sort = 1,                 // sort 跟踪
        E_Track_DeepSort = 2,             // deepsort 跟踪 
        E_Track_Byte = 3,                 // byte 跟踪
        E_Track_Ulucu = 4,                // ulucu 跟踪
    };    

    enum ETrack_Param{
        ETrack_Param_Score = 1,           // 跟踪分数
        ETrack_Param_Direction = 2,       // 方向 
        ETrack_Param_Stime = 4,           // 开始时间 

        ETrack_Param_Max,
    };    

    struct SUbTrackCfg
    {
        short scene;                           // 场景 1-排它性区域跟踪
        short min_hits;                        // 未命中时间下限
        int max_age;                           // 未命中时间上限

        float iou_thre;                        // iou阈值
        float score;                           // 分数
        
        float width;                           // 长
        float height;                          // 宽
        float critical_distance;               // 小于临界距离，需特殊处理
        
        int limitTimeDuration;                 // 限制时间范围 单位毫秒(ms) 
        int limitCallCount;                    // 限制调用次数 
        int maxBudget;                         // 最大预算  
        float maxCosineDist;                   // 最大余弦距离        
        char model_path[UB_MAX_PATH_LEN];      // 模型路径
        int gpu_id;
    }; 

#ifdef USE_TRACK

    // 单类别多目标跟踪
    class IObjTrack
    {
    public:
        IObjTrack() {}
        virtual ~IObjTrack() {}

        // 初始化
        virtual EReturn_Code Init(SUbTrackCfg& cfg) = 0;
        
        // 更新跟踪位置,返回丢失id数量
        virtual int Update(SUbImg &img, std::vector<SUbObjInfo> &objs) = 0;
        // 获取丢失的目标id
        virtual EReturn_Code Loss(std::vector<int> &loss_ids) = 0;
        // 获取丢失的目标数据
        virtual EReturn_Code LossData(std::vector<SUbLossInfo> &loss_datas) = 0;
        // 获取跟踪信息
        virtual EReturn_Code GetParam(int id, ETrack_Param etp, void* param) = 0;
    };

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code CreateTrack(ETrack_Type et, IObjTrack ** ppObjTrack);
    EReturn_Code DestroyTrack(IObjTrack ** ppObjTrack);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif 

};

#endif /* IUBObjTrack_h */