cmake_minimum_required(VERSION 3.1)
project(udpCOM_toESP)

# CMAKE_PREFIX_PATH(opencv)

set(CMAKE_CXX_STANDARD 14)

# set(OpenCV "/usr/local/include/opencv4")

# find_package(OpenCV REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(udpCOM udpCOM.cpp)
# target_link_libraries(DisplayImage ${OpenCV_LIBS})