cmake_minimum_required(VERSION 3.19)
project(imguiwin)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    find_path(DIR_IMGUI
        NAMES "imgui.h"
        HINTS "Libraries"
        REQUIRED
    )
    # set(DIR_IMGUI "C:/Users/berkh/Libraries")
else()
    # find_path(DIR_IMGUI
    #     NAMES "imgui.h"
    #     HINTS "imgui"
    #     REQUIRED
    # )
    set(DIR_IMGUI "/usr/local/include/imgui")
endif(WIN32)

get_filename_component(DIR_IMGUI ${DIR_IMGUI} DIRECTORY)

set(REPO_ROOT "../../../../../")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")


# include_directories(${DIR_HC_USEFUL})
# include_directories(${DIR_IMGUI}/imgui)

link_directories(${DIR_IMGUI}/imgui/backends)

file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui/imgui*.cpp"
)
set(HEADERS
    ${DIR_IMGUI}/imgui/imgui.h
    ${DIR_IMGUI}/imgui/misc/cpp/imgui_stdlib.h
    ${DIR_IMGUI}/imgui/backends/imgui_impl_allegro5.h
)


add_library(imguiwin STATIC
    ${HEADERS}
    imguiwin.cpp
    ${DIR_IMGUI}/imgui/backends/imgui_impl_allegro5.cpp
    ${DIR_IMGUI}/imgui/misc/cpp/imgui_stdlib.cpp
    ${imgui_depends}
)
add_library(imguiwin::imguiwin ALIAS imguiwin)

target_include_directories(imguiwin PUBLIC ${DIR_HC_USEFUL})
target_include_directories(imguiwin PUBLIC ${DIR_IMGUI}/imgui)
# target_link_directories(imguiwin ${DIR_IMGUI}/imgui/backends)

target_include_directories(imguiwin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
target_include_directories(imguiwin PUBLIC ${DIR_IMGUI})
target_link_libraries(imguiwin
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)


## Packaging rules
include(GNUInstallDirs)

install(TARGETS imguiwin
        EXPORT imguiwin_targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export( EXPORT imguiwin_targets
        NAMESPACE imguiwin::
        FILE "${CMAKE_INSTALL_LIBDIR}/cmake/imguiwin/imguiwin-config.cmake")




# cmake --build build
