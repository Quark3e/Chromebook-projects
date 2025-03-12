cmake_minimum_required(VERSION 3.18)
project(WavePropagation_sim)


set(CMAKE_CXX_STANDARD 17)

set(DIR_ROOT    "../../..")
set(DIR_USEFUL  "${DIR_ROOT}/teststuff/cpp/useful")

set(CMAKE_PREFIX_PATH ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/)

find_package(imguiwin REQUIRED)

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/processes_calc.cpp
    src/processes_gui.cpp
    src/variables_data.cpp
    src/variables_gui.cpp
    src/functions_data.cpp
    src/functions_gui.cpp
    ${DIR_USEFUL}/keyBind_handler.cpp
    # ${DIR_USEFUL}/diy_dictionary.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)

target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)


target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
