#include <ModbusDataModel.hpp>

DiscreteInputType ModbusDataModel::readDiscreteInput(uint8_t index) const
{
    if (index < discreteInputs.size()) {
        return discreteInputs[index];
    } else {
        return false;
    }
}

CoilType ModbusDataModel::readCoil(uint8_t index) const
{
    if (index < coils.size()) {
        return coils[index];
    } else {
        return false;
    }
}

InputRegisterType ModbusDataModel::readInputRegister(uint8_t index) const
{
    if (index < inputRegisters.size()) {
        return inputRegisters[index];
    } else {
        return 0;
    }
}

HoldingRegisterType ModbusDataModel::readHoldingRegister(uint8_t index) const
{
    if (index < holdingRegisters.size()) {
        return holdingRegisters[index];
    } else {
        return 0;
    }
}

void ModbusDataModel::writeCoil(uint8_t index, CoilType value)
{
    if (index < coils.size()) {
        coils[index] = value;
    }
}

void ModbusDataModel::writeHoldingRegister(uint8_t index, HoldingRegisterType value)
{
    if (index < holdingRegisters.size()) {
        holdingRegisters[index] = value;
    }
}

void ModbusDataModel::writeMultipleCoils(uint8_t start_index, const CoilType values[], size_t num_values)
{
    if (start_index < coils.size() && start_index + num_values <= coils.size()) {
        for (size_t i = 0; i < num_values; ++i) {
            coils[start_index + i] = values[i];
        }
    }
}

void ModbusDataModel::writeMultipleHoldingRegisters(uint8_t start_index, const HoldingRegisterType values[], size_t num_values)
{
    if (start_index < holdingRegisters.size() && start_index + num_values <= inputRegisters.size()) {
        for (size_t i = 0; i < num_values; ++i) {
            holdingRegisters[start_index + i] = values[i];
        }
    }
}
