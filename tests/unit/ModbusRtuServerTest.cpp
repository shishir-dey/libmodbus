#include <gtest/gtest.h>

#include "ModbusBaseServer.hpp"
#include "ModbusDataModel.hpp"

class ModbusRtuServerTest : public ::testing::Test {
protected:
ModbusRtuServer server;

void SetUp() override
{
    // Initialize test data using valid methods
    // Pattern for 0xD5 = 11010101 (LSB first): T,F,T,F,T,F,T,T
    server.data.writeCoil(0, true);
    server.data.writeCoil(1, false);
    server.data.writeCoil(2, true);
    server.data.writeCoil(3, false);
    server.data.writeCoil(4, true);
    server.data.writeCoil(5, false);
    server.data.writeCoil(6, true);
    server.data.writeCoil(7, true);

    // Initialize holding registers
    server.data.writeHoldingRegister(0, 0x1234);
    server.data.writeHoldingRegister(1, 0x5678);
    server.data.writeHoldingRegister(2, 0x9ABC);

    // Initialize input registers using new methods
    server.data.setInputRegister(0, 0xDEAD);
    server.data.setInputRegister(1, 0xBEEF);

    // Initialize discrete inputs using new methods
    server.data.setDiscreteInput(0, true);
    server.data.setDiscreteInput(1, false);
    server.data.setDiscreteInput(2, true);
}
};

TEST_F(ModbusRtuServerTest, ReadCoilsValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x01, 0x01, 0xD5, 0x90, 0x17 };

    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, ReadHoldingRegistersValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x03, 0x04, 0x12, 0x34, 0x56, 0x78, 0x81, 0x07 };

    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, ReadInputRegistersValidRequest)
{
    std::vector<uint8_t> request = { 0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x04, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x60, 0x61 };

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
    std::vector<uint8_t> request = { 0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x38, 0x0B };
    auto response = server.process(request);
    std::vector<uint8_t> expected = { 0x01, 0x02, 0x01, 0x05, 0x61, 0x8B };

    EXPECT_EQ(response, expected);
}

// New tests for enhanced functionality
TEST_F(ModbusRtuServerTest, WriteMultipleCoilsValidRequest)
{
    // Write 3 coils starting at address 10: T,F,T = 0x05 (corrected CRC)
    std::vector<uint8_t> request = { 0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x01, 0x05, 0xD7, 0x55 };
    auto response = server.process(request);
    // Expected response: echo back start address and quantity (corrected CRC)
    std::vector<uint8_t> expected = { 0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x35, 0xC8 };

    EXPECT_EQ(response, expected);

    // Verify the coils were written correctly
    EXPECT_TRUE(server.data.readCoil(10));
    EXPECT_FALSE(server.data.readCoil(11));
    EXPECT_TRUE(server.data.readCoil(12));
}

TEST_F(ModbusRtuServerTest, WriteMultipleRegistersValidRequest)
{
    // Write 2 registers starting at address 5: 0x1122, 0x3344 (corrected CRC)
    std::vector<uint8_t> request = { 0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44,
                                     0x82, 0x65 };
    auto response = server.process(request);
    // Expected response: echo back start address and quantity (corrected CRC)
    std::vector<uint8_t> expected = { 0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x51, 0xC9 };

    EXPECT_EQ(response, expected);

    // Verify the registers were written correctly
    EXPECT_EQ(server.data.readHoldingRegister(5), 0x1122);
    EXPECT_EQ(server.data.readHoldingRegister(6), 0x3344);
}

TEST_F(ModbusRtuServerTest, DiagnosticsReturnQueryData)
{
    // Diagnostics function 0x0000 (Return Query Data) with data 0x1234 (corrected CRC)
    std::vector<uint8_t> request = { 0x01, 0x08, 0x00, 0x00, 0x12, 0x34, 0xED, 0x7C };
    auto response = server.process(request);
    // Expected response: echo back the entire request data (corrected CRC)
    std::vector<uint8_t> expected = { 0x01, 0x08, 0x00, 0x00, 0x12, 0x34, 0xED, 0x7C };

    EXPECT_EQ(response, expected);
}

TEST_F(ModbusRtuServerTest, WriteMultipleCoilsInvalidByteCount)
{
    // Invalid byte count: should be 1 for 3 coils, but sending 2 (corrected CRC)
    std::vector<uint8_t> request = { 0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x02, 0x05, 0x00, 0xE5, 0x5E };
    auto response = server.process(request);

    // Should return exception response
    EXPECT_EQ(response.size(), 5); // Exception frame size
    EXPECT_EQ(response[0], 0x01); // Slave address
    EXPECT_EQ(response[1], 0x8F); // Exception function code (0x0F | 0x80)
    EXPECT_EQ(response[2], 0x03); // Exception code: ILLEGAL_DATA_VALUE
}

TEST_F(ModbusRtuServerTest, WriteMultipleRegistersInvalidByteCount)
{
    // Invalid byte count: should be 4 for 2 registers, but sending 3 (corrected CRC)
    std::vector<uint8_t> request = { 0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x03, 0x11, 0x22, 0x33, 0xC8,
                                     0x36 };
    auto response = server.process(request);

    // Should return exception response
    EXPECT_EQ(response.size(), 5); // Exception frame size
    EXPECT_EQ(response[0], 0x01); // Slave address
    EXPECT_EQ(response[1], 0x90); // Exception function code (0x10 | 0x80)
    EXPECT_EQ(response[2], 0x03); // Exception code: ILLEGAL_DATA_VALUE
}