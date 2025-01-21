cmake_minimum_required(VERSION 3.18)
project(terminalMenu)

set(CMAKE_CXX_STANDARD 17)
set(DIR_HC_USEFUL "${PROJECT_SOURCE_DIR}/../")

include_directories(${DIR_HC_USEFUL})
include_directories("C:/msys64/mingw64/include/")
link_directories(${DIR_HC_USEFUL})
link_directories("C:/msys64/mingw64/lib/")

add_executable(${PROJECT_NAME}
    main.cpp
    ${DIR_HC_USEFUL}/createTable.cpp
    ${DIR_HC_USEFUL}/terminalMenu.cpp
)

target_link_libraries(${PROJECT_NAME}
    # menu
    ncurses
)
target_compile_options(${PROJECT_NAME}
    PUBLIC
    # -lmenu
    -lncurses
)