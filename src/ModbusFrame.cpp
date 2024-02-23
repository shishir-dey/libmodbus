#include "ModbusFrame.hpp"
#include "Checksum.hpp"

std::array<uint8_t, 256> ModbusRtuFrame::serialize()
{
    std::array<uint8_t, 256> data {};
    size_t dataIndex = 0;

    data[dataIndex++] = slaveaddr;

    std::variant<ModbusResponseFrame, ModbusExceptionFrame> responseVariant = pdu;

    switch (responseVariant.index()) {
    case 0: /* ModbusResponseFrame */
    {
        ModbusResponseFrame* responseFrame = std::get_if<ModbusResponseFrame>(&responseVariant);
        data[dataIndex++] = static_cast<uint8_t>(responseFrame->functionCode);

        std::copy(responseFrame->frameData.begin(), responseFrame->frameData.end(), data.begin() + dataIndex);
        dataIndex += responseFrame->frameData.size();

        checksum = Checksum::calculateCRC16(data.data(), dataIndex);

        data[dataIndex++] = checksum & 0xFF;
        data[dataIndex++] = (checksum >> 8) & 0xFF;
        break;
    }
    case 1: /* ModbusExceptionFrame */
    {
        // ModbusExceptionFrame* exceptionFrame = std::get_if<ModbusExceptionFrame>(&responseVariant);
        break;
    }
    }

    return data;
}
void ModbusRtuFrame::deserialize(const std::array<uint8_t, 256>& data)
{
    /* assume incoming data is a valid request/response frame */
    slaveaddr = data[0];

    ModbusRequestFrame requestFrame;
    requestFrame.functionCode = static_cast<ModbusFunctionCode>(data[1]);
    for (size_t i = 2; i < data.size() - 2; ++i) {
        requestFrame.frameData[i - 2] = data[i];
    }
    pdu = requestFrame;
    checksum = (data[data.size() - 2 - 1] << 8) | data[data.size() - 1 - 1];

    /* verify checksum */
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
