cmake_minimum_required(VERSION 3.18)
project(AddressBookExample_test)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DIR_VCPKG_PACKAGE "C:/Users/berkh/Libraries/vcpkg/packages")

set(DIR_PROTO_LIB "../lib/build")

find_package(AddressBookExample_lib CONFIG REQUIRED PATHS "${DIR_PROTO_LIB}")

# Add the executable
add_executable(${PROJECT_NAME}
    testMain.cpp
    ${PROTO_SRCS}
)

target_include_directories(${PROJECT_NAME} PRIVATE AddressBookExample_lib)
target_link_libraries(${PROJECT_NAME} PRIVATE AddressBookExample_lib::AddressBookExample_lib)
