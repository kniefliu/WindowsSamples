file(GLOB SRC_Root "${CMAKE_CURRENT_SOURCE_DIR}/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../../dui/include")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_executable(test_app ${SRC_Root})
if (MSVC)
    set_target_properties(test_app PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    set_target_properties(test_app PROPERTIES COMPILE_FLAGS "/Yuprecompile.h /FIprecompile.h")
    set_source_files_properties(precompile.cc PROPERTIES COMPILE_FLAGS "/Ycprecompile.h")
endif(MSVC)
add_dependencies(test_app dui)
target_link_libraries(test_app dui)
