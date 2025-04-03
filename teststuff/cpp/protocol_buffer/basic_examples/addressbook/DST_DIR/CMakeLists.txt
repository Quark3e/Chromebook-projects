cmake_minimum_required(VERSION 3.18)
project(AddressBookExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Protobuf package
# find_package(absl CONFIG REQUIRED PATHS "C:/Users/berkh/Libraries/vcpkg/packages/abseil_x64-windows/share/absl")
find_package(
    protobuf
    CONFIG REQUIRED
    PATHS "C:/Users/berkh/Libraries/vcpkg/packages/protobuf_x64-windows/share/protobuf"
)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/berkh/Libraries/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(DIR_ROOT "../../../../../..")
set(DIR_PROTO_LIB "..")



# Add the generated protobuf files


set(PROTO_SRCS addressbook.pb.cc)
set(PROTO_HDRS addressbook.pb.h)

# # Add the source files
# set(SOURCE_FILES 
#     message_write.cpp
#     ${PROTO_SRCS}
# )

# Add the executable
add_executable(${PROJECT_NAME}
    message_write.cpp
    ${PROTO_SRCS}
)


# Include directories for Protobuf
# target_include_directories(${PROJECT_NAME} PRIVATE ${Protobuf_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_PROTO_LIB})


# Link Protobuf library
# target_link_libraries(${PROJECT_NAME} PRIVATE ${Protobuf_LIBRARIES})
# target_link_libraries(${PROJECT_NAME} PRIVATE absl::any absl::log absl::base absl::bits)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)

