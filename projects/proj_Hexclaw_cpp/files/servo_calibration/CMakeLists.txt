cmake_minimum_required(VERSION 3.1)
project(servo_calibration)

set(CMAKE_CXX_STANDARD 14)

find_package(PiPCA9685 REQUIRED)


add_executable(servo_cal main.cpp)
target_link_libraries(servo_cal ${PiPCA9685_LIBRARY})
