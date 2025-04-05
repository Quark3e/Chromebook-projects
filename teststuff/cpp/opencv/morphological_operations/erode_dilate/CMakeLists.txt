cmake_minimum_required(VERSION 3.18)
project(erode_dilate)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_FIND_DEBUG_MODE 1)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})