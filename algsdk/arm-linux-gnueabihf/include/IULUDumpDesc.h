//
//  IULUDumpDesc.h
//  FaceLib
//
//  Created by harris on 2018/7/29.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUDumpDesc_h
#define IULUDumpDesc_h

#include "IULUBaseType.h"
#include "IULUPerson.h"
#include "IULUPose.h"
#include "IULUObjDetect.h"
#include "IULUCarPlate.h"
#include "IULUOCR.h"

namespace ulu_face{
    extern "C" EReturn_Code StartDumpServ(const char * addr);
    extern "C" EReturn_Code DumpDesc(const char * desc,const std::vector<SULUImage>& images);
    //以下函数用来获得运算结果的字符串描述信息
    extern "C"  int PersonUpdateExDesc(const SULUImage& image,bool followed_by_imgs,const std::vector<SPersonInfo>& in_info,std::string& out_str);
    extern "C"  int PersonGetStatDesc(bool followed_by_imgs,const std::vector<SLostPersonInfo>& in_lost_info,const SPersonStat& in_stat,const STimestamp& tv,int route_id ,std::string& out_str);
    extern "C"  int DetectObjectDesc(const SULUImage& image,bool followed_by_imgs,const std::vector<SCommonObjectInfo>& in_out_cps_det_info,std::string& out_str);
    extern "C"  int RegCarPlateDesc(const SULUImage& image,bool followed_by_imgs,const std::vector<SCommonObjectInfo>& in_out_cps_det_info,const std::vector<SCarPlateRegInfo>& out_cps_reg_info,std::string& out_str);
    extern "C"  int RegOCRDesc(const SULUImage& image,bool followed_by_imgs,const std::vector<SCommonObjectInfo>& in_out_det_info,const std::vector<SOCRRegInfo>& out_reg_info,std::string& out_str);
    extern "C"  int GetPoseDesc(const SULUImage& image,bool followed_by_imgs,const std::vector<SPoseEstimation>& poses,std::string& out_str);
    extern "C"  int GetHotZoneImgDesc(const SULUImage& image,std::string& out_str);
};

#endif /* IULUDumpDesc_h */
