#include "../include/ModbusDataModel.hpp"
#include <gtest/gtest.h>

class ModbusDataModelTest : public ::testing::Test {
protected:
    ModbusDataModel modbusDataModel;

    void SetUp() override
    {
        // Setup code for Modbus object if needed
    }

    void TearDown() override
    {
        // Cleanup code for Modbus object if needed
    }
};

TEST_F(ModbusDataModelTest, WriteAndReadCoil)
{
    uint8_t testIndex = 2; // Choose an index within the valid range
    CoilType originalValue = true; // Set an initial value for the coil
    modbusDataModel.writeCoil(testIndex, originalValue); // Write the initial value to the coil

    CoilType readValue = modbusDataModel.readCoil(testIndex); // Read the value back from the coil
    EXPECT_EQ(originalValue, readValue); // Compare the written value with the read value
}
