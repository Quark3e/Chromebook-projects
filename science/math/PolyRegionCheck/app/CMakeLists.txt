cmake_minimum_required(VERSION 3.18)
project(libTest_PolyRegionCheck)


set(CMAKE_CXX_STANDARD 17)

set(DIR_ROOT    "../../../..")
set(DIR_USEFUL  "${DIR_ROOT}/teststuff/cpp/useful")

set(
    CMAKE_PREFIX_PATH
    ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/
    ../lib/build/
)

find_package(imguiwin REQUIRED)
find_package(PolyRegionCheck REQUIRED)

add_executable(${PROJECT_NAME}
    main.cpp
    ${DIR_USEFUL}/keyBind_handler.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)
target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_include_directories(${PROJECT_NAME} PUBLIC PolyRegionCheck)

target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE PolyRegionCheck::PolyRegionCheck)


