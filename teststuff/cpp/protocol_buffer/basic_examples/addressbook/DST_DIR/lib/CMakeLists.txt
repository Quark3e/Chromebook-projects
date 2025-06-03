cmake_minimum_required(VERSION 3.18)
project(AddressBookExample_lib)

set(CMAKE_CXX_STANDARD 17)
set(DIR_VCPKG_PACKAGE "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share")

# Find Protobuf package
set(DIR_absl        "${DIR_VCPKG_PACKAGE}/absl")
set(DIR_protobuf    "${DIR_VCPKG_PACKAGE}/protobuf")
set(DIR_utf8_range  "${DIR_VCPKG_PACKAGE}/utf8_range")


find_package(absl       CONFIG REQUIRED PATHS ${DIR_absl})
find_package(protobuf   CONFIG REQUIRED PATHS ${DIR_protobuf})
find_package(utf8_range CONFIG REQUIRED PATHS ${DIR_utf8_range})

set(CMAKE_TOOLCHAIN_FILE "C:/Users/berkh/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")

add_library(${PROJECT_NAME} STATIC addressbook.pb.cc)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR})
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)

# Include directories for Protobuf
target_include_directories(${PROJECT_NAME} PUBLIC ${PROTO_HDRS})
target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_PROTO_LIB})


# I thought that i had to link the library but when i do link it, the program builds without errors or warnings but won't start at all, like it won't even
# reach `int main()`. Nothing happens, no error, nothing. Adding an `std::cout<<"test"<<std::endl; return 0;` right at the start of `int main()` gives nothing.
# # Link Protobuf library
# target_link_libraries(${PROJECT_NAME} PRIVATE absl::any absl::log absl::base absl::bits)
# target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)
target_link_libraries(${PROJECT_NAME} PRIVATE ${absl})
target_link_libraries(${PROJECT_NAME} PRIVATE ${protobuf})


include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
        EXPORT ${PROJECT_NAME}_targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export( EXPORT ${PROJECT_NAME}_targets
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}-config.cmake")