#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread rt m)
elseif(WIN32)
endif()


link_directories(${PROJECT_SOURCE_DIR}/hardware/${PlatformToolchain})
include(${PROJECT_SOURCE_DIR}/hardware/${PlatformToolchain}/Linklib.cmake)

include(${PROJECT_SOURCE_DIR}/algsdk/Linklib.cmake)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS})
