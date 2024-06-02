//
//  IULUPose.h
//  FaceLib
//
//  Created by harris on 2018/7/29.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUPose_h
#define IULUPose_h

#include "IULUBaseType.h"

#define ULU_MAX_POSE_KEYPOINTS_COUNT 128

namespace ulu_face{
    struct SPoseEstimation
    {
        SBBox box;
        int keypoint_num = 18;
        float keypoints[ULU_MAX_POSE_KEYPOINTS_COUNT*2];//xyxyxyxy......
        float scores[ULU_MAX_POSE_KEYPOINTS_COUNT];
        
        SPoseEstimation(){
            memset(keypoints,0,sizeof(keypoints));
            memset(scores,0,sizeof(scores));
        }
    };

    enum EPoseAlgor_Type{
        E_PA_PersonPose_Openpose = 1,
        E_PA_PersonPose_Light_Openpose = 2,
        E_PA_PersonPose_Other1 = 3,
        E_PA_PersonPose_Other2 = 4,
        E_PA_Pose_Max,
    };

    enum EPoseKeyPoint_Type{
        E_PKP_PersonPose_COCO18 = 1,
        E_PKP_PersonPose_COCO17 = 2,
        E_PKP_Pose_Max,
    };
    
    typedef void(*GetPoseCallBackFunc)(SULUImage&,int&,std::vector<SPoseEstimation>&,void* user_data) ;
    class IULUPose
    {
    public:
        IULUPose(){}
        virtual ~IULUPose(){}
        
        virtual EReturn_Code SetPoseAlgorithmType(EPoseAlgor_Type pat) = 0;
        virtual EReturn_Code SetPoseKeyPointType(EPoseKeyPoint_Type pkpt) = 0;
        virtual EReturn_Code SetThreshould(float threshould) = 0;
        
        virtual EReturn_Code Init(EModel_Type mt,const char * model_path)=0;
        virtual EReturn_Code Init(EModel_Type mt,const unsigned char * buffer,size_t buffer_len)=0;
        
        //该函数传进一张原始图片，返回图片中所有可能的人形及对应的姿态
        virtual int GetPose(const SULUImage& image,std::vector<SPoseEstimation>& poses) = 0;

        //这两个函数用来异步执行GetPose
        //首先需要注册一个回调函数，用来在NPU处理完成之后，获取得到的数据。
        virtual void RegisterCallBackFunc(GetPoseCallBackFunc func) = 0;
        virtual int GetPoseAsync(const SULUImage& image,std::vector<SPoseEstimation>& poses,void* user_data) = 0;
        
        //当设置为异步回调时，SDK内部将会缓存图片并依次处理。但当图片累计到一定程度时，很显然这并不合理。
        //此函数用来指定SDK内部缓存图片的张数。当缓存图像达到此长度时，继续存入图片，会自动扔掉更早的图像。
        //目前默认值为2。当设置大于10的值时，会设置失败
        virtual EReturn_Code SetAsyncImageQueueLen(int len) = 0;
    };

    extern "C" EReturn_Code CreatePose(IULUPose ** pPose);
    extern "C" EReturn_Code DestroyPose(IULUPose ** pPose );
};

#endif /* IULUPose_h */
