cmake_minimum_required(VERSION 3.1)
project(udpAccel_pos)

# CMAKE_PREFIX_PATH(opencv)

set(CMAKE_CXX_STANDARD 14)

# set(OpenCV "/usr/local/include/opencv4")

# find_package(OpenCV REQUIRED)
find_package(PiPCA9685 REQUIRED)

# include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(udpAccel_pos main.cpp)
target_link_libraries(udpAccel_pos ${PiPCA9685_LIBRARY})
# target_link_libraries(DisplayImage ${OpenCV_LIBS})