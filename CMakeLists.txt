cmake_minimum_required(VERSION 3.10)

# Define the project name and the languages used
project(libmodbus VERSION 1.0 LANGUAGES CXX)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add library source files
file(GLOB SOURCES "src/*.cpp")
add_library(Modbus STATIC ${SOURCES})

# Include directories for the library
target_include_directories(Modbus PUBLIC 
    "${PROJECT_SOURCE_DIR}/include"
)

# Enable testing
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Add test executable
add_executable(ModbusDataModelTest tests/ModbusDataModelTest.cpp)

# Include directories for Google Test
target_include_directories(ModbusDataModelTest PRIVATE ${googletest_SOURCE_DIR}/googletest/include)

# Link the test executable with Google Test
target_link_libraries(ModbusDataModelTest PRIVATE gtest gtest_main Modbus)

# Register the test with ctest
add_test(NAME ModbusDataModelTest COMMAND ModbusDataModelTest)
