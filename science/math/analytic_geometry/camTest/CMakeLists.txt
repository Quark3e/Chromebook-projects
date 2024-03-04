cmake_minimum_required(VERSION 3.1)
project(camTest_cpp)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(cppEXE cpp_cameras.cpp)


target_link_libraries(
    cppEXE
    rt
    pigpio
)
target_compile_options(
    cppEXE
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(cppEXE ${OpenCV_LIBS})
