#添加公共依赖的库
set(PLATFORM_DEPEND_LIBS)
set(APP_DEPEND_LIBS)

if (UNIX)
    set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} pthread rt m curl)
elseif(WIN32)
endif()


link_directories(${PROJECT_SOURCE_DIR}/hardware/${ToolPlatform})
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/common/thirdparty/prebuilts/lib/${ToolPlatform})
include(${PROJECT_SOURCE_DIR}/hardware/${ToolPlatform}/Linklib.cmake)

include(${PROJECT_SOURCE_DIR}/algsdk/Linklib.cmake)

set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ${PLATFORM_DEPEND_LIBS})
