cmake_minimum_required(VERSION 3.1)
project(Image_to_ASCII)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(img_ascii main.cpp)


target_link_libraries(
    img_ascii
    rt
    pigpio
)
target_compile_options(
    img_ascii
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(img_ascii ${OpenCV_LIBS})
