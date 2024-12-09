cmake_minimum_required(VERSION 3.1)
project(libjpeg_decode)

# set(CMAKE_CXX_STANDARD 14)
# set(OpenCV "/usr/local/include")

# find_package(OpenCV REQUIRED)

find_library(JPEG_LIB
    jpeg
    HINTS "${CMAKE_PREFIX_PATH}/arm-linux-gnueabihf"
    REQUIRED
)

# include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(
    decoder
    main.cpp
)

target_link_libraries(decoder PRIVATE "${JPEG_LIB}")
# target_compile_options(
#     cv_video_srv
#     PUBLIC
#     # -Wall
#     -pthread
# )

target_link_libraries(decoder ${OpenCV_LIBS})