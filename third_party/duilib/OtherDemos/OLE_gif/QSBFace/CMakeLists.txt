#Cmake file for QSBFace
#Author: Knief Liu(kniefliu@163.com)
#Date: 2017/1/7

file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.rc")
file(GLOB SRC_cxImage "${CMAKE_CURRENT_SOURCE_DIR}/cxImage/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/cxImage/*.h")
source_group("cxImage"   FILES ${SRC_cxImage})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_CRT_NON_CONFORMING_SWPRINTFS)
add_definitions(-DDUI_RES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(QSBFace ${SRC_Root} ${SRC_cxImage})
set_target_properties(QSBFace PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
add_dependencies(QSBFace duilib)
target_link_libraries(QSBFace duilib)
