cmake_minimum_required(VERSION 3.10)

# Project name
project(dataPlotter)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(DIR_ROOT "../../../../../..")
set(DIR_USEFUL "${DIR_ROOT}/teststuff/cpp/useful")

set(CMAKE_PREFIX_PATH ${DIR_ROOT}/teststuff/cpp/gui/lib_imguiwin/lib/build/)

find_package(imguiwin REQUIRED)


if(WIN32)
else()
    find_program(LSB_RELEASE_EXEC lsb_release)
    execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
        OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message("lsb_release id short: " ${LSB_RELEASE_ID_SHORT})
endif(WIN32)

# Add the executable
add_executable(${PROJECT_NAME}
    src/main.cpp
    src/global_variables.cpp
    ${DIR_USEFUL}/keyBind_handler.cpp
)

# Include directories (if needed)
target_include_directories(dataPlotter PRIVATE ${PROJECT_SOURCE_DIR}/include)


target_include_directories(${PROJECT_NAME} PRIVATE DIR_USEFUL)
target_include_directories(${PROJECT_NAME} PUBLIC imguiwin)

target_link_libraries(${PROJECT_NAME} PRIVATE imguiwin::imguiwin)