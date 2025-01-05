cmake_minimum_required(VERSION 3.1)
project(udpCOM_toESP)

# CMAKE_PREFIX_PATH(opencv)

set(CMAKE_CXX_STANDARD 17)
set(REPO_ROOT ../../../../../..)


add_subdirectory(${PROJECT_SOURCE_DIR}/../../LIB_NETWORKCLASS binary_dir)

include_directories(${REPO_ROOT}/teststuff/cpp/useful/)
include_directories(${PROJECT_SOURCE_DIR}/../../LIB_NETWORKCLASS)

link_directories(${PROJECT_SOURCE_DIR}/../../LIB_NETWORKCLASS)

add_executable(udpCOM udpCOM.cpp)

if(WIN32)
    target_link_libraries(udpCOM ws2_32)
endif(WIN32)
target_link_libraries(udpCOM NETWORKCLASS_LIB)