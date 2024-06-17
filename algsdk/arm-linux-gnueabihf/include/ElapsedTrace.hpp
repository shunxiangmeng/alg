//
//  ElapsedTrace.hpp
//  FaceLib
//
//  Created by harris on 2018/8/1.
//  Copyright © 2018年 harris. All rights reserved.
//

#ifndef ElapsedTrace_hpp
#define ElapsedTrace_hpp

#include <stdio.h>
#include <string>
#include <chrono>

namespace ulu_face
{
    class CElapsedTrace
    {
    public:
        CElapsedTrace(const std::string& desc_string,FILE * fp = stdout);
        ~CElapsedTrace();
        void Start();
        void Trace(const std::string& desc_string);

    private:
        std::string desc_string_;
        FILE * fp_;
        std::chrono::steady_clock::time_point start_;
        std::chrono::steady_clock::time_point last_tick_;
    };
};

#endif /* ElapsedTrace_hpp */
