cmake_minimum_required(VERSION 3.24)
project(Hexclaw_Remote)

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


set(REPO_ROOT "../../..")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful") #"/usr/local/include/HC_useful")


# add_subdirectory(${REPO_ROOT}/teststuff/cpp/basic/networking/LIB_NETWORKCLASS binary_dir)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/hexclaw_networkDataThread)

include_directories(${PROJECT_SOURCE_DIR}/lib/hexclaw_networkDataThread)
# include_directories(${REPO_ROOT}/teststuff/cpp/basic/networking/LIB_NETWORKCLASS)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_IMGUI})
include_directories(${DIR_IMGUI}/imgui)
#add_subdirectory("${CMAKE_PREFIX_PATH}/imgui binary_dir)
#add_subdirectory(${CMAKE_PREFIX_PATH}/HC_useful binary_dir)



# link_directories(${REPO_ROOT}/teststuff/cpp/basic/networking/LIB_NETWORKCLASS)
link_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${CMAKE_PREFIX_PATH}/imgui/backends)
link_directories(${CMAKE_PREFIX_PATH}/HC_useful)
link_directories(${CMAKE_PREFIX_PATH}/lib)


file(GLOB imgui_depends CONFIGURE_DEPENDS
    "${DIR_IMGUI}/imgui/imgui*.cpp"
)

find_library(libHexclawDataThread
    "hexclaw_networkDataThread"
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


add_executable(remote_gui
    src/main.cpp
    src/global_variables.cpp
    src/tab_options.cpp
    ${DIR_HC_USEFUL}/diy_dictionary.cpp
    ${DIR_HC_USEFUL}/robotics/pathSchedule.cpp
    ${DIR_IMGUI}/imgui/backends/imgui_impl_allegro5.cpp
    ${imgui_depends}
)

target_link_libraries(remote_gui
    allegro
    allegro_main
    allegro_primitives
    allegro_image
)
target_link_libraries(remote_gui "${JPEG_LIB}")
# target_link_libraries(remote_gui NETWORKCLASS_LIB)
target_link_libraries(remote_gui libHexclawDataThread)
