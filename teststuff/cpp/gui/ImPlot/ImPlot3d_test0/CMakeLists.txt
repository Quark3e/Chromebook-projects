cmake_minimum_required(VERSION 3.18)
project(implot3d_test0)

set(CMAKE_CXX_STANDARD 17)

set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")
set(DIR_ROOT "../../../../..")
set(
    CMAKE_PREFIX_PATH
    ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/
)
# find_package(Allegro CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share/allegro5")
find_package(imgui CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share/imgui")

find_package(implot3d CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/packages/implot3d_x64-windows/share/implot3d")
find_package(imguiwin REQUIRED)
# find_package(implot3d REQUIRED)


add_executable(${PROJECT_NAME} main.cpp)


target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_ROOT}/teststuff/cpp/useful)
target_include_directories(${PROJECT_NAME} PRIVATE imguiwin)
target_include_directories(${PROJECT_NAME} PRIVATE implot3d)
# target_include_directories(${PROJECT_NAME} PRIVATE Allegro)
# target_include_directories(${PROJECT_NAME} PRIVATE imgui)


target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)
# target_link_libraries(${PROJECT_NAME} PRIVATE implot3d::implot3d)
# target_link_libraries(${PROJECT_NAME} PRIVATE Allegro::Allegro)
# target_link_libraries(${PROJECT_NAME} PRIVATE imgui::imgui)
