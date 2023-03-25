cmake_minimum_required(VERSION 3.1)
project(adxl345_joan)

include_directories(include)

set(CMAKE_CXX_STANDARD 14)
set(pigpio "/usr/include")

add_executable(adxl345_joan adxl345_joan.cpp)
# add_subdirectory(pigpio)
target_include_directories(adxl345_joan PUBLIC pigpio)
