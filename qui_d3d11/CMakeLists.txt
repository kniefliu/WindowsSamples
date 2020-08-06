#
# Global setup.
#

cmake_minimum_required(VERSION 3.17.3)

# Only generate Debug and Release configuration types.
set(CMAKE_CONFIGURATION_TYPES Debug Release)

# Project name.
set(TEMPLATE_NAME d3d11)
project(${TEMPLATE_NAME})

# Use folders in the resulting project files.
set_property(GLOBAL PROPERTY OS_FOLDERS ON)


set(QUI_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${QUI_ROOT}/cmake")


#
# Load the QUI configuration.
#

# Execute FindQUI.cmake which must exist in CMAKE_MODULE_PATH.
find_package(QUI REQUIRED)


#
# Define QUI-based targets.
#

# Include the qui target.
# Comes from the libqui_dll/CMakeLists.txt file in the binary distribution
# directory.
add_subdirectory(${TEMPLATE_NAME}_core)

# Include application targets.
# Comes from the <target>/CMakeLists.txt file in the current directory.
# TODO: Change these lines to match your project target when you copy this file.
add_subdirectory(demos/${TEMPLATE_NAME}_tutorial01)

# 
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TEMPLATE_NAME}_tutorial01)

# Display configuration settings.
PRINT_QUI_CONFIG()

