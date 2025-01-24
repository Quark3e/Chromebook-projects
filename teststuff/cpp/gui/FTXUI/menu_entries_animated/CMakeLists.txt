cmake_minimum_required(VERSION 3.18)
project(ftxui_menu_entries_animated)

if(WIN32)
    set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
    find_package(ftxui
        CONFIG REQUIRED
        PATHS "C:/Users/berkh/Libraries/vcpkg/packages/ftxui_x64-windows/share/ftxui"
    )
else()
    include(FetchContent)
    set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
    FetchContent_Declare(ftxui
        GIT_TAG "origin/main"
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
    )
    
    FetchContent_GetProperties(ftxui)

    if(NOT ftxui_POPULATED)
        FetchContent_Populate(ftxui)
        add_subdirectory(${ftxui_SOURCE_DIR} ${ftxui_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
endif(WIN32)


set(CMAKE_CXX_STANDARD 17)
set(DIR_HC_USEFUL "${PROJECT_SOURCE_DIR}/../../../useful/")

if(WIN32)
    set(CMAKE_TOOLCHAIN_FILE "C:/Users/berkh/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")
else()
endif(WIN32)


include_directories(${DIR_HC_USEFUL})
link_directories(${DIR_HC_USEFUL})

add_executable(${PROJECT_NAME}
    main.cpp
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ftxui::dom ftxui::screen ftxui::component)

else()
    target_include_directories(${PROJECT_NAME} PRIVATE src)
    target_link_libraries(${PROJECT_NAME} PRIVATE ftxui::dom ftxui::screen ftxui::component)

endif(WIN32)