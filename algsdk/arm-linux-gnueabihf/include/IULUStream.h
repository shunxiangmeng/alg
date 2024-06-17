//
//  IULUStream.h
//  FaceLib
//
//  Created by harris on 2024/4/22.
//  Copyright © 2023年 harris. All rights reserved.
//

#ifndef IULUStream_h
#define IULUStream_h

#include "IULUBaseType.h"

namespace ulu_face{
    class IULUStream
    {
    public:
        IULUStream();
        virtual ~IULUStream();
        
        virtual int Open(const char * addr) = 0;
        virtual bool IsValid()const = 0;
        virtual void Stop() = 0;
        virtual bool SetVideoCoderName(const char * coder_name) = 0;
        virtual bool SetAudioCoderName(const char * coder_name) = 0;
    };

    class IULUStreamClient : public IULUStream
    {
    public:
        IULUStreamClient();
        virtual ~IULUStreamClient();
        
        virtual int GetNextFrame(SULUImage& image) = 0;
    };

    class IULUStreamPublisher : public IULUStream
    {
    public:
        IULUStreamPublisher();
        virtual ~IULUStreamPublisher();

        virtual int PutNextFrame(const SULUImage& image) = 0;
        virtual void SetFPS(float fps) = 0;
    };

    extern "C" EReturn_Code CreateStreamClient(IULUStreamClient ** pClient);
    extern "C" EReturn_Code DestroyStreamClient(IULUStreamClient ** pClient );

    extern "C" EReturn_Code CreateStreamPublisher(IULUStreamPublisher ** pPublisher);
    extern "C" EReturn_Code DestroyStreamPublisher(IULUStreamPublisher ** pPublisher);
};

#endif /* IULUStream_h */
