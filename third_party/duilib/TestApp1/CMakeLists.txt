#Cmake file for TestApp1
#Author: Knief Liu(kniefliu@163.com)
#Date: 2018/1/7

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../bin/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(TestApp1 ${SRC_Root})
set_target_properties(TestApp1 PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(TestApp1 PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
add_dependencies(TestApp1 duilib)
target_link_libraries(TestApp1 duilib)
