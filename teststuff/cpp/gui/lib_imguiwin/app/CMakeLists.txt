cmake_minimum_required(VERSION 3.18)
project(lib_imguiwin_test)

set(CMAKE_CXX_STANDARD 17)


find_package(imguiwin REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)

# cmake -S app -B app/build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="C:\Users\berkh\Projects\Github_repo\Chromebook-projects\teststuff\cpp\gui\lib_imguiwin\lib\build\Debug"
# cmake --build build
