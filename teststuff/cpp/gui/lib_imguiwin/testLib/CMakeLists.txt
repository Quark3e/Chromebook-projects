cmake_minimum_required(VERSION 3.18)
project(lib_imguiwin_test)

set(CMAKE_CXX_STANDARD 17)
# set(IMGUI_USER_CONFIG "\\\"/usr/local/include/imgui/examples/example_allegro5/imconfig_allegro5.h\\\"")

set(CMAKE_FIND_DEBUG_MODE 1)

add_subdirectory(${PROJECT_SOURCE_DIR}/../ binary_dir)

include_directories(${PROJECT_SOURCE_DIR}/../)
link_directories(${PROJECT_SOURCE_DIR}/../)

# find_library(imguiwin name imguiwin)

#--------------

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


    set(REPO_ROOT "C:/Users/berkh/Projects/Github_repo/Chromebook-projects")
    set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")

    message("DIR_HC_USEFUL = " ${DIR_HC_USEFUL})

    include_directories(${DIR_HC_USEFUL})
    # include_directories(${DIR_IMGUI})
    include_directories(${DIR_IMGUI}/imgui)
    # link_directories(${DIR_IMGUI}/imgui/backends)

#--------------
add_executable(${PROJECT_NAME}
    main.cpp
)


target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} imguiwin)
