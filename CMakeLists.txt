cmake_minimum_required(VERSION 3.10)

# Define the project name and the languages used
project(libmodbus VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add library source files
file(GLOB SOURCES "src/*.cpp")
add_library(libmodbus STATIC ${SOURCES})

# Include directories for the library
target_include_directories(libmodbus PUBLIC 
    "${PROJECT_SOURCE_DIR}/include"
)

# Add Google Test subdirectory
add_subdirectory(extern/googletest)

# Enable testing
enable_testing()

# Add test directory
add_subdirectory(tests/unit)
