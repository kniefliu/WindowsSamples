# Copyright (c) 2020 The IQIYI RND Lyra Authors. All rights reserved. 
# Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# Source files.
#

# sources.
file(GLOB QUI_D3D11_TUTORIAL01_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")

#
# Shared configuration.
#

# Target executable names.
set(QUI_TARGET "${TEMPLATE_NAME}_tutorial01")

# Determine the target output directory.
SET_QUI_TARGET_OUT_DIR()

#
# Windows configuration.
#

if(OS_WINDOWS)
  # Executable target.
  add_executable(${QUI_TARGET} WIN32 ${QUI_D3D11_TUTORIAL01_SRCS})
  add_dependencies(${QUI_TARGET} ${TEMPLATE_NAME}_core)
  SET_EXECUTABLE_TARGET_PROPERTIES(${QUI_TARGET})
  target_link_libraries(${QUI_TARGET} ${TEMPLATE_NAME}_core ${QUI_STANDARD_LIBS})
  
  if (MSVC)
    set_target_properties(${QUI_TARGET} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")
    set_target_properties(${QUI_TARGET} PROPERTIES COMPILE_FLAGS "/Yuprecompile.h /FIprecompile.h")
    set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/precompile.cc" PROPERTIES COMPILE_FLAGS "/Ycprecompile.h")
    # GCC or Clang
    #add_definitions(-include dui_precompile.h)
  endif(MSVC)

  # Add the custom manifest files to the executable.
  #ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${QUI_TARGET}" "exe")

  # Copy binary and resource files to the target output directory.
  #COPY_FILES("${QUI_TARGET}" "${QUI_BINARY_FILES}" "${QUI_BINARY_DIR}" "${QUI_TARGET_OUT_DIR}")
  #COPY_FILES("${QUI_TARGET}" "skin" "${QUI_RESOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
  #COPY_FILES("${QUI_TARGET}" "skin" "${CMAKE_CURRENT_SOURCE_DIR}" "${QUI_TARGET_OUT_DIR}")
endif()
