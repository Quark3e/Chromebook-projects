cmake_minimum_required(VERSION 3.18)
project(AddressBookExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DIR_VCPKG_PACKAGE "C:/Users/berkh/Libraries/vcpkg/packages")

# Find Protobuf package

set(DIR_absl        "${DIR_VCPKG_PACKAGE}/abseil_x64-windows/share/absl")
set(DIR_protobuf    "${DIR_VCPKG_PACKAGE}/protobuf_x64-windows/share/protobuf")
set(DIR_utf8_range  "${DIR_VCPKG_PACKAGE}/utf8-range_x64-windows/share/utf8_range")


find_package(absl       CONFIG REQUIRED PATHS ${DIR_absl})
find_package(protobuf   CONFIG REQUIRED PATHS ${DIR_protobuf})
find_package(utf8_range CONFIG REQUIRED PATHS ${DIR_utf8_range})


set(CMAKE_TOOLCHAIN_FILE "C:/Users/berkh/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(DIR_ROOT "../../../../../..")
set(DIR_PROTO_LIB "..")


# Add the generated protobuf files
set(PROTO_SRCS addressbook.pb.cc)
set(PROTO_HDRS addressbook.pb.h)


# Add the executable
add_executable(${PROJECT_NAME}
    testMain.cpp
    ${PROTO_SRCS}
)


# Include directories for Protobuf
target_include_directories(${PROJECT_NAME} PRIVATE ${PROTO_HDRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_PROTO_LIB})


# Link Protobuf library
target_link_libraries(${PROJECT_NAME} PRIVATE absl::any absl::log absl::base absl::bits)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

