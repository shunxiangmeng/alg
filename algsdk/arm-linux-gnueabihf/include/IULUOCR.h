//
//  IULUOCR.h
//  FaceLib
//
//  Created by harris on 2024/4/12.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef IULUOCR_h
#define IULUOCR_h

#include "IULUBaseType.h"
#include "IULUObjDetect.h"

namespace ulu_face{
    const int max_ocr_len = 120;
    struct SOCRRegInfo
    {
        int ocr_len = 0;
        int type = 0;
        float confidences[max_ocr_len];
        char reg_result[max_ocr_len][5];
        
        SOCRRegInfo();
    };

    class IULUOCR : public IObjDetect
    {
    public:
        IULUOCR();
        virtual ~IULUOCR();
        
        virtual int Recognition(const SULUImage& image,std::vector<SCommonObjectInfo>& in_out_det_info,std::vector<SOCRRegInfo>& out_reg_info ) = 0;
        virtual int AlignOCR(const SULUImage& image,const SCommonObjectInfo& in_obj_info,SULUImage& out_image) = 0;
        virtual int AlignOCRs(const SULUImage& image,const std::vector<SCommonObjectInfo>& in_obj_infos,std::vector<SULUImage>& out_images) = 0;
    };

    extern "C" EReturn_Code CreateOCR(IULUOCR ** pOCR);
    extern "C" EReturn_Code DestroyOCR(IULUOCR ** pOCR );

    extern "C" EReturn_Code AlignOCR(const SULUImage& image,const SCommonObjectInfo& in_obj_info,SULUImage& out_image);
    extern "C" EReturn_Code AlignOCRs(const SULUImage& image,const std::vector<SCommonObjectInfo>& in_obj_infos,std::vector<SULUImage>& out_images);
};

#endif /* IULUOCR_h */
