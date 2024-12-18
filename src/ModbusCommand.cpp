#include "ModbusCommand.hpp"

bool ModbusCommand::validateQuantity(const ModbusFrame& request, uint16_t minQuantity, uint16_t maxQuantity, ModbusFrame& response)
{
    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (quantity < minQuantity || quantity > maxQuantity) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    return true;
}

bool ModbusCommand::validateAddress(const ModbusFrame& request, uint16_t maxValidAddress, ModbusFrame& response)
{
    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (startAddress + quantity < startAddress) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    if (startAddress + quantity > maxValidAddress) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    return true;
}

ModbusFrame ModbusCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameType = ModbusFrameType::NONE;
    return response;
}

ModbusFrame ReadCoilCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyOutput = (request.frameData[2] << 8) | request.frameData[3];

    if (!validateQuantity(request, 1, 2000, response)) {
        return response;
    }

    if (!validateAddress(request, ModbusDataModel::MAX_COILS, response)) {
        return response;
    }

    response.functionCode = ModbusFunctionCode::READ_COILS;
    uint8_t byteIndex = 1;
    for (uint16_t index = startAddress; index < startAddress + qtyOutput; ++index) {
        bool temp = data.readCoil(index);
        response.frameData[byteIndex] |= (temp << ((index - startAddress) % 8));
        if ((index - startAddress) % 8 == 7) {
            byteIndex++;
        }
    }
    response.frameData[0] = static_cast<uint8_t>((qtyOutput + 7) / 8);
    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame ReadDiscreteInputCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyInput = (request.frameData[2] << 8) | request.frameData[3];

    if (!validateQuantity(request, 1, 2000, response)) {
        return response;
    }

    if (!validateAddress(request, ModbusDataModel::MAX_DISCREET_INPUT, response)) {
        return response;
    }

    response.functionCode = ModbusFunctionCode::READ_DISCRETE_INPUTS;
    uint8_t byteIndex = 1;
    for (uint16_t index = startAddress; index < startAddress + qtyInput; ++index) {
        bool temp = data.readDiscreteInput(index);
        response.frameData[byteIndex] |= (temp << ((index - startAddress) % 8));
        if ((index - startAddress) % 8 == 7) {
            byteIndex++;
        }
    }
    response.frameData[0] = static_cast<uint8_t>((qtyInput + 7) / 8);
    response.frameType = ModbusFrameType::RESPONSE;
    return response;
}

ModbusFrame ReadHoldingRegisterCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyRegisters = (request.frameData[2] << 8) | request.frameData[3];

    if (!validateQuantity(request, 1, 2000, response)) {
        return response;
    }

    if (!validateAddress(request, ModbusDataModel::MAX_HOLDING_REGISTERS, response)) {
        return response;
    }

    response.functionCode = ModbusFunctionCode::READ_HOLDING_REGISTERS;
    response.frameData[0] = static_cast<uint8_t>(qtyRegisters * 2); // Number of bytes to follow

    uint8_t byteIndex = 1;
    for (uint16_t index = startAddress; index < startAddress + qtyRegisters; ++index) {
        uint16_t registerValue = data.readHoldingRegister(index);
        response.frameData[byteIndex++] = static_cast<uint8_t>(registerValue >> 8); // High byte
        response.frameData[byteIndex++] = static_cast<uint8_t>(registerValue & 0xFF); // Low byte
    }

    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame ReadInputRegisterCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyRegisters = (request.frameData[2] << 8) | request.frameData[3];

    if (!validateQuantity(request, 1, 2000, response)) {
        return response;
    }

    if (!validateAddress(request, ModbusDataModel::MAX_INPUT_REGISTERS, response)) {
        return response;
    }

    response.functionCode = ModbusFunctionCode::READ_INPUT_REGISTERS;
    response.frameData[0] = static_cast<uint8_t>(qtyRegisters * 2); // Number of bytes to follow

    uint8_t byteIndex = 1;
    for (uint16_t index = startAddress; index < startAddress + qtyRegisters; ++index) {
        uint16_t registerValue = data.readInputRegister(index);
        response.frameData[byteIndex++] = static_cast<uint8_t>(registerValue >> 8); // High byte
        response.frameData[byteIndex++] = static_cast<uint8_t>(registerValue & 0xFF); // Low byte
    }

    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame WriteCoilCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t address = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t value = (request.frameData[2] << 8) | request.frameData[3];

    if (value != 0x0000 && value != 0xFF00) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    if (!validateAddress(request, ModbusDataModel::MAX_COILS, response)) {
        return response;
    }

    data.writeCoil(address, value == 0xFF00);

    response.functionCode = ModbusFunctionCode::WRITE_SINGLE_COIL;
    response.frameData[0] = request.frameData[0];
    response.frameData[1] = request.frameData[1];
    response.frameData[2] = request.frameData[2];
    response.frameData[3] = request.frameData[3];
    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame WriteHoldingRegisterCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;
    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }

    uint16_t address = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t value = (request.frameData[2] << 8) | request.frameData[3];

    if (!validateAddress(request, ModbusDataModel::MAX_HOLDING_REGISTERS, response)) {
        return response;
    }

    data.writeHoldingRegister(address, value);

    response.functionCode = ModbusFunctionCode::WRITE_SINGLE_REGISTER;
    response.frameData[0] = request.frameData[0];
    response.frameData[1] = request.frameData[1];
    response.frameData[2] = request.frameData[2];
    response.frameData[3] = request.frameData[3];
    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}
