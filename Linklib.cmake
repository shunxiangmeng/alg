#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)
set(ALG_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread rt m curl)
elseif(WIN32)
endif()


set(ALG_DEPEND_LIBS ${ALG_DEPEND_LIBS} ulu_rknn_api)

link_directories(${PROJECT_SOURCE_DIR}/hardware/${ToolPlatform})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/thirdparty/prebuilts/lib/${ToolPlatform})

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/algsdk/${ToolPlatform}/newface/lib)

include(${PROJECT_SOURCE_DIR}/hardware/${ToolPlatform}/Linklib.cmake)

include(${PROJECT_SOURCE_DIR}/algsdk/Linklib.cmake)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS} ${ALG_DEPEND_LIBS})
