#include "ModbusCommand.hpp"

ModbusFrame ModbusCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
}

bool ModbusCommand::validateQuantity(const ModbusFrame& request, uint16_t minQuantity, uint16_t maxQuantity, ModbusFrame& response)
{
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (quantity < 1 || quantity > 0x7D0) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    return true;
}

bool ModbusCommand::validateAddress(const ModbusFrame& request, uint16_t maxValidAddress, ModbusFrame& response)
{
    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t quantity = (request.frameData[2] << 8) | request.frameData[3];

    if (startAddress >= 0 && startAddress + quantity > maxValidAddress) {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;
        return false;
    }

    return true;
}

ModbusFrame ReadCoilCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    /**
     * Procedure for Reading Coils
     *
     * 1) Check if the Quantity of Outputs is within the valid range (1 to 2000).
     *    - If valid, proceed to step 2.
     *    - If not valid, return ExceptionCode ILLEGAL_DATA_VALUE (03).
     *
     * 2) Verify the validity of the Starting Address and the sum of Starting Address + Quantity of Outputs.
     *    - If both conditions are met, proceed to step 3.
     *    - If not met, return ExceptionCode ILLEGAL_DATA_ADDRESS (02).
     *
     * 3) Perform the Read Discrete Outputs operation and check for success.
     *    - If successful, the Modbus Server sends a Modbus Response (mb_rsp).
     *    - If unsuccessful, return ExceptionCode SLAVE_DEVICE_FAILURE (04).
     */

    ModbusFrame response = { 0 };

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
    response.validDataCount = qtyOutput + 1;
    return response;
}

ModbusFrame ReadDiscreteInputCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    /**
     * Procedure for Reading Discreet Inputs
     *
     * 1) Check if the Quantity of Inputs is within the valid range (1 to 2000).
     *    - If valid, proceed to step 2.
     *    - If not valid, return ExceptionCode ILLEGAL_DATA_VALUE (03).
     *
     * 2) Verify the validity of the Starting Address and the sum of Starting Address + Quantity of Inputs.
     *    - If both conditions are met, proceed to step 3.
     *    - If not met, return ExceptionCode ILLEGAL_DATA_ADDRESS (02).
     *
     * 3) Perform the Read Discrete Inputs  operation and check for success.
     *    - If successful, the Modbus Server sends a Modbus Response (mb_rsp).
     *    - If unsuccessful, return ExceptionCode SLAVE_DEVICE_FAILURE (04).
     */

    ModbusFrame response = { 0 };

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
    response.validDataCount = qtyInput + 1;
    return response;
}

ModbusFrame ReadHoldingRegisterCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    /**
     * Procedure for Reading Holding Registers
     *
     * 1) Check if the Quantity of Inputs is within the valid range (1 to 2000).
     *    - If valid, proceed to step 2.
     *    - If not valid, return ExceptionCode ILLEGAL_DATA_VALUE (03).
     *
     * 2) Verify the validity of the Starting Address and the sum of Starting Address + Quantity of Registers.
     *    - If both conditions are met, proceed to step 3.
     *    - If not met, return ExceptionCode ILLEGAL_DATA_ADDRESS (02).
     *
     * 3) Perform the Read Holding Registers operation and check for success.
     *    - If successful, the Modbus Server sends a Modbus Response (mb_rsp).
     *    - If unsuccessful, return ExceptionCode SLAVE_DEVICE_FAILURE (04).
     */
}

ModbusFrame ReadInputRegisterCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    /**
     * Procedure for Reading Input Registers
     *
     * 1) Check if the Quantity of Inputs is within the valid range (1 to 2000).
     *    - If valid, proceed to step 2.
     *    - If not valid, return ExceptionCode ILLEGAL_DATA_VALUE (03).
     *
     * 2) Verify the validity of the Starting Address and the sum of Starting Address + Quantity of Registers.
     *    - If both conditions are met, proceed to step 3.
     *    - If not met, return ExceptionCode ILLEGAL_DATA_ADDRESS (02).
     *
     * 3) Perform the Read Input Registers operation and check for success.
     *    - If successful, the Modbus Server sends a Modbus Response (mb_rsp).
     *    - If unsuccessful, return ExceptionCode SLAVE_DEVICE_FAILURE (04).
     */
}
