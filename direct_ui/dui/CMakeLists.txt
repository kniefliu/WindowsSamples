# 创建文件过滤器

file(GLOB Include_Root "${CMAKE_CURRENT_SOURCE_DIR}/include/dui/*.h")
source_group("include"   FILES ${Include_Root})
file(GLOB Src_Root "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")
source_group("src"   FILES ${Src_Root})

file(GLOB Include_Core "${CMAKE_CURRENT_SOURCE_DIR}/include/dui/core/*.h")
source_group("include\\core"   FILES ${Include_Core})
file(GLOB Src_Core "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h")
source_group("src\\core"   FILES ${Src_Core})

file(GLOB Src_Core_Win "${CMAKE_CURRENT_SOURCE_DIR}/src/core/win/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/core/win/*.h")
source_group("src\\core\\win"   FILES ${Src_Core_Win})

file(GLOB Include_Window "${CMAKE_CURRENT_SOURCE_DIR}/include/dui/window/*.h")
source_group("include\\window"   FILES ${Include_Window})
file(GLOB Src_Window "${CMAKE_CURRENT_SOURCE_DIR}/src/window/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/window/*.h")
source_group("src\\window"   FILES ${Src_Window})

file(GLOB Src_Window_Win "${CMAKE_CURRENT_SOURCE_DIR}/src/window/win/*.cc" "${CMAKE_CURRENT_SOURCE_DIR}/src/window/win/*.h")
source_group("src\\window\\win"   FILES ${Src_Window_Win})

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_library(dui SHARED ${Src_Core} ${Include_Core} ${Include_Window} ${Src_Root} ${Include_Root} ${Src_Core_Win} ${Src_Window} ${Src_Window_Win})

if (MSVC)
    set_target_properties(dui PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    set_target_properties(dui PROPERTIES COMPILE_FLAGS "/Yudui_precompile.h /FIdui_precompile.h")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/dui_precompile.cc" PROPERTIES COMPILE_FLAGS "/Ycdui_precompile.h")
    #add_definitions(/FI"dui_precompile.h")
    # GCC or Clang
    #add_definitions(-include dui_precompile.h)
endif(MSVC)

add_definitions(-DDUI_IMPLEMENTATION)
#target_link_libraries(dui comctl32)
set_target_properties(dui PROPERTIES OUTPUT_NAME "dui")
