cmake_minimum_required(VERSION 3.18)
project(ElectricField_sim)

set(CMAKE_CXX_STANDARD 17)

set(DIR_ROOT "../../..")
set(DIR_USEFUL "${DIR_ROOT}/teststuff/cpp/useful")

message("DIR_USEFUL = " ${DIR_USEFUL})

find_package(imguiwin REQUIRED)



add_executable(${PROJECT_NAME}
    main.cpp
    C:/Users/berkh/Projects/Github_repo/Chromebook-projects/teststuff/cpp/useful/keyBind_handler.cpp
    # ${DIR_USEUFL}/keyBind_handler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)

target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)
