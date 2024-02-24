#include "../include/ModbusDataModel.hpp"
#include <gtest/gtest.h>

class ModbusDataModelTest : public ::testing::Test {
protected:
    ModbusDataModel modbusDataModel;

    void SetUp() override
    {
    }

    void TearDown() override
    {
    }
};

TEST_F(ModbusDataModelTest, WriteAndReadCoil)
{
    uint8_t testIndex = 2;
    CoilType originalValue = true;
    modbusDataModel.writeCoil(testIndex, originalValue);

    CoilType readValue = modbusDataModel.readCoil(testIndex);
    EXPECT_EQ(originalValue, readValue);
}
