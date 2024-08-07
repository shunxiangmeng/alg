#交叉编译工具链信息
#工具链全路径，防止重名冲突

if(PLATFORM)
    SET(Platform ${PLATFORM})
else()
    SET(Platform "x86")
endif()

SET(ToolPlatform)

if(${Platform} STREQUAL "FH885XV200")
    add_definitions(-DFH885XV200)
    add_definitions(-DPLATFORM="FH885XV200")
    SET(CMAKE_C_COMPILER "/opt/arm-fullhanv3-linux-uclibcgnueabi-b6/bin/arm-fullhanv3-linux-uclibcgnueabi-gcc")
    SET(CMAKE_CXX_COMPILER "/opt/arm-fullhanv3-linux-uclibcgnueabi-b6/bin/arm-fullhanv3-linux-uclibcgnueabi-g++")
    SET(CMAKE_C_COMPILER_VERSION "6.5.0")
elseif(${Platform} STREQUAL "RK")
    add_definitions(-DRK)
    SET(CMAKE_C_COMPILER "/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-gcc")
    SET(CMAKE_CXX_COMPILER "/opt/gcc-arm-8.3-2019.03-x86_64-arm-linux-gnueabihf/bin/arm-linux-gnueabihf-g++")
    SET(CMAKE_C_COMPILER_VERSION "8.3.0")
    SET(ToolPlatform "arm-linux-gnueabihf")
elseif(${Platform} STREQUAL "W2")
    SET(CMAKE_C_COMPILER "/opt/arm-buildroot-linux-uclibcgnueabihf-4.9.4-uclibc-1.0.31/bin/arm-buildroot-linux-uclibcgnueabihf-gcc")
    SET(CMAKE_CXX_COMPILER "/opt/arm-buildroot-linux-uclibcgnueabihf-4.9.4-uclibc-1.0.31/bin/arm-buildroot-linux-uclibcgnueabihf-g++")
    #版本真老
    SET(CMAKE_C_COMPILER_VERSION "4.9.4")
elseif(${Platform} STREQUAL "win32")
    add_definitions(-DWIN32)
elseif(${Platform} STREQUAL "x86")
    add_definitions(-DX86)
else()
    #直接报错
    MESSAGE(FATAL_ERROR "unsupport compile platform: " ${Platform})
endif()

if (CMAKE_CL_64)
    set(ADDRESS_MODEL 64)
    set(NODE_TARGET x64)
else()
    MESSAGE(STATUS "32 bit compile")
endif()

message(STATUS "compile platform -> " ${Platform})
message(STATUS "complier -> " ${CMAKE_C_COMPILER})
message(STATUS "complier version -> " ${CMAKE_C_COMPILER_VERSION})
