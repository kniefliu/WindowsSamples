# cmake file for duilib
#Author Qi Gao(monkgau@gmail.com)
#Created: 2012/09/16

# 创建文件过滤器
file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE SRC_Control "${CMAKE_CURRENT_SOURCE_DIR}/Control/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Control/*.h")
remove(SRC_Control "${CMAKE_CURRENT_SOURCE_DIR}/Control/UIFlash.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Control/UIFlash.h")
source_group("Control"   FILES ${SRC_Control})
file(GLOB_RECURSE SRC_Core "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.h")
source_group("Core"   FILES ${SRC_Core})
file(GLOB_RECURSE SRC_Layout "${CMAKE_CURRENT_SOURCE_DIR}/Layout/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Layout/*.h")
source_group("Layout"   FILES ${SRC_Layout})
file(GLOB_RECURSE SRC_Utils "${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.c" "${CMAKE_CURRENT_SOURCE_DIR}/Utils/*.h")
source_group("Utils"   FILES ${SRC_Utils})

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_library(duilib SHARED ${SRC_Root} ${SRC_Control} ${SRC_Core} ${SRC_Layout} ${SRC_Utils})

if (MSVC)
   set_target_properties(duilib PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
   set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
   set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Utils/XUnzip.cpp PROPERTIES COMPILE_FLAGS "/Y-")
   set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/Utils/stb_image.c PROPERTIES COMPILE_FLAGS "/Y-")
endif(MSVC)

add_definitions(-DUILIB_EXPORTS)
target_link_libraries(duilib comctl32)
set_target_properties(duilib PROPERTIES OUTPUT_NAME "duilib")
