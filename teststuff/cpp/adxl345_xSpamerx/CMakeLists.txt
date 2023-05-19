cmake_minimum_required(VERSION 3.1)
project(adxl345_xSpamerx)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE BUILD)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14")
# set(CMAKE_CXX_FLAGS "-std=c++17")


add_executable(adxl345 main.cpp)

target_link_libraries(adxl345 pigpio rt)
target_compile_options(adxl345 PUBLIC -Wall -pthread)

# set_property(TARGET adxl345 PROPERTY SKIP_BUILD_RPATH TRUE)
# install (TARGETS adxl345 RUNTIME DESTINATION bin)

#gcc -Wall -pthread -o adxl345_joan adxl345_joan.cpp -lpigpio -lrt
