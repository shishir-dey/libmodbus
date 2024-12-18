#include "ModbusFrame.hpp"
#include "Checksum.hpp"
#include <vector>

std::vector<uint8_t> ModbusRtuFrame::serialize()
{
    std::vector<uint8_t> output;

    switch (pdu.frameType) {
    case ModbusFrameType::REQUEST:
    case ModbusFrameType::RESPONSE: {
        size_t index = 0;
        output.push_back(slaveaddr);
        output.push_back(static_cast<uint8_t>(pdu.functionCode));
        for (const auto& byte : pdu.frameData) {
            output.push_back(byte);
        }
        output.push_back(checksum & 0xFF);
        output.push_back((checksum >> 8) & 0xFF);
        return output;
    }
    case ModbusFrameType::EXCEPTION: {
        return output;
    }
    case ModbusFrameType::NONE: {
        // Handle NONE case
        break;
    }
    }
    return output;
}

void ModbusRtuFrame::deserialize(ModbusFrameType frameType, const std::vector<uint8_t>& data)
{
    pdu.frameType = frameType;
    switch (pdu.frameType) {
    case ModbusFrameType::REQUEST:
    case ModbusFrameType::RESPONSE: {
        slaveaddr = data[0];
        pdu.functionCode = static_cast<ModbusFunctionCode>(data[1]);
        pdu.frameData.clear();
        for (size_t i = 2; i < data.size() - 2; ++i) {
            pdu.frameData.push_back(data[i]);
        }
        checksum = (data[data.size() - 2 - 1] << 8) | data[data.size() - 1 - 1];
        /* verify checksum */
    }
    case ModbusFrameType::EXCEPTION: {
    }
    case ModbusFrameType::NONE: {
        // Handle NONE case
        break;
    }
    }
}

std::vector<uint8_t> ModbusAsciiFrame::serialize()
{
    std::vector<uint8_t> data;
    /* TODO */
    return data;
}

void ModbusAsciiFrame::deserialize(const std::vector<uint8_t>& data)
{
    /* TODO */
}

std::vector<uint8_t> ModbusTcpFrame::serialize()
{
    std::vector<uint8_t> data;
    /* TODO */
    return data;
}

void ModbusTcpFrame::deserialize(const std::vector<uint8_t>& data)
{
    /* TODO */
}
