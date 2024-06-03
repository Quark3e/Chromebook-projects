cmake_minimum_required(VERSION 3.1)
project(Hexclaw)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")
# set(opt6_src "HW_options/opt6_terminal.cpp")
set(CMAKE_CXX_FLAGS "-DIMGUI_USER_CONFIG=\\\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\\\"")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(PiPCA9685 REQUIRED)
find_package(OpenCV REQUIRED)
# find_package(imgui REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/local/include/imgui/)

# add_subdirectory(HW_options)
# target_sources(Hexclaw_Main PRIVATE opt6_terminal.cpp)

# target_sources(${{PROJECT_NAME}} PRIVATE HW_options/opt6_terminal.cpp)

add_executable(
    Hexclaw_Main
    main.cpp
    /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp
    /usr/local/include/imgui/imgui_demo.cpp
    /usr/local/include/imgui/imgui_draw.cpp
    /usr/local/include/imgui/imgui_tables.cpp
    /usr/local/include/imgui/imgui_widgets.cpp
    /usr/local/include/imgui/imgui.cpp
)

# target_include_directories(
#     Hexclaw_Main
#     PUBLIC
# )

target_link_libraries(
    Hexclaw_Main
    bcm2835
    rt
    pigpio
    ST7735_TFT_RPI
    menu
    ncurses
    allegro
    allegro_main
    allegro_primitives
)
target_compile_options(
    Hexclaw_Main
    PUBLIC
    # -Wall
    -pthread
    -lmenu
    -lncurses
)

target_link_libraries(Hexclaw_Main ${PiPCA9685_LIBRARY})
target_link_libraries(Hexclaw_Main ${OpenCV_LIBS})


# cmake_minimum_required(VERSION 3.1)
# project(Hexclaw)

# set(CMAKE_CXX_STANDARD 14)
# set(OpenCV "/usr/local/include")
# # set(opt6_src "HW_options/opt6_terminal.cpp")
# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")


# find_package(PiPCA9685 REQUIRED)
# find_package(OpenCV REQUIRED)
# # include_directories()

# include_directories(${OpenCV_INCLUDE_DIRS})

# # add_subdirectory(HW_options)
# # target_sources(Hexclaw_Main PRIVATE opt6_terminal.cpp)

# # target_sources(${{PROJECT_NAME}} PRIVATE HW_options/opt6_terminal.cpp)

# add_executable(Hexclaw_Main main.cpp)


# target_link_libraries(
#     Hexclaw_Main
#     bcm2835
#     rt
#     pigpio
#     ST7735_TFT_RPI
#     menu
#     ncurses
# )
# target_compile_options(
#     Hexclaw_Main
#     PUBLIC
#     # -Wall
#     -pthread
#     -lmenu
#     -lncurses
# )

# target_link_libraries(Hexclaw_Main ${PiPCA9685_LIBRARY})
# target_link_libraries(Hexclaw_Main ${OpenCV_LIBS})

