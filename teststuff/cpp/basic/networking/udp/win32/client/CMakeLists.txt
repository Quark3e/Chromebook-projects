cmake_minimum_required(VERSION 3.1)
project(udp_client)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} ws2_32)