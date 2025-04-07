cmake_minimum_required(VERSION 3.10)

# Project name
project(hsv_tracking)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenCV package
find_package(OpenCV REQUIRED)

# Add the executable
add_executable(hsv_tracking main.cpp)

# Link OpenCV libraries
target_link_libraries(hsv_tracking ${OpenCV_LIBS})


set(DIR_CPP_USEFUL ${PROJECT_SOURCE_DIR}/../../..)
target_include_directories(hsv_tracking PRIVATE ${DIR_CPP_USEFUL})
