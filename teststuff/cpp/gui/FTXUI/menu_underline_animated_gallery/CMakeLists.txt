cmake_minimum_required(VERSION 3.18)
project(ftxui_menu_underline_animated_gallery)

if(WIN32)
set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
find_package(ftxui
    CONFIG REQUIRED
    PATHS "C:/Users/berkh/Libraries/vcpkg/packages/ftxui_x64-windows/share/ftxui"
)


endif(WIN32)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/berkh/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_CXX_STANDARD 17)
set(DIR_HC_USEFUL "${PROJECT_SOURCE_DIR}/../../../useful/")

include_directories(${DIR_HC_USEFUL})

link_directories(${DIR_HC_USEFUL})


add_executable(${PROJECT_NAME}
    main.cpp
)

if(WIN32)

target_link_libraries(${PROJECT_NAME} PRIVATE ftxui::dom ftxui::screen ftxui::component)


else()

endif(WIN32)
