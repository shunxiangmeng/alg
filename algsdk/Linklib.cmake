#添加公共依赖的库

#only for test
link_directories(${PROJECT_SOURCE_DIR}/algsdk/${ToolPlatform}/lib)
set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} ulu_crypt uluai_core uluai_imgio uluai_ppose uluai_carplate uluai_dump_desc uluai_fea_ext uluai_objdet uluai_passflow uluai_speech_rec rknn_api)
#set(APP_DEPEND_LIBS ${APP_DEPEND_LIBS} rknn_api)
