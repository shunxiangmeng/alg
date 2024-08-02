#ifndef UBImageQuality_h
#define UBImageQuality_h

#include "UbCommon.h" 

namespace ulu_best{
    // 0 - 1 越高越好   
    float UbEvalImageQuality(SUbImg &img);

    float UbEvalRectImageQuality(SUbImg &img, SUbRect &box);
};

#endif /* UBImageQuality_h */