#include "ModbusFrame.hpp"
#include "Checksum.hpp"

std::array<uint8_t, 256> ModbusRtuFrame::serialize()
{
    std::array<uint8_t, 256> data {};
    size_t dataIndex = 0;

    data[dataIndex++] = slaveaddr;
    if (auto* responseFrame = std::get_if<ModbusResponseFrame>(&frame); responseFrame != nullptr) {
        data[dataIndex++] = static_cast<uint8_t>(responseFrame->functionCode);

        std::copy(responseFrame->frameData.begin(), responseFrame->frameData.end(), data.begin() + dataIndex);
        dataIndex += responseFrame->frameData.size();

        checksum = Checksum::calculateCRC16(data.data(), dataIndex);

        data[dataIndex++] = checksum & 0xFF;
        data[dataIndex++] = (checksum >> 8) & 0xFF;
    }

    return data;
}
void ModbusRtuFrame::deserialize(const std::array<uint8_t, 256>& data)
{
    size_t dataIndex = 0;

    slaveaddr = data[dataIndex++];
    if (auto* responseFrame = std::get_if<ModbusResponseFrame>(&frame); responseFrame != nullptr) {
        responseFrame->functionCode = static_cast<ModbusFunctionCode>(data[dataIndex++]);

        size_t frameDataSize = data.size() - dataIndex - 2;
        std::copy(data.begin() + dataIndex, data.begin() + dataIndex + frameDataSize, responseFrame->frameData.begin());
        dataIndex += frameDataSize;

        uint16_t receivedChecksum = data[dataIndex++] & 0xFF;
        receivedChecksum |= static_cast<uint16_t>(data[dataIndex++] << 8) & 0xFF00;

        uint16_t expectedChecksum = Checksum::calculateCRC16(data.data(), data.size() - 2);

        checksum = receivedChecksum;
    }
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
