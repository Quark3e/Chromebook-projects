cmake_minimum_required(VERSION 3.24)
project(guiNodeChart)

set(CMAKE_CXX_STANDARD 14)

# set(IMGUI_USER_CONFIG "\\\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\\\"")

if(WIN32)
find_path(DIR_IMGUI NAMES "imgui.h"
    HINTS "Libraries"
    REQUIRED
)
# set(DIR_IMGUI "C:/Users/berkh/Libraries")
# get_filename_component(DIR_IMGUI ${DIR_IMGUI} DIRECTORY)
else()
find_path(DIR_IMGUI
    NAMES "imgui.h"
    HINTS "include"
    REQUIRED
)
# set(DIR_IMGUI "/usr/local/include")
endif(WIN32)

message(DIR_IMGUI = ${DIR_IMGUI})

set(REPO_ROOT "../../../..")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")


include_directories(${REPO_ROOT})
include_directories(${DIR_IMGUI})
include_directories(${DIR_IMGUI}/imgui)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_HC_USEFUL}/robotics)
include_directories(${DIR_HC_USEFUL}/jsonParser)
include_directories(${PROJECT_SOURCE_DIR})

link_directories(${DIR_IMGUI}/imgui/backens)
link_directories(${CMAKE_PREFIX_PATH}/lib)


file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui*.cpp"
)


add_executable(${PROJECT_NAME}
    main.cpp
    ${PROJECT_SOURCE_DIR}/gui_nodeChart.cpp
    ${PROJECT_SOURCE_DIR}/globals_variables.cpp
    ${PROJECT_SOURCE_DIR}/menuScreens.cpp
    ${DIR_HC_USEFUL}/diy_dictionary.cpp
    ${DIR_HC_USEFUL}/jsonParser/jsonParser.cpp
    ${DIR_IMGUI}/backends/imgui_impl_allegro5.cpp
    ${DIR_IMGUI}/misc/cpp/imgui_stdlib.cpp
    ${DIR_IMGUI}/addons/imguidatechooser/imguidatechooser.cpp
    ${imgui_depends}
)

target_compile_options(${PROJECT_NAME}
    PUBLIC
    -fdiagnostics-color=always
)
target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_IMGUI})
target_link_libraries(${PROJECT_NAME}
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)