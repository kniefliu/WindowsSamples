#Cmake file for MenuDemo
#Author: Qi Gao(monkgau@gmail.com)
#Created: 2012/9/17

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp")
remove(SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/MenuNotify.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/MenuNotify.h")

add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/../bin/")                     

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(MenuDemo ${SRC_Root} MenuDemo.rc)
set_target_properties(MenuDemo PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
target_link_libraries(MenuDemo duilib)
set_target_properties(MenuDemo PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")