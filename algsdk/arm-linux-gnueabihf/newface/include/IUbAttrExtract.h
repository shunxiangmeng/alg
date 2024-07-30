#ifndef IUBAttrExtract_h
#define IUBAttrExtract_h

#include "UbCommon.h"
#include <vector>

namespace ulu_best{

    enum EAttr_Type{
        E_Attr_Person = 1,                 // 人形属性
    };

    #define UB_MAX_ATTR_LEN  (16)           // 最大长度   
    struct SUbAttrInfo
    {
        unsigned int attr_cnt;               // 属性长度
        float attrs[UB_MAX_ATTR_LEN];        // 属性值 
    };

#ifdef USE_ATTR_EXTRACT

    class IAttrExtract
    {
    public:
        IAttrExtract() {}
        virtual ~IAttrExtract() {}

        // 模型初始化
        virtual EReturn_Code Init(const char * model_path, int gpu_id=0) = 0;
        virtual EReturn_Code Init(const unsigned char * buffer, int buffer_len, int gpu_id=0) = 0;

        // 获取属性
        virtual EReturn_Code GetAttr(SUbImg &img, SUbObjInfo &obj, SUbAttrInfo& out_attr) = 0;
        virtual EReturn_Code GetAttrs(SUbImg &img, std::vector<SUbObjInfo> &objs, std::vector<SUbAttrInfo>& out_attrs) = 0;
    };

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    EReturn_Code CreateAttrExtract(EAttr_Type eType, IAttrExtract ** ppAttr);
    EReturn_Code DestroyAttrExtract(IAttrExtract ** ppAttr);

#ifdef __cplusplus
}
#endif /* __cplusplus */    

#endif

};

#endif /* IUBAttrExtract_h */