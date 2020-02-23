#Cmake file for FlashDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../bin/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(FlashDemo ${SRC_Root})
add_dependencies(FlashDemo duilib)
target_link_libraries(FlashDemo duilib)
set_target_properties(FlashDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
set_target_properties(FlashDemo PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
