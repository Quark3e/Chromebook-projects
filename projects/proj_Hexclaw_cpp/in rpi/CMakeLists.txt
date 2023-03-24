cmake_minimum_required(VERSION 3.1)
project(Hexclaw)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(PiPCA9685 REQUIRED)
find_package(OpenCV REQUIRED)

# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(Hexclaw_Main Hexclaw_Main.cpp)
target_link_libraries(Hexclaw_Main ${PiPCA9685_LIBRARY})
