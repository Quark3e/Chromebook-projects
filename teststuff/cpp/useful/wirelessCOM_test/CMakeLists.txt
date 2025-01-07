cmake_minimum_required(VERSION 3.1)
project(wirelessCOM_test)

# CMAKE_PREFIX_PATH(opencv)

set(CMAKE_CXX_STANDARD 17)
# set(REPO_ROOT ../../../../..)

include_directories(..)


add_executable(${PROJECT_NAME} main.cpp ../wirelessCOM.cpp)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif(WIN32)