cmake_minimum_required(VERSION 3.1)
project(OpenCV_DisplayImage)

# CMAKE_PREFIX_PATH(opencv)

set(CMAKE_CXX_STANDARD 14)

set(OpenCV "/usr/local/include")

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(DisplayImage DisplayImage.cpp)
target_link_libraries(DisplayImage ${OpenCV_LIBS})