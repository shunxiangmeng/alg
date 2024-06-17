//
//  IULUFace.h
//  FaceLib
//
//  Created by harris on 2018/7/29.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUFace_h
#define IULUFace_h

#include <vector>
#include <string>

#include "IULUBaseType.h"

namespace ulu_face{
    struct SHeadPose //均为弧度值(3.14,如果需要转换为角度值，则 * 180.0 / PI)
    {
        float yaw;
        float pitch;
        float roll;
        SHeadPose();
    };

    struct SFaceInfo : public SDetectObjectInfo
    {
        SHeadPose pose;
        float landmarks[10];//left_eyex,right_eyex, nosex, left_concerx,right_concerx   left_eyey,right_eyey,nosey,left_concery,right_concery
        unsigned int alive;
        int related_person_id;
        SFaceInfo();
    };
    
    struct SFaceAttr
    {
        int gender;//0--female, 1--male
        int age;//0~100
        int reserved_attr[10];
        SFaceAttr();
    };

#define MAX_PedestrianAttr_NUM 64
    struct SPedestrianAttr
    {
        int attr_num = 0;
        int attrs[MAX_PedestrianAttr_NUM];
        float attr_scores[MAX_PedestrianAttr_NUM];
        SPedestrianAttr();
    };

    typedef struct SFeature SFaceFeature;

    struct SFaceAndPedestrianInfo
    {
        std::vector<SFaceInfo> face_info;
        std::vector<SDetectObjectInfo> pedestrian_info;
        std::vector<int> face_drop_ids;
        std::vector<int> pedestrian_drop_ids;
        int need_detect = 1;
    };

    //人形检测算法，用于算法切换
    enum EPersonDetectAlgor_Type{
        E_PDA_1 = 1,
        E_PDA_2 = 2,
        E_PDA_3 = 3,
        E_PDA_4 = 4,
        E_PDA_Max,
    };

    typedef EPersonDetectAlgor_Type EFaceDetectAlgor_Type;
    
    class IFaceDetect
    {
    public:
        IFaceDetect();
        virtual ~IFaceDetect();
        
        //设置人形检测算法
        virtual EReturn_Code SetPersonDetectAlgorithm(EPersonDetectAlgor_Type algo) = 0;
        
        //设置人脸检测算法
        virtual EReturn_Code SetFaceDetectAlgorithm(EFaceDetectAlgor_Type algo) = 0;
        
        virtual EReturn_Code Init(EModel_Type mt,const char * model_path)=0;
        virtual EReturn_Code Init(EModel_Type mt,const unsigned char * buffer,size_t buffer_len)=0;

        //virtual EReturn_Code Init(const char * model_path,int maxsize = 400,int minsize = 20)=0;
        //以下两个函数，只是用来检测人脸，没有跟踪功能
        virtual int DetectFaceBase(const SULUImage& image,std::vector<SBBox>& out_faceinfo) = 0;
        virtual int DetectFaceBases(const std::vector<SULUImage>& images,std::vector< std::vector<SBBox> >& out_faceinfos) = 0;

        virtual int DetectFaceWithPedestrian(const SULUImage& image,std::vector<SFaceInfo>& out_faceinfo,std::vector<SDetectObjectInfo>& out_pedestrian_info) = 0;
        virtual int DetectFaceWithPedestrians(const std::vector<SULUImage>& images,std::vector< std::vector<SFaceInfo> >& out_faceinfos,
            std::vector< std::vector<SDetectObjectInfo> >& out_pedestrian_infos) = 0;
        
        virtual int DetectFaceWithPedestrian(const SULUImage& image,SFaceAndPedestrianInfo& out_info) = 0;

        virtual bool SetRegion(int lt_x,int lt_y,int rb_x,int rb_y,int idx = 0) = 0;
        
        //设置多边形检测区域
        virtual bool SetRegion(const std::vector<SULUPoint>& pts,int idx = 0) = 0;
        
        virtual void SetRegionIOU(float iou) = 0;
        
        virtual int GetFaceInfo(const SULUImage& image,std::vector<SFaceInfo>& out_faceinfo) = 0;
        virtual int CalcImageQuality(const SULUImage& image,std::vector<SDetectObjectInfo>& out_det_obj_info) = 0;
        virtual int AlignFace(const SULUImage& image,const SBBox& box,const float face_landmarks[],SULUImage& out_image) = 0;
        virtual int AlignFace(const SULUImage& image,const SFaceInfo& in_faceinfo,SULUImage& out_image) = 0;
        virtual int AlignFaces(const SULUImage& image,const std::vector<SFaceInfo>& in_faceinfos,std::vector<SULUImage>& out_images) = 0;
        
        virtual int ResetRoute(int route_id) = 0;
    };

    class IFaceAttr : public IFaceDetect
    {
    public:
        IFaceAttr();
        virtual ~IFaceAttr();

        virtual int FaceAttr(const SULUImage& image,SFaceAttr& out_attr) = 0;
        virtual int FaceAttrs(const std::vector<SULUImage>& images,std::vector<SFaceAttr>& out_attrs) = 0;

        const int gender_list_[2] = {0,1};
        const std::string gender_str_list_[2] = {"female","male"};
    };

    class IPedestrianAttr : public IFaceAttr
    {
    public:
        IPedestrianAttr();
        virtual ~IPedestrianAttr();
        
        virtual int PedestrianAttr(const SULUImage& image,SPedestrianAttr& out_attr) = 0;
        virtual int PedestrianAttrs(const std::vector<SULUImage>& images,std::vector<SPedestrianAttr>& out_attrs) = 0;

    };
    
    class IFaceRecognition : public IPedestrianAttr
    {
    public:
        IFaceRecognition();
        virtual ~IFaceRecognition();
        
        virtual float GetReferenceThreshold() = 0;

        virtual int CalcFaceFeature(const SULUImage& align_image,SFaceFeature& out_feature) = 0;
        virtual int CalcFaceFeatures(const std::vector<SULUImage>& align_images,std::vector<SFaceFeature>& out_features) = 0;

        //virtual float CalcDiff(SFaceFeature& feature1,SFaceFeature& feature2) = 0;
        virtual void SetThreshold(float threshold) = 0;
        //virtual float ConvertDiff2Similarity(float diff) = 0;
        
        virtual float CalcSimilarity(const SFaceFeature& feature1,const SFaceFeature& feature2) = 0;
    };

    extern "C" EReturn_Code CreateFaceRecognition(IFaceRecognition ** pFaceRecognition);
    extern "C" EReturn_Code DestroyFaceRecognition(IFaceRecognition ** pFaceRecognition );
    extern "C" EReturn_Code SetMaxFaceSize(int height,int width);
    extern "C" EReturn_Code SetMinFaceSize(int height,int width);
    extern "C" EReturn_Code SetMaxPedestrianSize(int height,int width);
    extern "C" EReturn_Code SetMinPedestrianSize(int height,int width);
    extern "C" EReturn_Code SetFaceDetectThreshold(float threshold);
    extern "C" EReturn_Code SetFacePedestrianThreshold(float threshold);

};

#endif /* IULUFace_h */
