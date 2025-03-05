cmake_minimum_required(VERSION 3.18)
project(ElectricField_sim)


set(CMAKE_CXX_STANDARD 17)

set(DIR_ROOT "../../..")
set(DIR_USEFUL "${DIR_ROOT}/teststuff/cpp/useful")

set(CMAKE_PREFIX_PATH ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/)
# add_subdirectory(${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/ binary_dir)

message("PREFIXPATH = " ${CMAKE_PREFIX_PATH})
message("DIR_USEFUL = " ${DIR_USEFUL})

find_package(imguiwin REQUIRED)



add_executable(${PROJECT_NAME}
    main.cpp
    ${DIR_USEFUL}/keyBind_handler.cpp
    # ${DIR_USEUFL}/keyBind_handler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)

target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)

# cmake -S app -B app/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Users\berkh\Projects\Github_repo\Chromebook-projects\teststuff\cpp\gui\lib_imguiwin\lib\build\Debug"