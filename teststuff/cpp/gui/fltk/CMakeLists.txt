cmake_minimum_required(VERSION 3.15)

project(hello)

# optional (see below):
set(FLTK_DIR "/home/pi/fltk"
    CACHE FILEPATH "FLTK installation or build directory")

find_package(FLTK CONFIG REQUIRED)

add_executable       (hello WIN32 MACOSX_BUNDLE main.cpp)
target_link_libraries(hello PRIVATE fltk::fltk)