#include "ModbusCommand.hpp"

ModbusPDU ModbusCommand::execute(ModbusDataModel& data, const ModbusRequestFrame& request)
{
}

ModbusPDU ReadCoilCommand::execute(ModbusDataModel& data, const ModbusRequestFrame& request)
{
    uint16_t startAddress = (request.frameData[0] << 8) | request.frameData[1];
    uint16_t qtyOutput = (request.frameData[2] << 8) | request.frameData[3];

    if (qtyOutput >= 1 && qtyOutput <= 0x7D0) {
        if (startAddress >= 0 && startAddress + qtyOutput <= ModbusDataModel::MAX_COILS) {
            ModbusResponseFrame mRF;
            mRF.functionCode = ModbusFunctionCode::READ_COILS;
            uint8_t byteIndex = 1;
            for (uint16_t index = startAddress; index < startAddress + qtyOutput; ++index) {
                bool temp = data.readCoil(index);
                mRF.frameData[byteIndex] |= (temp << ((index - startAddress) % 8));
                if ((index - startAddress) % 8 == 7) {
                    byteIndex++;
                }
            }
            mRF.frameData[0] = static_cast<uint8_t>((qtyOutput + 7) / 8);
            return ModbusPDU(mRF);
        } else {
            ModbusExceptionFrame exception;
            exception.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_ADDRESS;
            return ModbusPDU(exception);
        }
    } else {
        ModbusExceptionFrame exception;
        exception.exceptionCode = ModbusExceptionCode::ILLEGAL_DATA_VALUE;
        return ModbusPDU(exception);
    }
}
