cmake_minimum_required(VERSION 3.0)
project(UDP_CamStream_OpenCV_client)

# set(CMAKE_CXX_STANDARD 14)
# set(OpenCV "/usr/local/include")
 
find_package(OpenCV REQUIRED)

add_subdirectory(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS binary_dir)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS)
link_directories(${PROJECT_SOURCE_DIR}/../../../LIB_NETWORKCLASS)

add_executable(
    cv_video_cli
    main.cpp
)


# target_link_libraries(
#     cv_video_cli
#     rt
#     pigpio
# )
target_compile_options(
    cv_video_cli
    PUBLIC
    -Wall
    # -pthread
)

target_link_libraries(cv_video_cli ${OpenCV_LIBS})
target_link_libraries(cv_video_cli NETWORKCLASS_LIB)