cmake_minimum_required(VERSION 2.8)

project(dui)

message(STATUS," CMake project files for dui")

# this line is for UNICODE release,which is required by DuiDesigner 
add_definitions(-DUNICODE -D_UNICODE -DDUI_DLL)

# add each CMake file
add_subdirectory(dui)
add_subdirectory(demos/test_app)
