//
//  IULUObjDetect.h
//  FaceLib
//
//  Created by harris on 2022/6/28.
//  Copyright © 2022年 harris. All rights reserved.
//

#ifndef IULUObjDetect_h
#define IULUObjDetect_h

#include "IULUBaseType.h"

namespace ulu_face{
    struct SCommonObjectInfo : public SDetectObjectInfo
    {
        int class_id = -1;
        int keypoint_cnt = 0;
#define MAX_COMMON_OBJ_INFO_KEYPOINT_CNT 20
        float keypoints[MAX_COMMON_OBJ_INFO_KEYPOINT_CNT*2];//xxxxyyyy
        SCommonObjectInfo();
    };

    class IObjDetect
    {
    public:
        IObjDetect();
        virtual ~IObjDetect();
        virtual EReturn_Code Init(EModel_Type mt,const char * model_path)=0;
        virtual EReturn_Code Init(EModel_Type mt,const unsigned char * buffer,size_t buffer_len)=0;
        
        virtual EReturn_Code SetClassNum(int class_num) = 0;
        virtual int DetectObject(const SULUImage& image,std::vector<SCommonObjectInfo>& out_objinfo) = 0;
        virtual int DetectObjects(const std::vector<SULUImage>& images,std::vector< std::vector<SCommonObjectInfo> >& out_objinfos) = 0;
        
        virtual void SetMinWidth(int w);
        virtual void SetMinHeight(int h);
        virtual void SetMaxWidth(int w);
        virtual void SetMaxHeight(int h);
        
        virtual void SetTrack(bool enable) = 0;
        virtual int ResetRoute(int route_id) = 0;
        //设置多边形检测区域
        virtual bool SetRegion(const std::vector<SULUPoint>& pts,int idx = 0) = 0;
        
        virtual void SetRegionIOU(float iou) = 0;
    };

    extern "C" EReturn_Code CreateObjDetect(IObjDetect ** pObjDetect);
    extern "C" EReturn_Code DestroyObjDetect(IObjDetect ** pObjDetect);
};

#endif /* IULUObjDetect_h */
