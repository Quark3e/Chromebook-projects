cmake_minimum_required(VERSION 3.18)
project(Hexclaw)

set(CMAKE_CXX_STANDARD 14)
set(OpenCV "/usr/local/include")
# set(opt6_src "HW_options/opt6_terminal.cpp")
set(CMAKE_CXX_FLAGS "-DIMGUI_USER_CONFIG=\\\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\\\"")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(REPO_ROOT "../../..")
set(DIR_CPP_USEFUL "${REPO_ROOT}/teststuff/cpp")
set(DIR_HC_USEFUL "${DIR_CPP_USEFUL}/useful")

find_package(PiPCA9685 REQUIRED)
find_package(OpenCV REQUIRED)
# find_package(imgui REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_CPP_USEFUL})
include_directories(${DIR_CPP_USEFUL}/basic/Performance)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/local/include/imgui)
include_directories(${REPO_ROOT}/teststuff/cpp)


link_directories(${DIR_HC_USEFUL})
link_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${REPO_ROOT}/teststuff/cpp)
# add_subdirectory(HW_options)
# target_sources(Hexclaw_Main PRIVATE opt6_terminal.cpp)

# target_sources(${{PROJECT_NAME}} PRIVATE HW_options/opt6_terminal.cpp)

file(GLOB HW_depends CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

list(REMOVE_ITEM HW_depends "${PROJECT_SOURCE_DIR}/src/main.cpp")

message(HW_DEPENDS = ${HW_depends})
message(PROJ_src_DIR = ${PROJECT_SOURCE_DIR})

add_executable(Hexclaw_Main
    src/main.cpp
    ${DIR_HC_USEFUL}/diy_dictionary.cpp
    ${DIR_HC_USEFUL}/createTable.cpp
    ${DIR_HC_USEFUL}/wirelessCOM.cpp
    ${DIR_HC_USEFUL}/stringToFunc.cpp
    ${DIR_HC_USEFUL}/IR_camTrack.cpp
    ${DIR_HC_USEFUL}/terminalMenu.cpp
    ${DIR_CPP_USEFUL}/basic/Performance/getPerformance.cpp
    ${DIR_CPP_USEFUL}/two_cam_coordinate/two_cam_coordinate.cpp
    ${HW_depends}
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
    -Wno-psabi
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

