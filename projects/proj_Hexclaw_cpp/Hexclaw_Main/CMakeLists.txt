cmake_minimum_required(VERSION 3.18)
project(Hexclaw_Main)

set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_FIND_DEBUG_MODE 1)
# set(BUILD_MODE Debug)

set(DIR_ROOT "../../..")
set(DIR_CPP_USEFUL "${DIR_ROOT}/teststuff/cpp")

set(
    CMAKE_PREFIX_PATH
    # ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/
    # ${DIR_ROOT}/teststuff/cpp/useful/lib_SystemOfCameras/build/
    ${DIR_ROOT}/teststuff/cpp/TwoCamCoordinate/lib/build/
    # ${DIR_ROOT}/science/math/PolyRegionCheck/lib/build/
)


if (MSVC)
    # # warning level 4
    # add_compile_options(/FS)
else()
    # additional warnings
    # add_compile_options(-Wall -Wextra -Wpedantic)
    # add_compile_options(-FS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:\\Users\\berkh\\Libraries\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
    set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
    
    find_library(pdcurses
        pdcurses
        PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/lib/"
        REQUIRED
    )

    set(EIGEN3_INCLUDE_DIR "C:/msys64/mingw64/include/eigen3")
else()
    find_package(PiPCA9685 REQUIRED)
    
    set(EIGEN3_INCLUDE_DIR "/usr/local/include/eigen3")
endif(WIN32)


find_package(OpenCV REQUIRED)
find_package(TwoCamCoordinate REQUIRED)


file(GLOB HW_src CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM HW_src "${PROJECT_SOURCE_DIR}/src/main.cpp")


set(HW_depends 
    ${DIR_CPP_USEFUL}/useful//diy_dictionary.cpp
    ${DIR_CPP_USEFUL}/useful//createTable.cpp
    ${DIR_CPP_USEFUL}/useful//wirelessCOM.cpp
    ${DIR_CPP_USEFUL}/useful//stringToFunc.cpp
    ${DIR_CPP_USEFUL}/useful//IR_camTrack.cpp
    ${DIR_CPP_USEFUL}/useful//terminalMenu.cpp
    ${DIR_CPP_USEFUL}/useful/camObjTracker/camObjTracker.cpp
    ${DIR_CPP_USEFUL}/basic/Performance/getPerformance.cpp
)


if(WIN32)
else()
    list(APPEND HW_depends ../PiPCA9685/src/PCA9685.cpp)
endif(WIN32)

add_executable(${PROJECT_NAME}
    src/main.cpp
    ${HW_depends}
    ${HW_src}
)

if(WIN32)
    target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_PDCURSES}/include/)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${pdcurses})
else()
    target_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
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
        ${PROJECT_NAME}
        PUBLIC
        # -Wall
        -Wno-psabi
        -pthread
        -lmenu
        -lncurses
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE ${PiPCA9685_LIBRARY})
endif(WIN32)


target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/lib)
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_CPP_USEFUL}/useful/)
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_CPP_USEFUL})
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_CPP_USEFUL}/basic/Performance)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_ROOT}/teststuff/cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${EIGEN3_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE TwoCamCoordinate)

target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME} PRIVATE TwoCamCoordinate::TwoCamCoordinate)

# add_definitions(-D_PROGRAM_PATH=${CMAKE_SOURCE_DIR})
# add_definitions(-D_PROGRAM_PATH__EXE=${CMAKE_BUILD_RPATH})
# add_definitions(-D_PROGRAM_PATH__DATA=${PROGRAM_PATH}/data)

target_compile_definitions(${PROJECT_NAME} PRIVATE _PROGRAM_PATH=${CMAKE_SOURCE_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE _PROGRAM_PATH__EXE=${CMAKE_CURRENT_BINARY_DIR})
target_compile_definitions(${PROJECT_NAME} PRIVATE _PROGRAM_PATH__DATA=${CMAKE_SOURCE_DIR}/data)