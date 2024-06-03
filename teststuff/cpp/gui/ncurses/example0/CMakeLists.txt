cmake_minimum_required(VERSION 3.1)
project(guiEXE)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# find_package(PiPCA9685 REQUIRED)
# find_package(OpenCV REQUIRED)
# include_directories()

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(guiEXE main.cpp)


target_link_libraries(
    guiEXE
    ncurses
    rt
)
target_compile_options(
    guiEXE
    PUBLIC
    # -Wall
    -pthread
    -lncursesw
)

# target_link_libraries(guiEXE ${PiPCA9685_LIBRARY})
# target_link_libraries(guiEXE ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} -lncursesw)
