cmake_minimum_required(VERSION 3.19)
project(scienceUnit)

set(CMAKE_CXX_STANDARD 17)


set(REPO_ROOT "../../../..")
set(DIR_HC_USEFUL "${REPO_ROOT}/teststuff/cpp/useful")


add_library(scienceUnit STATIC
    scienceUnit.cpp
)
add_library(scienceUnit::scienceUnit ALIAS scienceUnit)

target_include_directories(scienceUnit PUBLIC ${DIR_HC_USEFUL})
# target_link_directories(scienceUnit ${DIR_IMGUI}/imgui/backends)

target_include_directories(scienceUnit PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)


## Packaging rules
include(GNUInstallDirs)

install(TARGETS scienceUnit
        EXPORT scienceUnit_targets
        INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export( EXPORT scienceUnit_targets
        NAMESPACE scienceUnit::
        FILE "${CMAKE_INSTALL_LIBDIR}/cmake/scienceUnit/scienceUnit-config.cmake")




# cmake --build build
