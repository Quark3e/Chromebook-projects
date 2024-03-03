cmake_minimum_required(VERSION 3.1)
project(TwoCamTriangle)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(TwoCamCoordinate_exe main.cpp)


target_link_libraries(
    TwoCamCoordinate_exe
    rt
)
target_compile_options(
    TwoCamCoordinate_exe
    PUBLIC
    # -Wall
    -pthread
)

target_link_libraries(TwoCamCoordinate_exe ${OpenCV_LIBS})
