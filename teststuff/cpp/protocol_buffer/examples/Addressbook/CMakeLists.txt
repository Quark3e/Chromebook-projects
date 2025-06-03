cmake_minimum_required(VERSION 3.18)
project(AddressBookExample_APP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(proto)


add_executable( READ_${PROJECT_NAME}
    src/message_read.cpp
)
add_executable(WRITE_${PROJECT_NAME}
    src/message_write.cpp
)

target_compile_definitions( READ_${PROJECT_NAME} PRIVATE cxx_std_20)
target_compile_definitions(WRITE_${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries( READ_${PROJECT_NAME} PRIVATE myProtoLib)
target_link_libraries(WRITE_${PROJECT_NAME} PRIVATE myProtoLib)

    