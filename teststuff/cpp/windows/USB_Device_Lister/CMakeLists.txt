cmake_minimum_required(VERSION 3.10)

# Project name
project(USB_Device_Lister)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(USB_Device_Lister main.cpp)