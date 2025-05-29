#include <vector>

#include "Checksum.hpp"
#include "ModbusFrame.hpp"

std::vector<uint8_t> ModbusRtuFrame::serialize()
{
    std::vector<uint8_t> output;

    switch (pdu.frameType) {
    case ModbusFrameType::REQUEST:
    case ModbusFrameType::RESPONSE: {
        // Build frame without CRC first
        output.push_back(slaveaddr);
        output.push_back(static_cast<uint8_t>(pdu.functionCode));

        // Add frame data - but only the actual data length needed
        size_t dataLength = 0;

        if (!pdu.frameData.empty()) {
            // For responses, find the actual data length based on the first byte (byte count)
            if (pdu.frameType == ModbusFrameType::RESPONSE) {
                switch (pdu.functionCode) {
                case ModbusFunctionCode::READ_COILS:
                case ModbusFunctionCode::READ_DISCRETE_INPUTS:
                case ModbusFunctionCode::READ_HOLDING_REGISTERS:
                case ModbusFunctionCode::READ_INPUT_REGISTERS:
                    // First byte is byte count, so data length is byte count + 1
                    dataLength = pdu.frameData[0] + 1;
                    break;

                case ModbusFunctionCode::WRITE_SINGLE_COIL:
                case ModbusFunctionCode::WRITE_SINGLE_REGISTER:
                case ModbusFunctionCode::WRITE_MULTIPLE_COILS:
                case ModbusFunctionCode::WRITE_MULTIPLE_REGISTERS:
                    // Echo back the 4-byte request data (start address + quantity)
                    dataLength = 4;
                    break;

                case ModbusFunctionCode::DIAGNOSTICS:
                    // Diagnostics always returns 4 bytes (sub-function + data)
                    dataLength = 4;
                    break;

                default:
                    dataLength = pdu.frameData.size();
                    break;
                }
            } else {
                // For requests, use full data length
                dataLength = pdu.frameData.size();
            }
        }

        // Add the actual data bytes
        for (size_t i = 0; i < dataLength && i < pdu.frameData.size(); ++i) {
            output.push_back(pdu.frameData[i]);
        }

        // Calculate CRC on the frame data (excluding CRC itself)
        checksum = Checksum::calculateCRC16(output.data(), output.size());

        // Append CRC (low byte first, then high byte)
        output.push_back(checksum & 0xFF);
        output.push_back((checksum >> 8) & 0xFF);

        return output;
    }

    case ModbusFrameType::EXCEPTION: {
        // Build exception frame
        output.push_back(slaveaddr);
        output.push_back(static_cast<uint8_t>(pdu.functionCode) | 0x80); // Set MSB for exception
        output.push_back(static_cast<uint8_t>(pdu.exceptionCode));

        // Calculate CRC
        checksum = Checksum::calculateCRC16(output.data(), output.size());

        // Append CRC (low byte first, then high byte)
        output.push_back(checksum & 0xFF);
        output.push_back((checksum >> 8) & 0xFF);

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
    if (data.size() < 4) {
        return; // Invalid frame
    }

    pdu.frameType = frameType;
    slaveaddr = data[0];

    // Check if it's an exception response
    if ((data[1] & 0x80) != 0) {
        pdu.frameType = ModbusFrameType::EXCEPTION;
        pdu.functionCode = static_cast<ModbusFunctionCode>(data[1] & 0x7F);

        if (data.size() >= 3) {
            pdu.exceptionCode = static_cast<ModbusExceptionCode>(data[2]);
        }

        pdu.frameData.clear();
    } else {
        pdu.functionCode = static_cast<ModbusFunctionCode>(data[1]);
        pdu.frameData.clear();

        // Extract frame data (everything except slave address, function code, and CRC)
        for (size_t i = 2; i < data.size() - 2; ++i) {
            pdu.frameData.push_back(data[i]);
        }
    }

    // Extract CRC (low byte first, then high byte)
    if (data.size() >= 2) {
        checksum = data[data.size() - 2] | (data[data.size() - 1] << 8);
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