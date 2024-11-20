cmake_minimum_required(VERSION 3.1)
project(TCP_CamStream_OpenCV_server)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cv_video_srv server.cpp)


target_link_libraries(
    cv_video_srv
    rt
    pigpio
)
target_compile_options(
    cv_video_srv
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(cv_video_srv ${OpenCV_LIBS})
