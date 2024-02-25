cmake_minimum_required(VERSION 3.1)
project(test_IR_camTrack)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(test_IR_camTrack main.cpp)


target_link_libraries(
    test_IR_camTrack
    rt
    pigpio
)
target_compile_options(
    test_IR_camTrack
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(test_IR_camTrack ${OpenCV_LIBS})
