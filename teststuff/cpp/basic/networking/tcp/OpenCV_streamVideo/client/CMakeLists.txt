cmake_minimum_required(VERSION 3.12)
project(TCP_CamStream_OpenCV_client)

set(CMAKE_CXX_STANDARD 14)
# set(OpenCV "/usr/local/include")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")


add_definitions(-DCMAKE_BUILD_TYPE="Debug")
add_definitions(-D__TAKE_PERF=false)


find_package(OpenCV REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS binary_dir)

include_directories(../../../../../../..)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS)
link_directories(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS)

if(WIN32)
find_library(JPEG_LIB
    jpeg
    PATHS "C:\\Users\\berkh\\Libraries\\imgui\\examples\\example_allegro5\\vcpkg\\installed\\x64-windows\\lib"
    REQUIRED
)
else()
find_library(JPEG_LIB
    jpeg
    HINTS "${CMAKE_PREFIX_PATH}/arm-linux-gnueabihf"
    REQUIRED
)
endif(WIN32)

add_executable(
    cv_video_cli
    client.cpp
)


# target_compile_definitions(cv_video_cli PRIVATE CMAKE_BUILD_TYPE="Debug")
# target_compile_definitions(cv_video_cli PRIVATE TAKE_PERF=true)

# target_link_libraries(
#     cv_video_cli
#     rt
#     pigpio
# )
# target_compile_options(
#     cv_video_cli
#     PUBLIC
#     # -Wall
#     -pthread
# )

target_link_libraries(cv_video_cli "${JPEG_LIB}")
target_link_libraries(cv_video_cli ${OpenCV_LIBS})
target_link_libraries(cv_video_cli NETWORKCLASS_LIB)