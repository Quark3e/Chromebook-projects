# cmake_minimum_required(VERSION 3.1)
# project(adxl345_joan)

# include_directories(pigpio)

# set(CMAKE_CXX_STANDARD 14)
# set(pigpio "/usr/include")

# add_executable(adxl345_joan adxl345_joan.cpp)
# # add_subdirectory(pigpio)
# # target_include_directories(adxl345_joan ${pigpio})


cmake_minimum_required(VERSION 3.1)
project(adxl345_joan)

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
# set(CMAKE_CXX_FLAGS "-std=c++17")


add_executable(adxl345 adxl345_joan.cpp)

target_link_libraries(adxl345 pigpio rt pthread bsd)

set_property(TARGET adxl345 PROPERTY SKIP_BUILD_RPATH TRUE)
install (TARGETS adxl345 RUNTIME DESTINATION bin)