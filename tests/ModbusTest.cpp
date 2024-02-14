#include "../src/Modbus.h"
#include <gtest/gtest.h>

class ModbusTest : public ::testing::Test {
protected:
    Modbus modbus;

    void SetUp() override
    {
        // Setup code for Modbus object if needed
    }

    void TearDown() override
    {
        // Cleanup code for Modbus object if needed
    }
};

TEST_F(ModbusTest, WriteAndReadCoil)
{
    uint8_t testIndex = 2; // Choose an index within the valid range
    CoilType originalValue = true; // Set an initial value for the coil
    modbus.writeCoil(testIndex, originalValue); // Write the initial value to the coil

    CoilType readValue = modbus.readCoil(testIndex); // Read the value back from the coil
    EXPECT_EQ(originalValue, readValue); // Compare the written value with the read value
}
