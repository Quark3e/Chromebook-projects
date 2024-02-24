cmake_minimum_required(VERSION 3.1)
project(opencv_VideoRecorder)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(opencv_recorderEXE videoRecorder.cpp)


target_link_libraries(
    opencv_recorderEXE
    rt
    pigpio
)
target_compile_options(
    opencv_recorderEXE
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(opencv_recorderEXE ${OpenCV_LIBS})
