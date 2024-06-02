//
//  IULUPerson.h
//  FaceLib
//
//  Created by harris on 2022/9/29.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUPerson_h
#define IULUPerson_h

#include "IULUBaseType.h"
#include "IULUFace.h"

namespace ulu_face{
    enum EScene{
        ES_CommonStore = 0x1,//有容纳空间的门店，例如服装店、餐厅
        ES_RoadsideStore = 0x2,//路边小店，沿街客流，例如奶茶店、包子店
        ES_GE60DegreeObliqueStore = 0x03,//近景客流，当某些门店受限于安装条件，摄像头安装角度大于等于60度的倾斜度，此时只能通过人形来判断客流
        ES_MAX_STORE_TYPE,
    };

    //移动方向
    enum EDirection{
        ED_UNKNOWN = 0,
        ED_TOP = 1,
        ED_BOTTOM = 2,
        ED_LEFT = 3,
        ED_RIGHT = 4,
        
        ED_TOP_LEFT = 5,
        ED_TOP_RIGHT = 6,

        ED_BOTTOM_LEFT = 7,
        ED_BOTTOM_RIGHT = 8,
    };

    enum EPersonModule{
        EPF_FaceDetect = 0x1,
        EPF_PedestrianDetect = 0x2,
        EPF_FaceVerify = 0x4,
        EPF_ReID = 0x8,
        EPF_PassengerFlow = 0x10,
        EPF_HotZone = 0x20,
    };

    enum EPersonStatus{
        EPS_APPEAR = 0x01,//出现了
        EPS_PASS = 0x02,//满足了指定条件的过店（例如在沿街客流中，一个人停留了三秒以上，就算是一个过店）
        
        //如下三种状态，是互斥的
        EPS_PASS_InStore = 0x04,//有可能也只是店内的过店（可能是店内的工作人员闲逛）
        EPS_IN = 0x8,
        EPS_OUT = 0x10,
        
        //如果该位置位，表示以上三种状态是确定的
        //如果该位没有置位，则表示某人消失时的坐标位置，并不在边缘位置，导致难以判断此人是否为进店或者出店。因此EPS_PASS_InStore、EPS_IN和EPS_OUT状态并不是确定的，需要外部根据实际场景进行取舍。
        //也就是说，如果需要严格限制的进店和出店，需要EPS_SURE同时置位才行。
        EPS_SURE = 0x20,
        
        EPS_LOST = 0x80,
    };

    struct SPersonBaseInfo
    {
        unsigned int person_id;
        EDirection direction;
        unsigned int status;
        int hit_times;
        float live_seconds;
                
        SULURect face_active_region;//中心点构建的人脸活动区域
        SULURect pedestrian_active_region;//中心点构建的人形活动区域
        
        SULURect face_start;//人脸开始的位置
        SULURect ped_start;//人形开始的位置
        
        SULURect face_end;//人脸结束的位置
        SULURect ped_end;//人形结束的位置
        
        STimestamp ts_start;//此人在该段轨迹中刚出现时的时间戳
        STimestamp ts_first_start;//REID启用的情况下，此人第一次出现时的时间戳
        SPersonBaseInfo();
    };

    struct SPersonInfo : public SPersonBaseInfo
    {
        SDetectObjectInfo face_info;
        SHeadPose face_pose;
        float face_landmarks[10];//left_eyex,right_eyex, nosex, left_concerx,right_concerx   left_eyey,right_eyey,nosey,left_concery,right_concery
        float face_score;//score<0.1:no face.
        SDetectObjectInfo pedestrian_info;
        float pedestrian_score;//score<0.1:no ped.
    };

    struct SLostPersonInfo : public SPersonBaseInfo
    {
        STimestamp ts_end;//此人在该段轨迹中消失时的时间戳
        std::vector<SULUImage> crop_face_imgs;
        SFeature face_feature;
        std::vector<SULUImage> crop_pedestrian_imgs;
        SFeature pedestrian_feature;
    };

    struct SPersonStat
    {
        unsigned int in_cnt;
        unsigned int pass_cnt;
        unsigned int out_cnt;
        unsigned int reserved;
        SPersonStat();
    };

    class IULUHotZone
    {
    public:
        IULUHotZone();
        virtual ~IULUHotZone();
        
        //热力值获取，width热力图的宽，height热力图的高，hot_value热力图。返回的热力图，不需要释放。
        virtual int GetHotZone(int& width,int& height,unsigned int** hot_value,int route_id = 0) = 0;
        
        //热力图获取。
        virtual int GetHotZoneImg(SULUImage&image,int route_id = 0) = 0;
        
        //设置热区的网格单元
        //为提高热区计算速度，对热区进行网格聚类。例如640*480的输入图像，若grid_unit设置为16，则会将图像分置为40*30的网格块，对每一个网格块求其最大的热点值
        //该值默认为32，设置时最好设置为2的整数倍
        virtual void SetHotZoneGridUnit(unsigned int grid_unit) = 0;
        
        //设置热区的峰值影响范围
        //对每一个峰值，其影响的半径
        //该值默认为25，设置时该值最好小于grid_unit
        virtual void SetHotZoneAffectRadius(unsigned int radius) = 0;
        
        //设置热区的热力值范围
        //最小值，低于该值时，将被忽略掉。例如：该值默认为100，表示某个区域累计命中值低于100时，该区域将被忽略掉
        //最大值，该值设置为0，表示以实际热力值的峰值为最红色进行着色。若该值不为零，则以该值为基准峰值进行着色.
        virtual void SetHotZoneValueRange(unsigned int min_value,unsigned int max_value) = 0;
        
        //手动清除热区累计信息
        virtual void ClearHotZoneValue(int route_id = 0) = 0;
    };

    class IULUPersonConfig : public IULUHotZone
    {
    public:
        IULUPersonConfig();
        virtual ~IULUPersonConfig();
        
        //设置帧率，每秒多少帧
        virtual void SetFrameRate(float frame_per_sec) = 0;
        
        //设置最小命中次数。若小于该次数，则不会返回
        virtual void SetMinHitTimes(int hit_times) = 0;
        
        //设置最小移动距离。若小于该距离，则不会认为是一个客流
        virtual void SetMinMoveDistance(int distance) = 0;
        
        //设置最小秒数。若一个对象出现时间小于该秒，则不会返回，若大于该秒，则会认为至少是一个过店（建议此值设置为2秒或者3秒）
        virtual void SetMinLifeSeconds(float seconds) = 0;
        
        //设置进店秒数。若一个对象出现时间大于该秒，则会认为是产生了一个进店客流
        virtual void SetInLifeSeconds(float seconds) = 0;
        
        //设置最大合并秒数。若两段轨迹间隔超过该秒数，则不会合并。
        virtual void SetMaxMergeSeconds(float seconds) = 0;
        
        //设置人形合并阈值
        virtual void SetPedMaxMergeThreshould(float threshould) = 0;

        //设置检测区域，只有出现在该区域才会上报
        //设置多边形检测区域
        virtual bool SetRegion(const std::vector<SULUPoint>& pts,int idx = 0) = 0;
        
        //设置限制区域IOU，当iou值为多少时才会认为在区域内
        virtual void SetRegionIOU(float iou) = 0;
        
        //设置穿越区域，穿越过该区域时，会被认为是进店或者离店。
        //因为摄像头装在店内，因此从下往上穿越视为进店，从上往下穿越视为离店
        //穿越区域下方，从左往右，从右往左，均视为过店
        virtual bool SetAcrossRegion(const std::vector<SULUPoint>& pts) = 0;
        
        /*
        //启用REID.
        //该接口即将被弃，使用SetModule(ulu_face::EPF_ReID,true)代替使用
        virtual void EnableReID() = 0;
        
        //启用FaceVerify.
        //该接口即将被弃，使用SetModule(ulu_face::EPF_FaceVerify,true)代替使用
        virtual void EnableFaceVerify() = 0;
        
        //禁用人脸信息
        //该接口即将被弃，使用SetModule(ulu_face::EPF_FaceDetect,false)代替使用
        virtual void DisableFace() = 0;
         
        //禁用人形信息
        //该接口即将被弃，使用SetModule(ulu_face::EPF_PedestrianDetect,false)代替使用
        virtual void DisablePedestrian() = 0;
         */
        
        //设置人脸合并阈值
        virtual void SetFaceMaxMergeThreshould(float threshould) = 0;
        
        //设置瞳距范围
        virtual void SetPupilDistance(unsigned int min_d,unsigned int max_d) = 0;
        
        //设置人脸宽度范围
        virtual void SetFaceWidthRange(unsigned int min_d,unsigned int max_d) = 0;
        
        //设置启用哪些模块，一次可以设置多个模块
        virtual void SetModule(unsigned int modules,bool is_enable) = 0;
        
        //设置人脸得分范围，低于low分的，将不会计算特征
        virtual void SetFaceScoreRange(float low,float high) = 0;
        //设置人形得分范围，低于low分的，将不会计算特征
        virtual void SetPedScoreRange(float low,float high) = 0;
        
        //设置输出图像数量，最多为五张
        virtual void SetOutputImageCount(int img_cnt) = 0;
        
        //是否需要整张图片的输出
        //默认情况下，当一个人的轨迹彻底结束后，SDK内给出的是裁剪文件。当need为true时，SDK内给出整张图片
        virtual void NeedOutputFullImage(bool need) = 0;
        
        //是否每帧的检测出像都传出
        //默认情况下，只有有效的客流才会输出。当need为true时，则每帧中检测到的对象都会传出，但Personid可能为-1
        virtual void NeedOutputDetectImage(bool need) = 0;
    };

    class IULUPerson : public IULUPersonConfig
    {
    public:
        IULUPerson();
        virtual ~IULUPerson();
        
        virtual EReturn_Code Init(EModel_Type mt,const char * model_path)=0;
        virtual EReturn_Code Init(EModel_Type mt,const unsigned char * buffer,size_t buffer_len)=0;

        //设置场景
        virtual EReturn_Code SetEScene(EScene scene,bool load_default_param=false) = 0;
        
        //设置人形检测算法
        virtual EReturn_Code SetPersonDetectAlgorithm(EPersonDetectAlgor_Type algo) = 0;
        
        //这个函数,传入一张图片，返回当前图片中的人
        virtual int Update(const SULUImage& image,std::vector<SPersonInfo>& out_info) = 0;
        
        //这个函数相对于Update而言,多了一个参数orig_image，该参数为image的原始高清图片。SDK内部将缓存高清的图片。高清图片对于人脸识别、人形识别来说，能提高一定的准确率。
        virtual int UpdateEx(const SULUImage& image,std::vector<SPersonInfo>& out_info,const SULUImage& orig_image) = 0;
        
        //这个函数，参数中多了一个检测框集合。
        //这个函数内将不会再执行检测，而是使用传入的检测框，进行客流计算
        virtual int UpdateEx(const SULUImage& image,const std::vector<SBBox>& in_boxes,std::vector<SPersonInfo>& out_info,const SULUImage& orig_image) = 0;
        
        //这个函数由外部定期调用（1-5秒内调用一次，返回自上次调用后消失的人，返回累计进店数，返回累计过店数）
        //重要，这个函数一定要定时调用,调用间隔时间根据系统资源情况来决定，但不要小于1秒，不要大于5秒
        virtual int GetStat(std::vector<SLostPersonInfo>& out_lost_info,SPersonStat& stat,const STimestamp& tv,int route_id = 0) = 0;
        
        //这个函数用来清除当前的计数，从头开始
        virtual int ResetRoute(std::vector<SLostPersonInfo>& out_lost_info,int route_id = 0) = 0;
        
        virtual int AlignFace(const SULUImage& image,const SBBox& box,const float face_landmarks[],SULUImage& out_image) = 0;
        /*
        //这些接口仅仅为了兼容以前的内容
        virtual int DetectFaceWithPedestrian(SULUImage& image,SFaceAndPedestrianInfo& out_info) = 0;
        virtual int GetFaceInfo(SULUImage& image,std::vector<SFaceInfo>& out_faceinfo) = 0;
        virtual int CalcImageQuality(SULUImage& image,std::vector<SDetectObjectInfo>& out_det_obj_info) = 0;
         */
        virtual int AlignFace(const SULUImage& image,const SFaceInfo& in_faceinfo,SULUImage& out_image) = 0;
        virtual int AlignFaces(const SULUImage& image,const std::vector<SFaceInfo>& in_faceinfos,std::vector<SULUImage>& out_images) = 0;
                
        virtual int FaceAttr(const SULUImage& image,SFaceAttr& out_attr) = 0;
        virtual int FaceAttrs(const std::vector<SULUImage>& images,std::vector<SFaceAttr>& out_attrs) = 0;
        const int gender_list_[2] = {0,1};
        const std::string gender_str_list_[2] = {"female","male"};
        
        virtual int PedestrianAttr(const SULUImage& image,SPedestrianAttr& out_attr) = 0;
        virtual int PedestrianAttrs(const std::vector<SULUImage>& images,std::vector<SPedestrianAttr>& out_attrs) = 0;

        virtual int CalcFaceFeature(const SULUImage& align_image,SFaceFeature& out_feature) = 0;
        virtual int CalcFaceFeatures(const std::vector<SULUImage>& align_images,std::vector<SFaceFeature>& out_features) = 0;
        virtual float GetFaceReferenceThreshold() = 0;
        virtual void SetFaceThreshold(float threshold) = 0;
        virtual float CalcFaceSimilarity(const SFeature& feature1,const SFeature& feature2) = 0;
        
        virtual int CalcPedFeature(const SULUImage& align_image,SFeature& out_feature) = 0;
        virtual int CalcPedFeatures(const std::vector<SULUImage>& align_images,std::vector<SFeature>& out_features) = 0;
        virtual float GetPedReferenceThreshold() = 0;
        virtual void SetPedThreshold(float threshold) = 0;
        virtual float CalcPedSimilarity(const SFeature& feature1,const SFeature& feature2) = 0;
        
        virtual int MergeFeatures(const std::vector<SFeature>& in_features,SFeature& out_feature) = 0;
    };

    extern "C" EReturn_Code CreatePerson(IULUPerson ** pPerson);
    extern "C" EReturn_Code DestroyPerson(IULUPerson ** pPerson );

    extern "C" float CalcFacePose(const SHeadPose& face_pose);
    extern "C" float CalcPupilDistance(float face_landmarks1[10]);
};

#endif /* IULUPerson_h */
