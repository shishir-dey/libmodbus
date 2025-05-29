#include "Checksum.hpp"
#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::vector<uint8_t> ModbusRtuServer::process(const std::vector<uint8_t>& requestData)
{
    // Minimum frame size: slave address (1) + function code (1) + CRC (2) = 4 bytes
    if (requestData.size() < 4) {
        return {}; // Invalid frame size
    }

    ModbusRtuFrame request, response;
    request.deserialize(ModbusFrameType::REQUEST, requestData);

    // Validate CRC
    std::vector<uint8_t> dataWithoutCrc(requestData.begin(), requestData.end() - 2);
    uint16_t expectedCrc = Checksum::calculateCRC16(dataWithoutCrc.data(), dataWithoutCrc.size());

    if (expectedCrc != request.checksum) {
        return {}; // Invalid CRC - no response
    }

    // Check slave address - only respond to address 1 or broadcast (0)
    // For simplicity, we'll assume this server has address 1
    const uint8_t SERVER_ADDRESS = 1;

    if (request.slaveaddr != SERVER_ADDRESS && request.slaveaddr != 0) {
        return {}; // Wrong slave address - no response
    }

    // Don't respond to broadcast messages (address 0)
    if (request.slaveaddr == 0) {
        return {}; // Broadcast - no response expected
    }

    // Check if function code is supported
    auto commandIter = commands.find(request.pdu.functionCode);

    if (commandIter == commands.end()) {
        // Unknown function code - return exception
        response.slaveaddr = request.slaveaddr;
        response.pdu.frameType = ModbusFrameType::EXCEPTION;
        response.pdu.functionCode = request.pdu.functionCode;
        response.pdu.exceptionCode = ModbusExceptionCode::ILLEGAL_FUNCTION;
        response.pdu.frameData.clear();

        std::vector<uint8_t> responseData = response.serialize();

        return responseData;
    }

    // Process the command
    response.slaveaddr = request.slaveaddr;
    response.pdu = commandIter->second->execute(data, request.pdu);

    // Calculate CRC for response
    std::vector<uint8_t> responseData = response.serialize();

    return responseData;
}