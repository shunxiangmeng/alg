#ifndef IUBLandmarkDetect_h
#define IUBLandmarkDetect_h

#include "UbCommon.h"
#include <vector>

namespace ulu_best{

    enum ELandmarkDetect_Type{
        E_LandmarkDetect_Face = 1,               // 人脸关键点检测
    };

#ifdef USE_LANDMARK_DETECT

    class ILandmarkDetect
    {
    public:
        ILandmarkDetect() {}
        virtual ~ILandmarkDetect() {}

        // 模型初始化
        virtual EReturn_Code Init(const char * model_path, int gpu_id=0) = 0;
        virtual EReturn_Code Init(const unsigned char * buffer, int buffer_len, int gpu_id=0) = 0;
        
        // 检测单张图片多个区域
        virtual int GetLandmarks(SUbImg &img, std::vector<SUbObjInfo> &objs) = 0;
        // 检测单张图片
        virtual int GetLandmark(SUbImg &img, SUbLandmarkInfo &ldk) = 0;
        
    };

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code CreateLandmarkDetect(ELandmarkDetect_Type et, ILandmarkDetect ** ppLandmark);
    EReturn_Code DestroyLandmarkDetect(ILandmarkDetect ** ppLandmark);

#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif

};

#endif /* IUBLandmarkDetect_h */