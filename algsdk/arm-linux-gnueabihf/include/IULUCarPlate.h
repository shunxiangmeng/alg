//
//  IULUCarPlate.h
//  FaceLib
//
//  Created by harris on 2022/7/26.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUCarPlate_h
#define IULUCarPlate_h

#include "IULUBaseType.h"
#include "IULUObjDetect.h"

namespace ulu_face{
    const int max_plate_len = 8;
    enum EULU_CarPlateColor { ECPC_BLUE = 1, ECPC_YELLOW, ECPC_GREEN, ECPC_WHITE, ECPC_BLACK, ECPC_UNKNOWN = 101, };
    enum EULU_CarPlateType { ECPT_NORMAL =1, ECPT_UNKNOWN = 101, };

    struct SCarPlateRegInfo
    {
        int plate_len = 7;
        EULU_CarPlateColor cp_color;
        EULU_CarPlateType cp_type;
        
        float confidences[max_plate_len];
        char reg_result[max_plate_len][5];
        
        SCarPlateRegInfo();
    };

    class IULUCarPlate : public IObjDetect
    {
    public:
        IULUCarPlate();
        virtual ~IULUCarPlate();
        
        virtual int RegCarPlate(const SULUImage& image,std::vector<SCommonObjectInfo>& in_out_cps_det_info,std::vector<SCarPlateRegInfo>& out_cps_reg_info ) = 0;
    };

    extern "C" EReturn_Code CreateCarPlate(IULUCarPlate ** pCarPlate);
    extern "C" EReturn_Code DestroyCarPlate(IULUCarPlate ** pCarPlate );

    extern "C" EReturn_Code AlignCarPlate(const SULUImage& image,const SCommonObjectInfo& in_obj_info,SULUImage& out_image);
    extern "C" EReturn_Code AlignCarPlates(const SULUImage& image,const std::vector<SCommonObjectInfo>& in_obj_infos,std::vector<SULUImage>& out_images);
};

#endif /* IULUCarPlate_h */
