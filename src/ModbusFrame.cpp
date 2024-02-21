#include "ModbusFrame.hpp"
#include "Checksum.hpp"

std::array<uint8_t, 256> ModbusFrame::serialize()
{
}

void ModbusFrame::deserialize(const std::array<uint8_t, 256>& data)
{
}

std::array<uint8_t, 256> ModbusRtuFrame::serialize()
{
    std::array<uint8_t, 256> data {};
    size_t dataIndex = 0;

    data[dataIndex++] = slaveaddr;
    data[dataIndex++] = static_cast<uint8_t>(functionCode);

    std::copy(frameData.begin(), frameData.end(), data.begin() + dataIndex);
    dataIndex += frameData.size();

    checksum = Checksum::calculateCRC16(data.data(), dataIndex);

    data[dataIndex++] = checksum & 0xFF;
    data[dataIndex++] = (checksum >> 8) & 0xFF;

    return data;
}
void ModbusRtuFrame::deserialize(const std::array<uint8_t, 256>& data)
{
    size_t dataIndex = 0;

    slaveaddr = data[dataIndex++];
    functionCode = static_cast<ModbusFunctionCode>(data[dataIndex++]);

    size_t frameDataSize = data.size() - dataIndex - 2;
    std::copy(data.begin() + dataIndex, data.begin() + dataIndex + frameDataSize, frameData.begin());
    dataIndex += frameDataSize;

    uint16_t receivedChecksum = data[dataIndex++] & 0xFF;
    receivedChecksum |= static_cast<uint16_t>(data[dataIndex++] << 8) & 0xFF00;

    uint16_t expectedChecksum = Checksum::calculateCRC16(data.data(), data.size() - 2);

    checksum = receivedChecksum;
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
