cmake_minimum_required(VERSION 3.10.0)

project(smart)

set(Tartget smart)

message(STATUS "******************************************************************")
message(STATUS "CMAKE_SOURCE_DIR->" ${CMAKE_SOURCE_DIR})
message(STATUS "PROJECT_SOURCE_DIR->" ${PROJECT_SOURCE_DIR})
message(STATUS "******************************************************************")

set(CMAKE_CXX_STANDARD 14)

#编译详情
#set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_BUILD_TYPE Debug)

#交叉编译工具链信息
include(${PROJECT_SOURCE_DIR}/Toolchains.cmake)

if(WIN32)
    if(MSVC)
        #禁止编译警告：warning C4819：该文件包含不能在当前代码页(936)中表示的字符。请将该文件保存为 Unicode 格式以防止数据丢失
        add_compile_options(/wd4819)
        add_compile_options(/wd4200)  #禁止C4200警告
        add_compile_definitions(_WINSOCK_DEPRECATED_NO_WARNINGS)
        add_compile_definitions(_CRT_SECURE_NO_WARNINGS)  # 禁止C4996警告
    endif()
else()
    # 消除GCC 7.x 的一个 abi 编译 note
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi")
endif()

#依赖库信息
set(APP_DEPEND_LIBS)
include(${PROJECT_SOURCE_DIR}/Linklib.cmake)

#头文件路径
set(APP_INCLUDE_DIRS)
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/common)
include_directories(${PROJECT_SOURCE_DIR}/common/infra)
include_directories(${PROJECT_SOURCE_DIR}/common/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/common/thirdparty/msgpack)
include_directories(${PROJECT_SOURCE_DIR}/common/common/mediafiles)
include_directories(${PROJECT_SOURCE_DIR}/common/common/mediafiles/common/libmov/include)
include_directories(${PROJECT_SOURCE_DIR}/common/middleware)
include_directories(${PROJECT_SOURCE_DIR}/applicaton)
include_directories(${PROJECT_SOURCE_DIR}/hardware)
include_directories(${PROJECT_SOURCE_DIR}/algsdk/${PlatformToolchain}/include)

#源文件
set(APP_SOURCE_FILES)
set(APP_SOURCE_HEADER_FILES)

add_subdirectory(common/infra)
add_subdirectory(common/common)
add_subdirectory(common/middleware)
add_subdirectory(common/thirdparty)

#file(GLOB_RECURSE OAC_SOURCE_FILES common/middleware/oac/src/common/*.cpp common/middleware/oac/src/client/*.cpp common/middleware/private/*.cpp)
#file(GLOB_RECURSE OAC_SOURCE_HEADER_FILES common/middleware/oac/src/common/*.h common/middleware/oac/src/client/*.h common/middleware/private/*.h)

file(GLOB_RECURSE APPLICATION_SOURCE_FILES application/*.cpp)
file(GLOB_RECURSE APPLICATION_SOURCE_HEADER_FILES application/*.h)

set(APP_SOURCE_FILES ${APP_SOURCE_FILES} ${APPLICATION_SOURCE_FILES} ${OAC_SOURCE_FILES})
set(APP_SOURCE_HEADER_FILES ${APP_SOURCE_HEADER_FILES} ${APPLICATION_SOURCE_HEADER_FILES} ${OAC_SOURCE_HEADER_FILES})

# for freetype
add_compile_definitions(DARWIN_NO_CARBON)
add_compile_definitions(FT2_BUILD_LIBRARY)

function(set_file_group prefix)
    message(STATUS "set_file_group " ${prefix} " " ${ARGC})
    foreach(FILE IN LISTS ARGN 1)
        # Get the directory of the source file
        get_filename_component(PARENT_DIR "${FILE}" DIRECTORY)
        # Remove common directory prefix to make the group
        string(REPLACE "${prefix}" "" GROUP "${PARENT_DIR}")
        # Make sure we are using windows slashes
        string(REPLACE "/" "\\" GROUP "${GROUP}")
        source_group("${GROUP}" FILES "${FILE}")
    endforeach()
endfunction()

if(WIN32)
    set_file_group(${CMAKE_SOURCE_DIR} ${APP_SOURCE_FILES} ${APP_SOURCE_HEADER_FILES})
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

#指定头文件路径
include_directories(${APP_INCLUDE_DIRS})
add_executable(${Tartget} ${APP_SOURCE_FILES} ${APP_SOURCE_HEADER_FILES})
#添加依赖库
target_link_libraries(${Tartget} ${APP_DEPEND_LIBS})

#输出目录
#SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/Bin/${PLATFORM}/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../Build/${Platform})
