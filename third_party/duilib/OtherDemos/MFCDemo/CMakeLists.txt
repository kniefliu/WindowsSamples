#Cmake file for MFCDemo
#Author: Knief Liu(kniefliu@163.com)
#Date: 2017/1/7

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.rc")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(MFCDemo ${SRC_Root})
add_definitions(-D_AFCDLL)
set(CMAKE_MFC_FLAG 2)
set_target_properties(MFCDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(MFCDemo PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
add_dependencies(MFCDemo duilib)
target_link_libraries(MFCDemo duilib)

