//
//  IULUFeatureExtract.h
//  FaceLib
//
//  Created by harris on 2022/7/15.
//  Copyright © 2022年 harris. All rights reserved.
//

#ifndef IULUFeatureExtract_h
#define IULUFeatureExtract_h

#include "IULUBaseType.h"

namespace ulu_face{
    class IFeatureExtract
    {
    public:
        IFeatureExtract();
        virtual ~IFeatureExtract();
        virtual EReturn_Code Init(EModel_Type mt,const char * model_path)=0;
        virtual EReturn_Code Init(EModel_Type mt,const unsigned char * buffer,size_t buffer_len)=0;
        
        virtual int CalcFeature(const SULUImage& align_image,SFeature& out_feature) = 0;
        virtual int CalcFeatures(const std::vector<SULUImage>& align_images,std::vector<SFeature>& out_features) = 0;
        virtual int MergeFeatures(const std::vector<SFeature>& in_features,SFeature& out_feature) = 0;
        
        //virtual float CalcDiff(SFeature& feature1,SFeature& feature2) = 0;
        virtual float GetReferenceThreshold() = 0;
        virtual void SetThreshold(float threshold) = 0;
        //virtual float ConvertDiff2Similarity(float diff) = 0;
        
        virtual float CalcSimilarity(const SFeature& feature1,const SFeature& feature2) = 0;
    };

    extern "C" EReturn_Code CreateFeatureExtract(IFeatureExtract ** pFE);
    extern "C" EReturn_Code DestroyFeatureExtract(IFeatureExtract ** pFE );
};

#endif /* IULUFeatureExtract_h */
