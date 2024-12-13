add_library(libHexclawDataThread hexclaw_networkDataThread.cpp)

if(WIN32)
find_library(JPEG_LIB
    jpeg
    PATHS "C:\\Users\\berkh\\Libraries\\imgui\\examples\\example_allegro5\\vcpkg\\installed\\x64-windows\\lib"
    REQUIRED
)
else()
find_library(JPEG_LIB
    jpeg
    HINTS "${CMAKE_PREFIX_PATH}/arm-linux-gnueabihf" "${CMAKE_PREFIX_PATH}/x86_64-linux-gnu/"
    REQUIRED
)
endif(WIN32)

target_link_libraries(libHexclawDataThread "${JPEG_LIB}")
