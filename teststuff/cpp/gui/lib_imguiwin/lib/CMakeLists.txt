cmake_minimum_required(VERSION 3.19)
project(imguiwin)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)
    # find_path(DIR_IMGUI
    #     NAMES "imgui.h"
    #     HINTS "Libraries"
    #     REQUIRED
    # )
    
    find_package(Allegro CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share/allegro5")
    find_package(imgui CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share/imgui")

    # set(DIR_IMGUI "C:/Users/berkh/Libraries")
    # message(DIR_IMGUI : ${DIR_IMGUI})
else()
    # find_path(DIR_IMGUI
    #     NAMES "imgui.h"
    #     HINTS "imgui"
    #     REQUIRED
    # )
    set(DIR_IMGUI "/usr/local/include/imgui")
endif(WIN32)

# get_filename_component(DIR_IMGUI ${DIR_IMGUI} DIRECTORY)

set(REPO_ROOT "../../../../../")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")


# include_directories(${DIR_HC_USEFUL})
# include_directories(${DIR_IMGUI}/imgui)

# file(GLOB imgui_depends CONFIGURE_DEPENDS
#     "${DIR_IMGUI}/imgui*.cpp"
# )
# set(HEADERS
#     ${DIR_IMGUI}/imgui.h
#     ${DIR_IMGUI}/misc/cpp/imgui_stdlib.h
#     ${DIR_IMGUI}/imgui_impl_allegro5.h
# )

add_library(imguiwin STATIC imguiwin.cpp)
# add_library(imguiwin STATIC
#     ${HEADERS}
#     imguiwin.cpp
#     ${DIR_IMGUI}/imgui_impl_allegro5.cpp
#     ${DIR_IMGUI}/misc/cpp/imgui_stdlib.cpp
#     ${imgui_depends}
# )
# target_link_directories(imguiwin PUBLIC ${DIR_IMGUI}/backends)

add_library(imguiwin::imguiwin ALIAS imguiwin)

target_include_directories(imguiwin PUBLIC ${DIR_HC_USEFUL})
# target_include_directories(imguiwin PUBLIC ${DIR_IMGUI}/imgui)
# target_link_directories(imguiwin ${DIR_IMGUI}/backends)

target_include_directories(imguiwin PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
# target_include_directories(imguiwin PUBLIC ${DIR_IMGUI})
target_include_directories(imguiwin PUBLIC ${imgui})
target_include_directories(imguiwin PUBLIC ${Allegro})
target_link_libraries(imguiwin PUBLIC
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)
target_link_libraries(imguiwin PUBLIC ${imgui})
target_link_libraries(imguiwin PUBLIC ${Allegro})


## Packaging rules
include(GNUInstallDirs)

install(TARGETS imguiwin
        EXPORT imguiwin_targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export( EXPORT imguiwin_targets
        NAMESPACE imguiwin::
        FILE "${CMAKE_INSTALL_LIBDIR}/cmake/imguiwin/imguiwin-config.cmake")




# cmake --build build
