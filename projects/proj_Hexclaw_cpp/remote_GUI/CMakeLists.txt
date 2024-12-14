cmake_minimum_required(VERSION 3.24)
project(Hexclaw_Remote)



if(WIN32)
# find_path(DIR_IMGUI NAMES "imgui.h"
#     HINTS "/Libraries/imgui"
#     REQUIRED
# )
set(DIR_IMGUI "C:/Users/berkh/Libraries")
# get_filename_component(DIR_IMGUI ${DIR_IMGUI} DIRECTORY)
else()
find_path(DIR_IMGUI NAMES "imgui.h"
    HINTS "include/imgui"
    REQUIRED
)
endif(WIN32)

message(DIR_IMGUI = ${DIR_IMGUI})

set(REPO_ROOT "../../..")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful") #"/usr/local/include/HC_useful")


add_subdirectory(${PROJECT_SOURCE_DIR}/lib/hexclaw_networkDataThread)

if(WIN32)
include_directories("C:\\msys64\\ucrt64\\include")
endif(WIN32)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories("${REPO_ROOT}/projects/proj_Hexclaw_cpp/in rpi")
include_directories(${PROJECT_SOURCE_DIR}/lib/hexclaw_networkDataThread)
include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_HC_USEFUL}/robotics)
include_directories(${DIR_IMGUI})
include_directories(${DIR_IMGUI}/imgui)


link_directories(${PROJECT_SOURCE_DIR}/src)
link_directories(${DIR_IMGUI}/imgui/backends)
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
