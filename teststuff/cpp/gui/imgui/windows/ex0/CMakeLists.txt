cmake_minimum_required(VERSION 3.24)
project(imgui_test CXX)

if(WIN32)
find_path(DIR_IMGUI
    NAMES "imgui.h"
    HINTS "/Libraries"
    REQUIRED
)
else()
find_path(DIR_IMGUI
    NAMES "imgui/imgui.h"
    HINTS /imgui
    REQUIRED
)
endif(WIN32)


set(REPO_ROOT "../../../../../..")

include_directories(${DIR_IMGUI})
include_directories(${DIR_IMGUI}/imgui)


link_directories(${DIR_IMGUI}/imgui/backends)
# link_directories(${DIR_IMGUI})


file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui/imgui*.cpp"
)


# if(WIN32)
# find_library(JPEG_LIB
#     jpeg
#     PATHS "C:\\Users\\berkh\\Libraries\\imgui\\examples\\example_allegro5\\vcpkg\\installed\\x64-windows\\lib"
#     REQUIRED
# )
# else()
# find_library(JPEG_LIB
#     jpeg
#     HINTS "${CMAKE_PREFIX_PATH}/arm-linux-gnueabihf" "${CMAKE_PREFIX_PATH}/x86_64-linux-gnu/"
#     REQUIRED
# )
# endif(WIN32)


add_executable(imgui_test
    main.cpp
    ${DIR_IMGUI}/imgui/backends/imgui_impl_allegro5.cpp
    ${imgui_depends}
)
target_include_directories(imgui_test PUBLIC ${DIR_IMGUI}/imgui)
target_link_libraries(imgui_test
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)
# target_link_libraries(imgui_test "${JPEG_LIB}")
# target_link_libraries(imgui_test libHexclawDataThread)
