#ifndef UBCommon_h
#define UBCommon_h

#ifdef _WINDOWS
#ifdef LIBULU_EXPORTS
#define ULU_SDK_API __declspec(dllexport)
#else
#define ULU_SDK_API __declspec(dllimport)
#endif
#else	// _WINDOWS
#define ULU_SDK_API
#endif	// _WINDOWS

#include "UbConfig.h"
#include <string.h>

//#define USE_OPENCV
#ifdef USE_OPENCV   
#include <opencv2/opencv.hpp>
#endif

namespace ulu_best{

    #define UB_MAX_PATH_LEN         (256)       // 最大路径长度
    #define UB_MAX_AREA_LIMIT_COUNT (4)         // 最大区域数量
    #define UB_MAX_OBJMARKS_NUM     (5)         // 最大关键点数量 
    #define UB_MAX_OBJDETECT_NUM    (64)        // 最大目标检测数量
    #define UB_MAX_CLASSDETECT_NUM  (16)        // 最大检测目标类别数量
    
    enum EReturn_Code{
        EC_OK = 0,
        EC_FAIL = -1,//运行失败，内部错误
        EC_SYSTEM_UNSUPPORTED = -2,//所依赖的操作系统或者环境不支持此SDK
        EC_UNSUPPORTED = -3,//不支持的函数调用方式
        EC_UNIMPLEMENT = -4,//暂未实现的函数调用方式
        EC_INVALIDARG = -5,//无效参数
        EC_OUTOFMEMORY = -6,//内存不足
        EC_DELNOTFOUND = -7,//定义缺失
        EC_INVALID_PIXEL_FORMAT = -8,//不支持的图像格式
        EC_RESOURCE_NOT_FOUND = -9,//资源不存在
        EC_INVALID_RESOURCE_FORMAT = -10,//模型格式不正确导致加载失败
        EC_RESOURCE_EXPIRE = -11,//模型文件过期
        EC_AUTH_INVALID = -12,//license 不合法
        EC_AUTH_EXPIRE = -13,//license已过期
    };

    enum EPixel_Format{
        EPIX_FMT_UNDEFINE = 0x00,

        // RGB
        EPIX_FMT_BGR888 = 0x0001,          // bgr bgr bgr
        EPIX_FMT_RGB888 = 0x0002,          // rgb rgb rgb 

        // YUV
        EPIX_FMT_YUV420P = 0x0101,          // yyyyuuvv
    };

    enum EMindUnit_type
    {
        EMindUnit_Class = 1,
        EMindUnit_ObjDetect = 2,
        EMindUnit_LdmDetect = 3,
        EMindUnit_Track = 4,
        EMindUnit_Feature = 5,
        EMindUnit_Regco = 6,
        EMindUnit_HotZone = 7,
    };

    enum EImg_Process
    {
        EP_Nome = 0,              // 无需处理|默认处理
        EP_Resize = 1,            // resize
        EP_Letterbox = 2,         // letterbox   
    };

    struct SUbImg{
#ifndef USE_OPENCV        
        EPixel_Format  pixel_format = EPIX_FMT_UNDEFINE;
        int            width;
        int            height;
        unsigned char* buffer[3];	// 如果是rgb，设置buffer[0]即可        
	    int			   stride[3];	// 如果是rgb，可以不设置
#else
        cv::Mat        src;
#endif // USE_OPENCV       
        long long      time;            
    };

    struct SUbPoint{
        float x;
        float y;
    };

    struct SUbRect{
        float x;
        float y;
        float width;
        float height;
    };

    struct SUbBox {
        float bbox[4];     // x1,y1,x2,y2
    };

    struct SUbLandmarkInfo
    {
        int pt_cnt;        // 关键点数量
        float pt_score;    // point score
        SUbPoint keypts[UB_MAX_OBJMARKS_NUM]; 
        SUbPoint* extra_pt; // pt_cnt > UB_MAX_OBJMARKS_NUM 会用到
    };

    struct SUbObjInfo
    {
        float bbox[4];     // x1,y1,x2,y2
        float box_thre;    // box threshold  (0 - 1)  
        int clsid;         // 类别id
        float cls_thre;    // class threshold (0 - 1) 
        int areaid;        // 重合度最高的区域id
        float area_iou;    // area iou (区域重合度 = 重合面积/检测框面积)
        float score;       // 分数
        int objid;         // 跟踪id
        long long first_time;   // 跟踪开始时间  

        SUbLandmarkInfo ldk;  // 关键点信息       
    };

    struct SUbLossInfo
    {
        int track_id;                          // 跟踪id 
        int track_count;                       // 跟踪次数

        long long first_time;                  // 开始时间 
        long long last_time;                   // 结束时间
    };

};

#endif /* UBCommon_h */
