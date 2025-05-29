#include "ModbusCommand.hpp"

bool ModbusCommand::validateQuantity(
    const ModbusFrame& request,
    uint16_t minQuantity,
    uint16_t maxQuantity,
    ModbusFrame& response
)
{
    if (request.frameData.size() < 4) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return false;
    }

    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (quantity < minQuantity || quantity > maxQuantity) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return false;
    }

    return true;
}

// Function-specific quantity validation methods
bool ModbusCommand::validateReadCoilsQuantity(const ModbusFrame& request, ModbusFrame& response)
{
    return validateQuantity(request, 1, 2000, response);
}

bool ModbusCommand::validateReadRegistersQuantity(const ModbusFrame& request, ModbusFrame& response)
{
    return validateQuantity(request, 1, 125, response);
}

bool ModbusCommand::validateWriteMultipleCoilsQuantity(const ModbusFrame& request, ModbusFrame& response)
{
    return validateQuantity(request, 1, 1968, response);
}

bool ModbusCommand::validateWriteMultipleRegistersQuantity(
    const ModbusFrame& request,
    ModbusFrame& response
)
{
    return validateQuantity(request, 1, 123, response);
}

bool ModbusCommand::validateAddress(
    const ModbusFrame& request,
    uint16_t maxValidAddress,
    ModbusFrame& response
)
{
    if (request.frameData.size() < 4) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return false;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (startAddress + quantity < startAddress) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;

        return false;
    }

    // For 0-based indexing, if we have maxValidAddress items, valid addresses are 0 to (maxValidAddress-1)
    // So startAddress + quantity should not exceed maxValidAddress
    if (startAddress >= maxValidAddress || startAddress + quantity > maxValidAddress) {
        response.functionCode = request.functionCode;
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

    if (!validateReadCoilsQuantity(request, response)) {
        return response;
    }

    if (!validateAddress(request, data.getMaxCoils(), response)) {
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

    if (!validateReadCoilsQuantity(request, response)) {
        return response;
    }

    if (!validateAddress(request, data.getMaxDiscreteInputs(), response)) {
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

    if (!validateReadRegistersQuantity(request, response)) {
        return response;
    }

    if (!validateAddress(request, data.getMaxHoldingRegisters(), response)) {
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

    if (!validateReadRegistersQuantity(request, response)) {
        return response;
    }

    if (!validateAddress(request, data.getMaxInputRegisters(), response)) {
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

    // Single address validation - check if address is within valid range
    if (address >= data.getMaxCoils()) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;

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

    // Single address validation - check if address is within valid range
    if (address >= data.getMaxHoldingRegisters()) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;

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

ModbusFrame WriteMultipleCoilsCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;

    response.frameData.clear();
    response.frameData.resize(256, 0);

    // Minimum: Start Address (2) + Quantity (2) + Byte Count (1) = 5 bytes
    if (request.frameData.size() < 5) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];
    uint8_t byteCount = request.frameData[4];

    if (!validateWriteMultipleCoilsQuantity(request, response)) {
        response.functionCode = request.functionCode;

        return response;
    }

    // Check if the byte count is correct
    uint8_t expectedByteCount = (quantity + 7) / 8;

    if (byteCount != expectedByteCount || request.frameData.size() < 5 + byteCount) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    // Validate address range
    if (startAddress >= data.getMaxCoils() || startAddress + quantity > data.getMaxCoils()) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    // Extract and write coil values
    for (uint16_t i = 0; i < quantity; ++i) {
        uint8_t byteIndex = 5 + i / 8;
        uint8_t bitIndex = i % 8;
        bool coilValue = (request.frameData[byteIndex] >> bitIndex) & 0x01;
        data.writeCoil(startAddress + i, coilValue);
    }

    // Build response: echo back start address and quantity
    response.functionCode = ModbusFunctionCode::WRITE_MULTIPLE_COILS;
    response.frameData[0] = request.frameData[0]; // Start address high byte
    response.frameData[1] = request.frameData[1]; // Start address low byte
    response.frameData[2] = request.frameData[2]; // Quantity high byte
    response.frameData[3] = request.frameData[3]; // Quantity low byte
    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame WriteMultipleRegistersCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;

    response.frameData.clear();
    response.frameData.resize(256, 0);

    // Minimum: Start Address (2) + Quantity (2) + Byte Count (1) = 5 bytes
    if (request.frameData.size() < 5) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];
    uint8_t byteCount = request.frameData[4];

    if (!validateWriteMultipleRegistersQuantity(request, response)) {
        response.functionCode = request.functionCode;

        return response;
    }

    // Check if the byte count is correct (each register is 2 bytes)
    uint8_t expectedByteCount = quantity * 2;

    if (byteCount != expectedByteCount || request.frameData.size() < 5 + byteCount) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    // Validate address range
    if (startAddress >= data.getMaxHoldingRegisters() ||
        startAddress + quantity > data.getMaxHoldingRegisters()) {
        response.functionCode = request.functionCode;
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    // Extract and write register values
    for (uint16_t i = 0; i < quantity; ++i) {
        uint16_t regValue = (request.frameData[5 + i * 2] << 8) | request.frameData[5 + i * 2 + 1];
        data.writeHoldingRegister(startAddress + i, regValue);
    }

    // Build response: echo back start address and quantity
    response.functionCode = ModbusFunctionCode::WRITE_MULTIPLE_REGISTERS;
    response.frameData[0] = request.frameData[0]; // Start address high byte
    response.frameData[1] = request.frameData[1]; // Start address low byte
    response.frameData[2] = request.frameData[2]; // Quantity high byte
    response.frameData[3] = request.frameData[3]; // Quantity low byte
    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}

ModbusFrame DiagnosticsCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response;

    response.frameData.clear();
    response.frameData.resize(256, 0);

    if (request.frameData.size() < 4) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    uint16_t subFunction = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t data_field = (request.frameData[2] << 8) | request.frameData[3];

    response.functionCode = ModbusFunctionCode::DIAGNOSTICS;

    // Echo back the sub-function code
    response.frameData[0] = request.frameData[0];
    response.frameData[1] = request.frameData[1];

    switch (static_cast<ModbusDiagnosticsCode>(subFunction)) {
    case ModbusDiagnosticsCode::RETURN_QUERY_DATA:
        // Echo back the data field
        response.frameData[2] = request.frameData[2];
        response.frameData[3] = request.frameData[3];
        break;

    case ModbusDiagnosticsCode::RESTART_COMMUNICATIONS_OPTION:
        // Clear all counters and restart communications
        response.frameData[2] = 0x00;
        response.frameData[3] = 0x00;
        break;

    case ModbusDiagnosticsCode::RETURN_DIAGNOSTIC_REGISTER:
        // Return a dummy diagnostic register value
        response.frameData[2] = 0x00;
        response.frameData[3] = 0x00;
        break;

    case ModbusDiagnosticsCode::CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER:
        // Clear all diagnostic counters
        response.frameData[2] = 0x00;
        response.frameData[3] = 0x00;
        break;

    default:
        // Unsupported diagnostic function
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_FUNCTION;
        response.frameType = ModbusFrameType::EXCEPTION;

        return response;
    }

    response.frameType = ModbusFrameType::RESPONSE;

    return response;
}