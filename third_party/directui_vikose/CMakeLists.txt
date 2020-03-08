# CMake root file for directui_viksoe related apps
# Date		:  2017-12-07
# Author  	:  kniefliu (kniefliu@163.com)
#
# 	CMake files for UIlib
# 	file for each app is under the related sub-dir
#

cmake_minimum_required(VERSION 2.8)

project(UIlib_viksoe)

message(STATUS, "CMake project files for UIlib_viksoe")

# _MBCS or (UNICODE _UNICODE)
add_definitions(-D_MBCS)

# add each CMake file
add_subdirectory(UIlib)
add_subdirectory(App)
