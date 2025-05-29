#ifndef MODBUSDATAMODEL_HPP
#define MODBUSDATAMODEL_HPP

#include <cstdint>

#include <stddef.h>

#include <vector>

using DiscreteInputType = bool;
using CoilType = bool;
using InputRegisterType = uint16_t;
using HoldingRegisterType = uint16_t;

class ModbusDataModel {
public:
// Configurable limits - can be set at construction or runtime
static constexpr size_t DEFAULT_MAX_DISCRETE_INPUTS = 2000;
static constexpr size_t DEFAULT_MAX_COILS = 2000;
static constexpr size_t DEFAULT_MAX_INPUT_REGISTERS = 125;
static constexpr size_t DEFAULT_MAX_HOLDING_REGISTERS = 125;

// Legacy constants for backward compatibility
static constexpr size_t MAX_DISCREET_INPUT = DEFAULT_MAX_DISCRETE_INPUTS;
static constexpr size_t MAX_COILS = DEFAULT_MAX_COILS;
static constexpr size_t MAX_INPUT_REGISTERS = DEFAULT_MAX_INPUT_REGISTERS;
static constexpr size_t MAX_HOLDING_REGISTERS = DEFAULT_MAX_HOLDING_REGISTERS;

private:
std::vector<DiscreteInputType> discreteInputs;
std::vector<CoilType> coils;
std::vector<InputRegisterType> inputRegisters;
std::vector<HoldingRegisterType> holdingRegisters;

public:
// Constructors
ModbusDataModel()
    : discreteInputs(DEFAULT_MAX_DISCRETE_INPUTS, false)
    , coils(DEFAULT_MAX_COILS, false)
    , inputRegisters(DEFAULT_MAX_INPUT_REGISTERS, 0)
    , holdingRegisters(DEFAULT_MAX_HOLDING_REGISTERS, 0)
{}

ModbusDataModel(
    size_t maxDiscreteInputs,
    size_t maxCoils,
    size_t maxInputRegisters,
    size_t maxHoldingRegisters
)
    : discreteInputs(maxDiscreteInputs, false)
    , coils(maxCoils, false)
    , inputRegisters(maxInputRegisters, 0)
    , holdingRegisters(maxHoldingRegisters, 0)
{}

// Size getters
size_t getMaxDiscreteInputs() const {
    return discreteInputs.size();
}
size_t getMaxCoils() const {
    return coils.size();
}
size_t getMaxInputRegisters() const {
    return inputRegisters.size();
}
size_t getMaxHoldingRegisters() const {
    return holdingRegisters.size();
}

// Read operations
DiscreteInputType readDiscreteInput(uint16_t index) const;
CoilType readCoil(uint16_t index) const;
InputRegisterType readInputRegister(uint16_t index) const;
HoldingRegisterType readHoldingRegister(uint16_t index) const;

// Write operations
void writeCoil(uint16_t index, CoilType value);
void writeHoldingRegister(uint16_t index, HoldingRegisterType value);
void writeMultipleCoils(uint16_t start_index, const CoilType values[], size_t num_values);
void writeMultipleHoldingRegisters(
    uint16_t start_index,
    const HoldingRegisterType values[],
    size_t num_values
);

// New: Direct access for discrete inputs (for testing/initialization)
void setDiscreteInput(uint16_t index, DiscreteInputType value);
void setInputRegister(uint16_t index, InputRegisterType value);

// For legacy compatibility - public access (deprecated but maintained)
std::vector<DiscreteInputType>& getDiscreteInputs(){
    return discreteInputs;
}
std::vector<CoilType>& getCoils(){
    return coils;
}
std::vector<InputRegisterType>& getInputRegisters(){
    return inputRegisters;
}
std::vector<HoldingRegisterType>& getHoldingRegisters(){
    return holdingRegisters;
}
};

#endif