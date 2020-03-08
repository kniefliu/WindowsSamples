# cmake file for App
# Date		:  2017-12-07
# Author  	:  kniefliu (kniefliu@163.com)

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB SRC_Resource "${CMAKE_CURRENT_SOURCE_DIR}/*.rc")
file(GLOB SRC_Resource_res "${CMAKE_CURRENT_SOURCE_DIR}/res/*.ico" "${CMAKE_CURRENT_SOURCE_DIR}/res/*.bmp")
source_group("res" FILES ${SRC_Resource_res})

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(App WIN32 ${SRC_Root} ${SRC_Resource} ${SRC_Resource_res})
target_link_libraries(App UIlib)

if (MSVC)
	set_target_properties(App PROPERTIES COMPILE_FLAGS "/YuStdAfx.h")
	set_source_files_properties(StdAfx.cpp PROPERTIES COMPILE_FLAGS "/YcStdAfx.h")
endif(MSVC)

add_dependencies(App UIlib)
