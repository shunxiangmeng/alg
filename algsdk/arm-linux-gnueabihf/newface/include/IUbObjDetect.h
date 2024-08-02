#ifndef IUBObjDetect_h
#define IUBObjDetect_h

#include "UbCommon.h"
#include <vector>
#include <map>

namespace ulu_best{

    enum EObjDetect_Type{
        E_ObjDetect_Yolov5 = 1,               // 目标检测
        E_ObjDetect_Yolov5Landmark = 2,       // 目标+关键点检测
        E_ObjDetect_Yolov6 = 11,              // 暂无实现              
        E_ObjDetect_Yolov7 = 21,              // 暂无实现
        E_ObjDetect_Yolov8 = 31,              // 暂无实现
        E_ObjDetect_Max, 
    };

    enum EThreshold_Conf                   // 置信度（0-1）
    {
        ETHRE_Box = 1,                     // 检测框的置信度
        ETHRE_Class = 2,                   // 类别的置信度
        ETHRE_Point = 3,                   // 关键点的置信度 
        ETHRE_Nms = 4,                     // IOU
        ETHRE_Area = 5,                    // 区域重合度  
        ETHRE_ALL,                 
    };
    
    enum EDetect_ScoreMode                 // 分数计算方式
    {
        EScore_Threshold = 0x0001,         // 按置信度
        EScore_BoxArea = 0x0002,           // 按框面积
        EScore_Quality = 0x0004,           // 按图片质量    
    };

    enum EDetect_AREA{
        //最多可设置4个区域
        ERK_IN_AREA1 = 0X01,
        ERK_IN_AREA2 = 0X02,
        ERK_IN_AREA3 = 0X04,
        ERK_IN_AREA4 = 0X08,
    };

    enum EDetect_LINE{
        //最多可设置4个相交线
        ERK_IN_LINE1 = 0X01,
        ERK_IN_LINE2 = 0X02,
        ERK_IN_LINE3 = 0X04,
        ERK_IN_LINE4 = 0X08,
    };


#ifdef USE_OBJ_DETECT

    class IObjDetect
    {
    public:
        IObjDetect() {}
        virtual ~IObjDetect() {}

        // 模型初始化
        virtual EReturn_Code Init(const char * model_path, int gpu_id=0) = 0;
        virtual EReturn_Code Init(const unsigned char * buffer, int buffer_len, int gpu_id=0) = 0;

        // 设置瞄定框参数（不设置有默认值）
        virtual EReturn_Code SetAnchor(int* anachors, int cols, int rows) = 0;    
        // 设置检测列表数量（不设置默认1）
        virtual EReturn_Code SetNumClass(int nc) = 0; 
        // 设置全局置信度（不设置有默认值）
        virtual EReturn_Code SetThreshold(EThreshold_Conf thre, float score) = 0;
        // 设置单类的conf
        virtual EReturn_Code SetClsConf(int clsid, float conf) = 0;
        // 设置单类的nms
        virtual EReturn_Code SetClsNms(int clsid, float nms) = 0;
        // 设置单类的区域重合度
        virtual EReturn_Code SetClsAreaIou(int clsid, float iou) = 0;        
        // 设置单类分数计算方式
        virtual EReturn_Code SetClsScoreMode(int clsid, EDetect_ScoreMode mode) = 0;        
        // 关闭检测类别ID(坐标从0开始)
        virtual EReturn_Code DisableClass(int clsid) = 0;  
        // 关闭类别ID相交线检测(坐标从0开始)
        virtual EReturn_Code DisableClsLine(int clsid) = 0;                    
        // 设置检测区域
        virtual EReturn_Code SetRegion(std::vector<SUbPoint> &pts, EDetect_AREA idx) = 0;
        // 获取检测区域
        virtual EReturn_Code GetRegion(std::vector<SUbPoint> &pts, EDetect_AREA idx) = 0;
        // 设置相交线
        virtual EReturn_Code SetLine(std::vector<SUbPoint> &pts, EDetect_LINE idx) = 0;
        // 获取相交线
        virtual EReturn_Code GetLine(std::vector<SUbPoint> &pts, EDetect_LINE idx) = 0;        
        // 检测图片,返回检测目标数量
        virtual int Input(SUbImg &img, EImg_Process ep) = 0;
        // 获取结果
        virtual EReturn_Code Output(std::map<int, std::vector<SUbObjInfo>> &objs) = 0;
    };

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code CreateDetect(EObjDetect_Type et, IObjDetect ** ppObjDetect);
    EReturn_Code DestroyDetect(IObjDetect ** ppObjDetect);

#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif

};

#endif /* IUBObjDetect_h */