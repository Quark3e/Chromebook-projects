cmake_minimum_required(VERSION 3.18)
project(implot_test0)

set(CMAKE_CXX_STANDARD 17)

set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
set(DIR_ROOT "../../../../..")
set(
    CMAKE_PREFIX_PATH
    ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/
)

find_package(implot CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/packages/implot_x64-windows/share/implot")
find_package(imguiwin REQUIRED)
# find_package(implot REQUIRED)


add_executable(${PROJECT_NAME} main.cpp)


target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_ROOT}/teststuff/cpp/useful)
target_include_directories(${PROJECT_NAME} PRIVATE imguiwin)
target_include_directories(${PROJECT_NAME} PRIVATE implot)


target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE implot::implot)
