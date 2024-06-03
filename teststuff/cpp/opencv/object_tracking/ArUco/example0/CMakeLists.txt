cmake_minimum_required(VERSION 3.1)
project(ArUco_tracking)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(ArUco_tracker main.cpp)


target_link_libraries(
    ArUco_tracker
    rt
    pigpio
)
target_compile_options(
    ArUco_tracker
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(ArUco_tracker ${OpenCV_LIBS})
