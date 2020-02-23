#Cmake file for 360SafeDemo
#Author: Qi Gao(monkgau@gmail.com)
#Date: 2012/9/17

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../bin/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(360safedemo ${SRC_Root})
set_target_properties(360safedemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(360safedemo PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
add_dependencies(360safedemo duilib)
target_link_libraries(360safedemo duilib)
