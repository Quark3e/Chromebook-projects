cmake_minimum_required(VERSION 3.18)
project(terminalMenu)

if(WIN32)
# find_package(pdcurses CONFIG REQUIRED)
# find_package(unofficial-pdcurses CONFIG REQUIRED)

set(DIR_PDCURSES "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows")

# find_path(DIR_PDCURSES
#     curses.h
#     HINTS "${VCPKG_ROOT}\\installed\\x64-windows\\"
# )
find_library(pdcurses
    pdcurses
    PATHS "C:/Users/berkh/Libraries/vcpkg/installed/x64-windows/lib/"
    REQUIRED
)
message("pdcurses lib: " = ${pdcurses})

endif(WIN32)

set(CMAKE_TOOLCHAIN_FILE "C:\\Users\\berkh\\Libraries\\vcpkg\\scripts\\buildsystems\\vcpkg.cmake")
set(CMAKE_CXX_STANDARD 17)
set(DIR_HC_USEFUL "${PROJECT_SOURCE_DIR}/../")

include_directories(${DIR_HC_USEFUL})
include_directories(${DIR_PDCURSES}/include/)
link_directories(${DIR_HC_USEFUL})

add_executable(${PROJECT_NAME}
    main.cpp
    ${DIR_HC_USEFUL}/createTable.cpp
    ${DIR_HC_USEFUL}/terminalMenu.cpp
)

if(WIN32)
# target_link_libraries(${PROJECT_NAME}
#     PRIVATE
#     pdcurses:pdcurses
# )
# target_link_libraries(${PROJECT_NAME} PRIVATE pdcurses::pdcurses)

target_include_directories(${PROJECT_NAME} PRIVATE ${DIR_PDCURSES}/include/)
target_link_libraries(${PROJECT_NAME} ${pdcurses})
else()
target_link_libraries(${PROJECT_NAME} ncurses)
endif(WIN32)

# # g++ -I../ main.cpp ../terminalMenu.cpp ../createTable.cpp -o test.exe -lncurses -DNCURSES_STATIC 


# gcc -I/mingw64/include/ncurses -o PROGRAMNAME main.c -lncurses -L/mingw64/bin -static
# g++ -I/mingw64/include/ncurses -I/Users\berkh/Projects/Github_repo/Chromebook-projects/teststuff/cpp/useful/ -o PROGRAMNAME main.cpp ../createTable.cpp ../terminalMenu.cpp -L/mingw64/bin -lncurses  -static


# cmake_minimum_required(VERSION 3.18)
# project(terminalMenu)

# set(CMAKE_CXX_STANDARD 17)

# include_directories(${CMAKE_SOURCE_DIR}/../)
# include_directories("C:/msys64/mingw64/include/")
# link_directories("C:/msys64/")
# link_directories("C:/msys64/ucrt64/lib/")
# link_directories("C:/msys64/mingw64/lib/")


# add_executable(${PROJECT_NAME}
#     main.cpp
# )

# target_link_libraries(${PROJECT_NAME}
#     ncurses
# )

# target_compile_definitions(${PROJECT_NAME}
#     PUBLIC
#     NCURSES_STATIC
# )

# # g++ -I../ main.cpp ../terminalMenu.cpp ../createTable.cpp -o test.exe -lncurses -DNCURSES_STATIC 