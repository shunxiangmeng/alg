//
//  IULUImageIO.h
//  FaceLib
//
//  Created by harris on 2023/9/18.
//  Copyright © 2023年 harris. All rights reserved.
//

#ifndef IULUImageIO_h
#define IULUImageIO_h

#include "IULUBaseType.h"

namespace ulu_face{
    /*
     如果图像是灰度图像，则load出来是gray格式
     如果图像是jpg真彩图像，则load出来是rgb格式
     如果图像是png图像，则load出来是rgba格式
     通过image.pixel_format可以获得load图像的格式
     */
    extern "C" EReturn_Code LoadImageFromFile(const char *file_name,SULUImage& image);
    extern "C" EReturn_Code LoadImageFromMemory(const unsigned char *buffer,int buffer_len,SULUImage& image);

    /*
     因为大多数神经网络模型均需要输入为rgb格式,因此提供了这两个函数
     */
    extern "C" EReturn_Code ConvertRGBA2RGB(const SULUImage& in_image,SULUImage& out_image);
    extern "C" EReturn_Code ConvertGray2RGB(const SULUImage& in_image,SULUImage& out_image);

    /*
     将图像编码后写进jpg或者png文件
     目前支持两种扩展名：.jpg或者.png
     */
    extern "C" EReturn_Code WriteImageToFile(const char *file_name,const SULUImage& image);
};

#endif /* IULUImageIO_h */
