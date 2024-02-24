#include "ModbusCommand.hpp"

ModbusFrame ModbusCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
}

ModbusFrame ReadCoilCommand::execute(ModbusDataModel& data, const ModbusFrame& request)
{
    ModbusFrame response = { 0 };

    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyOutput = (request.frameData[2] << 8) | request.frameData[3];

    if (qtyOutput >= 1 && qtyOutput <= 0x7D0) {
        if (startAddress >= 0 && startAddress + qtyOutput <= ModbusDataModel::MAX_COILS) {
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
        } else {
            response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
            response.frameType = ModbusFrameType::EXCEPTION;
            return response;
        }
    } else {
        response.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
        response.frameType = ModbusFrameType::EXCEPTION;
        return response;
    }
}
