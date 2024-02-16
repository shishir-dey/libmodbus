#include "config.h"
#include <array>
#include <cstdint>
#include <stddef.h>

// Data types for register values
using DiscreteInputType = bool;
using CoilType = bool;
using InputRegisterType = uint16_t;
using HoldingRegisterType = uint16_t;

class ModbusDataModel {
public:
    // Modbus address of the device
    uint8_t address;

    // Exception code (optional)
    uint8_t exception = 0;

    // Member variables for device states
    std::array<DiscreteInputType, MAX_DISCREET_INPUT> discreteInputs;
    std::array<CoilType, MAX_COILS> coils;
    std::array<InputRegisterType, MAX_INPUT_REGISTERS> inputRegisters;
    std::array<HoldingRegisterType, MAX_HOLDING_REGISTERS> holdingRegisters;

    // Function to read a discrete input
    DiscreteInputType readDiscreteInput(uint8_t index) const;

    // Function to read a coil
    CoilType readCoil(uint8_t index) const;

    // Function to read an input register
    InputRegisterType readInputRegister(uint8_t index) const;

    // Function to read a holding register
    HoldingRegisterType readHoldingRegister(uint8_t index) const;

    // Function to write a coil
    void writeCoil(uint8_t index, CoilType value);

    // Function to write an input register (typically read-only)
    void writeInputRegister(uint8_t index, InputRegisterType value);

    // Function to write multiple coils
    void writeMultipleCoils(uint8_t start_index, const CoilType values[], size_t num_values);

    // Function to write multiple input registers (typically read-only)
    void writeMultipleInputRegisters(uint8_t start_index, const InputRegisterType values[], size_t num_values);
};
