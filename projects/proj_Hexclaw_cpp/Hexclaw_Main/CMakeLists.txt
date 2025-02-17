cmake_minimum_required(VERSION 3.18)
project(Hexclaw)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_FIND_DEBUG_MODE 1)
set(BUILD_MODE Debug)
# set(OpenCV "/usr/local/include")
# set(opt6_src "HW_options/opt6_terminal.cpp")
# set(CMAKE_CXX_FLAGS "-DIMGUI_USER_CONFIG=\\\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\\\"")
# set(CMAKE_BUILD_TYPE Debug)

set(REPO_ROOT "../../..")
set(DIR_CPP_USEFUL "${REPO_ROOT}/teststuff/cpp")
set(DIR_HC_USEFUL "${DIR_CPP_USEFUL}/useful")

if (MSVC)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    # warning level 4
    # add_compile_options(/FS)
else()
    # additional warnings
    # add_compile_options(-Wall -Wextra -Wpedantic)
    # add_compile_options(-FS)
endif()

if(WIN32)
    set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
    find_library(pdcurses
        pdcurses
        PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/lib/"
        REQUIRED
    )

    set(CMAKE_TOOLCHAIN_FILE "C:\\Users\\berkh\\Libraries\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")

else()
    find_package(PiPCA9685 REQUIRED)
endif(WIN32)


find_package(OpenCV REQUIRED)


include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_CPP_USEFUL})
include_directories(${DIR_CPP_USEFUL}/basic/Performance)
include_directories(${OpenCV_INCLUDE_DIRS})
# include_directories(/usr/local/include/imgui)
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


if(WIN32)

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
)

target_include_directories(Hexclaw_Main PRIVATE ${DIR_PDCURSES}/include/)
target_link_libraries(Hexclaw_Main ${pdcurses})

else()

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
    # /usr/local/include/imgui/backends/imgui_impl_allegro5.cpp
    # /usr/local/include/imgui/imgui_demo.cpp
    # /usr/local/include/imgui/imgui_draw.cpp
    # /usr/local/include/imgui/imgui_tables.cpp
    # /usr/local/include/imgui/imgui_widgets.cpp
    # /usr/local/include/imgui/imgui.cpp
    ../PiPCA9685/src/PCA9685.cpp
)

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
endif(WIN32)

target_link_libraries(Hexclaw_Main ${OpenCV_LIBS})
