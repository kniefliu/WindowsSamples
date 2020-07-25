# Copyright (c) 2016 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

# Must be loaded via FindQUI.cmake.
if(NOT DEFINED _QUI_ROOT_EXPLICIT)
  message(FATAL_ERROR "Use find_package(QUI) to load this file.")
endif()


#
# Shared configuration.
#

# Determine the platform.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  set(OS_MACOSX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  set(OS_LINUX 1)
  set(OS_POSIX 1)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  set(OS_WINDOWS 1)
endif()

# Determine the project architecture.
if(NOT DEFINED PROJECT_ARCH)
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(PROJECT_ARCH "x86_64")
  else()
    set(PROJECT_ARCH "x86")
  endif()
endif()

if(${CMAKE_GENERATOR} STREQUAL "Ninja")
  set(GEN_NINJA 1)
elseif(${CMAKE_GENERATOR} STREQUAL "Unix Makefiles")
  set(GEN_MAKEFILES 1)
endif()

# Determine the build type.
if(NOT CMAKE_BUILD_TYPE AND (GEN_NINJA OR GEN_MAKEFILES))
  # CMAKE_BUILD_TYPE should be specified when using Ninja or Unix Makefiles.
  set(CMAKE_BUILD_TYPE Release)
  message(WARNING "No CMAKE_BUILD_TYPE value selected, using ${CMAKE_BUILD_TYPE}")
endif()


# Path to the include directory.
set(QUI_INCLUDE_PATH "${_QUI_ROOT}/${TEMPLATE_NAME}_core/include")


# Shared compiler/linker flags.
list(APPEND QUI_COMPILER_DEFINES
  # Allow C++ programs to use stdint.h macros specified in the C99 standard that aren't 
  # in the C++ standard (e.g. UINT8_MAX, INT64_MIN, etc)
  __STDC_CONSTANT_MACROS __STDC_FORMAT_MACROS
  )


# Configure use of the sandbox.
option(USE_DLL "Enable or disable use of qui dll." ON)


#
# Windows configuration.
#

if(OS_WINDOWS)  
  if (USE_DLL)
    list(APPEND QUI_COMPILER_DEFINES
      COMPONENT_BUILD             # 
      )
  endif()

  # Consumers who run into LNK4099 warnings can pass /Z7 instead (see issue #385).
  set(QUI_DEBUG_INFO_FLAG "/Zi" CACHE STRING "Optional flag specifying specific /Z flag to use")

  # Consumers using different runtime types may want to pass different flags
  set(QUI_RUNTIME_LIBRARY_FLAG "/MD" CACHE STRING "Optional flag specifying which runtime to use")
  if (QUI_RUNTIME_LIBRARY_FLAG)
    list(APPEND QUI_COMPILER_FLAGS_DEBUG ${QUI_RUNTIME_LIBRARY_FLAG}d)
    list(APPEND QUI_COMPILER_FLAGS_RELEASE ${QUI_RUNTIME_LIBRARY_FLAG})
  endif()

  # Platform-specific compiler/linker flags.
  set(QUI_LIBTYPE STATIC)
  list(APPEND QUI_COMPILER_FLAGS
    /MP           # Multiprocess compilation
    /Gy           # Enable function-level linking
    /GR-          # Disable run-time type information
    /W4           # Warning level 4
    /WX           # Treat warnings as errors
    /wd4100       # Ignore "unreferenced formal parameter" warning
    /wd4127       # Ignore "conditional expression is constant" warning
    /wd4244       # Ignore "conversion possible loss of data" warning
    /wd4481       # Ignore "nonstandard extension used: override" warning
    /wd4512       # Ignore "assignment operator could not be generated" warning
    /wd4701       # Ignore "potentially uninitialized local variable" warning
    /wd4702       # Ignore "unreachable code" warning
    /wd4996       # Ignore "function or variable may be unsafe" warning
    ${QUI_DEBUG_INFO_FLAG}
    )
  list(APPEND QUI_COMPILER_FLAGS_DEBUG
    /RTC1         # Disable optimizations
    /Od           # Enable basic run-time checks
    )
  list(APPEND QUI_COMPILER_FLAGS_RELEASE
    /O2           # Optimize for maximum speed
    /Ob2          # Inline any suitable function
    /GF           # Enable string pooling
    )
  list(APPEND QUI_LINKER_FLAGS_DEBUG
    /DEBUG        # Generate debug information
    )
  list(APPEND QUI_EXE_LINKER_FLAGS
    /MANIFEST:NO        # No default manifest (see ADD_WINDOWS_MANIFEST macro usage)
    /LARGEADDRESSAWARE  # Allow 32-bit processes to access 3GB of RAM
    )
  list(APPEND QUI_COMPILER_DEFINES
    WIN32 _WIN32 _WINDOWS             # Windows platform
    UNICODE _UNICODE                  # Unicode build
    WINVER=0x0601 _WIN32_WINNT=0x601  # Targeting Windows 7
    NOMINMAX                          # Use the standard's templated min/max
    WIN32_LEAN_AND_MEAN               # Exclude less common API declarations
    _HAS_EXCEPTIONS=0                 # Disable exceptions
    )
  list(APPEND QUI_COMPILER_DEFINES_RELEASE
    NDEBUG _NDEBUG                    # Not a debug build
    )
  list(APPEND QUI_COMPILER_DEFINES_DEBUG
    QUI_DEBUG                    # A debug build
    )

  # Standard libraries.
  set(QUI_STANDARD_LIBS
    comctl32.lib
    rpcrt4.lib
    shlwapi.lib
    ws2_32.lib
    )

  # QUI directory paths.
  set(QUI_RESOURCE_DIR        "${_QUI_ROOT}/Resources")
  set(QUI_BINARY_DIR          "${_QUI_ROOT}/$<CONFIGURATION>")
  set(QUI_BINARY_DIR_DEBUG    "${_QUI_ROOT}/Debug")
  set(QUI_BINARY_DIR_RELEASE  "${_QUI_ROOT}/Release")

  # QUI library paths.
  set(QUI_LIB_DEBUG   "${QUI_BINARY_DIR_DEBUG}/${TEMPLATE_NAME}_core.lib")
  set(QUI_LIB_RELEASE "${QUI_BINARY_DIR_RELEASE}/${TEMPLATE_NAME}_core.lib")

  # List of QUI binary files.
  set(QUI_BINARY_FILES
    "${TEMPLATE_NAME}_core.dll"
    )

endif()
