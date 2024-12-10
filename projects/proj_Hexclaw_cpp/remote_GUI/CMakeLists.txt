cmake_minimum_required(VERSION 3.24)
project(Hexclaw_Remote)


#find_path(DIR_HC_USEFUL
#    NAMES "useful.hpp"
#    HINTS "${CMAKE_PREFIX_PATH}/HC_useful"
#    REQUIRED
#)

set(REPO_ROOT "../../..")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful") #"/usr/local/include/HC_useful")
set(DIR_IMGUI "/usr/local/include/imgui")
#find_library()

#include_directories(../../..)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_IMGUI})
#add_subdirectory("${CMAKE_PREFIX_PATH}/imgui binary_dir)
#add_subdirectory(${CMAKE_PREFIX_PATH}/HC_useful binary_dir)

link_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${CMAKE_PREFIX_PATH}/imgui/backends)
link_directories(${CMAKE_PREFIX_PATH}/HC_useful)
link_directories(${CMAKE_PREFIX_PATH}/lib)

file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui*.cpp"
)

add_executable(remote_gui
    src/main.cpp
    src/global_variables.cpp
    src/tab_options.cpp
    ${DIR_HC_USEFUL}/diy_dictionary.cpp
    ${DIR_HC_USEFUL}/robotics/pathSchedule.cpp
    ${DIR_IMGUI}/backends/imgui_impl_allegro5.cpp
    ${imgui_depends}
)

target_link_libraries(remote_gui
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)
