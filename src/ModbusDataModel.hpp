#ifndef MODBUSDATAMODEL_HPP
#define MODBUSDATAMODEL_HPP

#include <array>
#include <cstdint>
#include <stddef.h>

using DiscreteInputType = bool;
using CoilType = bool;
using InputRegisterType = uint16_t;
using HoldingRegisterType = uint16_t;

class ModbusDataModel {
public:
    static constexpr size_t MAX_DISCREET_INPUT = 8;
    static constexpr size_t MAX_COILS = 8;
    static constexpr size_t MAX_INPUT_REGISTERS = 8;
    static constexpr size_t MAX_HOLDING_REGISTERS = 8;

    std::array<DiscreteInputType, MAX_DISCREET_INPUT> discreteInputs;
    std::array<CoilType, MAX_COILS> coils;
    std::array<InputRegisterType, MAX_INPUT_REGISTERS> inputRegisters;
    std::array<HoldingRegisterType, MAX_HOLDING_REGISTERS> holdingRegisters;

    DiscreteInputType readDiscreteInput(uint8_t index) const;

    CoilType readCoil(uint8_t index) const;

    InputRegisterType readInputRegister(uint8_t index) const;

    HoldingRegisterType readHoldingRegister(uint8_t index) const;

    void writeCoil(uint8_t index, CoilType value);

    void writeHoldingRegister(uint8_t index, HoldingRegisterType value);

    void writeMultipleCoils(uint8_t start_index, const CoilType values[], size_t num_values);

    void writeMultipleHoldingRegisters(uint8_t start_index, const HoldingRegisterType values[], size_t num_values);
};

#endif