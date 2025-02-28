

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

set(REPO_ROOT "../../../../")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")


include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_IMGUI}/imgui)

link_directories(${DIR_IMGUI}/imgui/backends)
link_directories(${CMAKE_PREFIX_PATH}/lib)

file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui/imgui*.cpp"
)

add_library(imguiwin STATIC
    imguiwin.cpp
    ${DIR_IMGUI}/imgui/backends/imgui_impl_allegro5.cpp
    ${DIR_IMGUI}/imgui/misc/cpp/imgui_stdlib.cpp
    ${imgui_depends}
)

target_include_directories(imguiwin PUBLIC ${DIR_IMGUI})
target_link_libraries(imguiwin
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)