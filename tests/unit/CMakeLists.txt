# Add test executable
add_executable(modbus_tests
    ModbusRtuServerTest.cpp
)

# Link test executable with Google Test and libmodbus
target_link_libraries(modbus_tests
    gtest_main
    libmodbus
)

# Add test to CTest
add_test(NAME modbus_tests COMMAND modbus_tests)

target_include_directories(modbus_tests PRIVATE ${PROJECT_SOURCE_DIR}/include)