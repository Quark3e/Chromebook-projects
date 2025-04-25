cmake_minimum_required(VERSION 3.10)

# Project name
project(dataPlotter)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(WIN32)

else()
find_program(LSB_RELEASE_EXEC lsb_release)
execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
    OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message("lsb_release id short: " LSB_RELEASE_ID_SHORT)
endif(WIN32)

# Add the executable
add_executable(dataPlotter main.cpp)

# Include directories (if needed)
target_include_directories(dataPlotter PRIVATE ${PROJECT_SOURCE_DIR}/include)

# Link libraries (if needed)
# target_link_libraries(dataPlotter <library_name>)