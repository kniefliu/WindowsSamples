# cmake file for UIlib
# Date		:  2017-12-07
# Author  	:  kniefliu (kniefliu@163.com)

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_library(UIlib SHARED ${SRC_Root})

if (MSVC)
	set_target_properties(UIlib PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
	set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
endif(MSVC)

add_definitions(-DUILIB_EXPORTS)
