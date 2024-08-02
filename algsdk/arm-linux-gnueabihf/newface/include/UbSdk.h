#ifndef UBSDK_h
#define UBSDK_h

#include "IUbObjDetect.h"
#include "IUbLandmarkDetect.h"
#include "IUbObjTrack.h"
#include "IUbMixDetect.h"
#include "IUbAttrExtract.h"
#include "UbImageQuality.h"

namespace ulu_best{

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code SdkInit();
    EReturn_Code SdkUninit();
    EReturn_Code GetVersion(char szVersion[16]);
    EReturn_Code EnableDebug(int level); // 1- error  2- warning  3- info 4- debug

#ifdef __cplusplus
}
#endif /* __cplusplus */

};

#endif /* UBSDK_h */