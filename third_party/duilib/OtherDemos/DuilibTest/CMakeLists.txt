#Cmake file for DuilibTest
#Author: Knief Liu(kniefliu@163.com)
#Date: 2017/1/7

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.rc")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(DuilibTest ${SRC_Root})
set_target_properties(DuilibTest PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(DuilibTest PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
add_dependencies(DuilibTest duilib)
target_link_libraries(DuilibTest duilib)
