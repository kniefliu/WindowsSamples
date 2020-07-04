# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

#
# This file is the QUI CMake configuration entry point and should be loaded
# using `find_package(QUI REQUIRED)`. See the top-level CMakeLists.txt file
# included with the QUI binary distribution for usage information.
#

# Find the QUI binary distribution root directory.
set(_QUI_ROOT "")
if(QUI_ROOT AND IS_DIRECTORY "${QUI_ROOT}")
  set(_QUI_ROOT "${QUI_ROOT}")
  set(_QUI_ROOT_EXPLICIT 1)
else()
  set(_ENV_QUI_ROOT "")
  if(DEFINED ENV{QUI_ROOT})
    file(TO_CMAKE_PATH "$ENV{QUI_ROOT}" _ENV_QUI_ROOT)
  endif()
  if(_ENV_QUI_ROOT AND IS_DIRECTORY "${_ENV_QUI_ROOT}")
    set(_QUI_ROOT "${_ENV_QUI_ROOT}")
    set(_QUI_ROOT_EXPLICIT 1)
  endif()
  unset(_ENV_QUI_ROOT)
endif()

if(NOT DEFINED _QUI_ROOT_EXPLICIT)
  message(FATAL_ERROR "Must specify a QUI_ROOT value via CMake or environment variable.")
endif()

if(NOT IS_DIRECTORY "${_QUI_ROOT}/cmake")
  message(FATAL_ERROR "No CMake bootstrap found for QUI binary distribution at: ${QUI_ROOT}.")
endif()

# Execute additional cmake files from the QUI binary distribution.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${_QUI_ROOT}/cmake")
include("qui_variables")
include("qui_macros")
