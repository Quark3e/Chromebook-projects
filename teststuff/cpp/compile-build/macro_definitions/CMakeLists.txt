cmake_minimum_required(VERSION 3.10)

# Project name
project(MacroDefinitionsExample)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)



# Add the executable
add_executable(MacroDefinitionsExample main.cpp)

# Optionally define macros
# Uncomment the following line to define _TESTMACRO2
target_compile_definitions(MacroDefinitionsExample PRIVATE
    _TESTMACRO2="Redefined Another internal string"

)

