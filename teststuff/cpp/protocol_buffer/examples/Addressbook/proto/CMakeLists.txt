set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DIR_VCPKG_PACKAGE "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/share")

set(DIR_absl        "${DIR_VCPKG_PACKAGE}/absl")
set(DIR_protobuf    "${DIR_VCPKG_PACKAGE}/protobuf")
set(DIR_utf8_range  "${DIR_VCPKG_PACKAGE}/utf8_range")


find_package(absl       CONFIG REQUIRED PATHS ${DIR_absl})
find_package(protobuf   CONFIG REQUIRED PATHS ${DIR_protobuf})
find_package(utf8_range CONFIG REQUIRED PATHS ${DIR_utf8_range})


set(PROTO_FILES addressbook.proto)

add_library(myProtoLib STATIC ${PROTO_FILES} )
target_include_directories(myProtoLib INTERFACE ${CMAKE_CURRENT_BINARY_DIR})


# I thought that i had to link the library but when i do link it, the program builds without errors or warnings but won't start at all, like it won't even
# reach `int main()`. Nothing happens, no error, nothing. Adding an `std::cout<<"test"<<std::endl; return 0;` right at the start of `int main()` gives nothing.
# target_link_libraries(myProtoLib PUBLIC protobuf::libprotobuf)

protobuf_generate(TARGET myProtoLib LANGUAGE cpp)