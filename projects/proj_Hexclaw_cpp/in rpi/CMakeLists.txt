cmake_minimum_required(VERSION 3.1)
project(Hexclaw)

set(CMAKE_CXX_STANDARD 14)

find_package(PiPCA9685 REQUIRED)

# include_directories()

add_executable(Hexclaw_Main_2 Hexclaw_Main_2.cpp)
target_link_libraries(Hexclaw_Main_2 ${PiPCA9685_LIBRARY})
