cmake_minimum_required(VERSION 3.19)
project(PolyRegionCheck)

set(CMAKE_CXX_STANDARD 17)

if(WIN32)

else()

endif(WIN32)

set(REPO_ROOT "../../../..")
set(DIR_HC_USEFUL   "${REPO_ROOT}/teststuff/cpp/useful")
set(DIR_HC_SCIENCE  "${REPO_ROOT}/science")

#message(DIR_HC_USEFUL=${DIR_HC_USEFUL})

add_library(${PROJECT_NAME}
    PolyRegionCheck.cpp #not utilised as of now. Included here just to have it here for the future.
)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME} PUBLIC ${DIR_HC_USEFUL})
target_include_directories(${PROJECT_NAME} PUBLIC ${REPO_ROOT}/science/math/line_intersection/line_intersection_2D)
target_include_directories(${PROJECT_NAME} PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)


## Packaging rules
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}_targets
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

export( EXPORT ${PROJECT_NAME}_targets
        NAMESPACE ${PROJECT_NAME}::
        FILE "${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake")
        
