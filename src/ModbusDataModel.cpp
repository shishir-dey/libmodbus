#include <ModbusDataModel.h>

DiscreteInputType ModbusDataModel::readDiscreteInput(uint8_t index) const
{
    // Check for valid index within discreteInputs array
    if (index < discreteInputs.size()) {
        return discreteInputs[index];
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        return false; // Replace with appropriate behaviour
    }
}

CoilType ModbusDataModel::readCoil(uint8_t index) const
{
    // Check for valid index within coils array
    if (index < coils.size()) {
        return coils[index];
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        return false; // Replace with appropriate behaviour
    }
}

InputRegisterType ModbusDataModel::readInputRegister(uint8_t index) const
{
    // Check for valid index within inputRegisters array
    if (index < inputRegisters.size()) {
        return inputRegisters[index];
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        return 0; // Replace with appropriate behaviour
    }
}

HoldingRegisterType ModbusDataModel::readHoldingRegister(uint8_t index) const
{
    // Check for valid index within holdingRegisters array
    if (index < holdingRegisters.size()) {
        return holdingRegisters[index];
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        return 0; // Replace with appropriate behaviour
    }
}

void ModbusDataModel::writeCoil(uint8_t index, CoilType value)
{
    // Check for valid index within coils array
    if (index < coils.size()) {
        coils[index] = value;
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        // Consider omitting for read-only registers
    }
}

void ModbusDataModel::writeInputRegister(uint8_t index, InputRegisterType value)
{
    // Check for valid index within inputRegisters array
    if (index < inputRegisters.size()) {
        // Typically read-only, implement appropriate behaviour if used
    } else {
        // Handle invalid index (e.g., throw an exception, log an error)
        // Consider omitting for read-only registers
    }
}

void ModbusDataModel::writeMultipleCoils(uint8_t start_index, const CoilType values[], size_t num_values)
{
    // Check for valid start index and values length
    if (start_index < coils.size() && start_index + num_values <= coils.size()) {
        for (size_t i = 0; i < num_values; ++i) {
            coils[start_index + i] = values[i];
        }
    } else {
        // Handle invalid input (e.g., throw an exception, log an error)
        // Consider partial writes or clamping indices
    }
}

void ModbusDataModel::writeMultipleInputRegisters(uint8_t start_index, const InputRegisterType values[], size_t num_values)
{
    // Check for valid start index and values length
    if (start_index < inputRegisters.size() && start_index + num_values <= inputRegisters.size()) {
        // Typically read-only, implement appropriate behaviour if used
    } else {
        // Handle invalid input (e.g., throw an exception, log an error)
        // Consider partial writes or clamping indices
    }
}
