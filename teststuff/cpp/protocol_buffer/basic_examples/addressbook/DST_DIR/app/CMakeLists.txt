cmake_minimum_required(VERSION 3.18)
project(AddressBookExample_APP)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(DIR_VCPKG_PACKAGE "C:/Users/berkh/Libraries/vcpkg/packages")

set(DIR_PROTO_LIB "../lib/build")

find_package(AddressBookExample_lib CONFIG REQUIRED PATHS "${DIR_PROTO_LIB}")

# Add the executables
add_executable(READ_${PROJECT_NAME}
    message_read.cpp
)
add_executable(WRITE_${PROJECT_NAME}
    message_write.cpp
)


target_include_directories( READ_${PROJECT_NAME} PRIVATE AddressBookExample_lib)
target_include_directories(WRITE_${PROJECT_NAME} PRIVATE AddressBookExample_lib)
target_link_libraries( READ_${PROJECT_NAME} PRIVATE AddressBookExample_lib::AddressBookExample_lib)
target_link_libraries(WRITE_${PROJECT_NAME} PRIVATE AddressBookExample_lib::AddressBookExample_lib)
