cmake_minimum_required(VERSION 3.18)
project(libeigen_test)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_FIND_DEBUG_MODE 1)
set(BUILD_MODE Debug)


set(REPO_ROOT "../../../../..")
set(DIR_CPP_USEFUL "${REPO_ROOT}/teststuff/cpp")
set(DIR_HC_USEFUL "${DIR_CPP_USEFUL}/useful")

set(CMAKE_PREFIX_PATH ${REPO_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/)

set(EIGEN3_INCLUDE_DIR "C:/msys64/mingw64/include/eigen3")

find_package(imguiwin REQUIRED)
# find_package(Eigen3 REQUIRED PATHS "C:/msys64/mingw64/share/cmake/Eigen3" NO_DEFAULT_PATH)

message("Eigen3_INCLUDE_DIR: ${EIGEN3_INCLUDE_DIR}")
# message("Eigen3 package dir: ${Eigen3_DIR}")


add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ${EIGEN3_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)
target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)
target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)


target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/include)


