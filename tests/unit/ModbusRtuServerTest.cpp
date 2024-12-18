#include "ModbusBaseServer.hpp"
#include "ModbusDataModel.hpp"
#include <gtest/gtest.h>

class ModbusRtuServerTest : public ::testing::Test {
protected:
    ModbusRtuServer server;

    void SetUp() override
    {
        // Initialize test data using valid methods
        server.data.writeCoil(0, true);
        server.data.writeCoil(1, false);
        server.data.writeCoil(2, true);
        server.data.writeCoil(3, false);
        server.data.writeCoil(4, true);
        server.data.writeCoil(5, false);
        server.data.writeCoil(6, true);
        server.data.writeCoil(7, false);

        // Initialize holding registers
        server.data.writeHoldingRegister(0, 0x1234);
        server.data.writeHoldingRegister(1, 0x5678);
        server.data.writeHoldingRegister(2, 0x9ABC);

        // Directly initialize input registers if needed
        server.data.inputRegisters[0] = 0xDEAD;
        server.data.inputRegisters[1] = 0xBEEF;
    }
};

TEST_F(ModbusRtuServerTest, ReadCoilsValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x01, 0x01, 0xA5, 0x4D, 0x3B };
    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, ReadHoldingRegistersValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x03, 0x04, 0x12, 0x34, 0x56, 0x78, 0xB0, 0x1A };
    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, ReadInputRegistersValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x04, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0xA5, 0xC4 };
    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, InvalidSlaveAddress)
{
    std::vector<uint8_t> request = { 0x02, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC };
    auto response = server.process(request);
    // Server should not respond to requests with wrong slave address
    EXPECT_TRUE(response.empty());
}

TEST_F(ModbusRtuServerTest, InvalidCRC)
{
    std::vector<uint8_t> request = { 0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00 };
    auto response = server.process(request);
    // Server should not respond to requests with invalid CRC
    EXPECT_TRUE(response.empty());
}

TEST_F(ModbusRtuServerTest, ReadDiscreteInputsValidRequest)
{
    // Set some discrete inputs first
    server.data.discreteInputs[0] = true;
    server.data.discreteInputs[1] = false;
    server.data.discreteInputs[2] = true;

    std::vector<uint8_t> request = { 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x38, 0x0B };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x02, 0x01, 0x05, 0x88, 0xFA };
    EXPECT_EQ(response, expected);
}