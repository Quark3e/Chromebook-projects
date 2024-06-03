cmake_minimum_required(VERSION 3.1)
project(ST7735)

set(CMAKE_CXX_STANDARD 14)

add_executable(st7735 src/main.cpp)

target_link_libraries(
    st7735
    bcm2835
    rt
    ST7735_TFT_RPI
)
target_compile_options(
    st7735 
    PUBLIC
    -Wall
    -pthread
)

set_target_properties(st7735 PROPERTIES RUNTIME_OUTPUT_DIRECTORY "/home/pi/Chromebook-projects/teststuff/electronics/tft_display/ST7735_TFT_RPI_test/ST7735_TFT_RPI-1.5/example/bin")

# g++ -Iinclude/  -c -Wall  src/main.cpp -o  obj/main.o 
# g++ obj/main.o -o bin/test -lbcm2835 -lrt -lST7735_TFT_RPI