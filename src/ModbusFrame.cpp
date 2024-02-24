#include "ModbusFrame.hpp"
#include "Checksum.hpp"

std::array<uint8_t, 256> ModbusRtuFrame::serialize()
{
    std::array<uint8_t, 256> output {};

    switch (pdu.frameType) {
    case ModbusFrameType::REQUEST:
        pdu.validDataCount = 4;
    case ModbusFrameType::RESPONSE: {
        size_t index = 0;
        output[index++] = slaveaddr;
        output[index++] = static_cast<uint8_t>(pdu.functionCode);
        for (size_t i = 0; i < pdu.validDataCount; ++i) {
            output[index++] = pdu.frameData[i];
        }
        output[index++] = checksum & 0xFF;
        output[index++] = (checksum >> 8) & 0xFF;
        return output;
    }
    case ModbusFrameType::EXCEPTION: {
        return output;
    }
    }
    return output;
}
void ModbusRtuFrame::deserialize(ModbusFrameType frameType, const std::array<uint8_t, 256>& data)
{
    reset();
    pdu.frameType = frameType;
    switch (pdu.frameType) {
    case ModbusFrameType::REQUEST:
    case ModbusFrameType::RESPONSE: {
        slaveaddr = data[0];
        pdu.functionCode = static_cast<ModbusFunctionCode>(data[1]);
        for (size_t i = 2; i < data.size() - 2; ++i) {
            pdu.frameData[i - 2] = data[i];
        }
        checksum = (data[data.size() - 2 - 1] << 8) | data[data.size() - 1 - 1];
        /* verify checksum */
    }
    case ModbusFrameType::EXCEPTION: {
    }
    }
}

void ModbusRtuFrame::reset()
{
    slaveaddr = 0;
    pdu.frameType = ModbusFrameType::NONE;
    pdu.functionCode = ModbusFunctionCode::NONE;
    pdu.frameData.fill(0);
    checksum = 0;
}

std::array<uint8_t, 256> ModbusAsciiFrame::serialize()
{
    std::array<uint8_t, 256> data {};
    /* TODO */
    return data;
}

void ModbusAsciiFrame::deserialize(const std::array<uint8_t, 256>& data)
{
    /* TODO */
}

std::array<uint8_t, 256> ModbusTcpFrame::serialize()
{
    std::array<uint8_t, 256> data {};
    /* TODO */
    return data;
}

void ModbusTcpFrame::deserialize(const std::array<uint8_t, 256>& data)
{
    /* TODO */
}
