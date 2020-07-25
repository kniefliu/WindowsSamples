# Append platform specific sources to a list of sources.
macro(LIBQUI_APPEND_PLATFORM_SOURCES name_of_list)
  if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows" AND ${name_of_list}_WINDOWS)
    list(APPEND ${name_of_list} ${${name_of_list}_WINDOWS})
  endif()
endmacro()

set(QUI_TARGET ${TEMPLATE_NAME}_core)

file(GLOB LIBQUI_INCLUDE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/include/qui/*.h")
source_group(include\\\\qui FILES ${LIBQUI_INCLUDE_SRCS})

file(GLOB LIBQUI_INCLUDE_CORE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/include/qui/core/*.h")
source_group(include\\\\qui\\\\core FILES ${LIBQUI_INCLUDE_CORE_SRCS})

file(GLOB LIBQUI_INCLUDE_TEMPLATE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/include/qui/${TEMPLATE_NAME}/*.h")
source_group("include\\\\qui\\\\${TEMPLATE_NAME}" FILES ${LIBQUI_INCLUDE_TEMPLATE_SRCS})


file(GLOB LIBQUI_SRC_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")
source_group(src FILES ${LIBQUI_SRC_SRCS})

file(GLOB LIBQUI_SRC_CORE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cc")
source_group(src\\\\core FILES ${LIBQUI_SRC_CORE_SRCS})

file(GLOB LIBQUI_SRC_TEMPLATE_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/src/${TEMPLATE_NAME}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/${TEMPLATE_NAME}/*.cc")
source_group("src\\\\${TEMPLATE_NAME}" FILES ${LIBQUI_SRC_TEMPLATE_SRCS})


# Determine the target output directory.
SET_QUI_TARGET_OUT_DIR()

if(USE_DLL)
  set(QUI_LIB_TYPE "SHARED")
else()
  set(QUI_LIB_TYPE "STATIC")
endif()

add_library(${QUI_TARGET} ${QUI_LIB_TYPE}
  ${LIBQUI_INCLUDE_SRCS}
  ${LIBQUI_INCLUDE_CORE_SRCS}
  ${LIBQUI_INCLUDE_TEMPLATE_SRCS}
  ${LIBQUI_SRC_SRCS}
  ${LIBQUI_SRC_CORE_SRCS}
  ${LIBQUI_SRC_TEMPLATE_SRCS}
  )
SET_LIBRARY_TARGET_PROPERTIES(${QUI_TARGET})
include_directories(${QUI_TARGET} ${CMAKE_CURRENT_SOURCE_DIR}/src)
if(USE_DLL)
  add_definitions(-DQUI_IMPLEMENTATION)
endif()

if (MSVC)
    set_target_properties(${QUI_TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    set_target_properties(${QUI_TARGET} PROPERTIES COMPILE_FLAGS "/Yuqui_precompile.h /FIqui_precompile.h")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/src/qui_precompile.cc" PROPERTIES COMPILE_FLAGS "/Ycqui_precompile.h")
    # GCC or Clang
    #add_definitions(-include dui_precompile.h)
endif(MSVC)

# Remove the default "lib" prefix from the resulting library.
set_target_properties(${QUI_TARGET} PROPERTIES PREFIX "")

# COPY_FILES("${QUI_TARGET}" "${QUI_BINARY_FILES}" "${QUI_TARGET_OUT_DIR}" "${QUI_BINARY_DIR}")

