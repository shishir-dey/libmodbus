/**
 * @file ModbusDataModel.hpp
 * @brief Modbus data model implementation with configurable limits
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file implements the Modbus data model that stores all four types of
 * Modbus data: coils, discrete inputs, holding registers, and input registers.
 * The implementation supports configurable limits and provides both modern
 * vector-based storage and legacy compatibility.
 */

#ifndef MODBUSDATAMODEL_HPP
#define MODBUSDATAMODEL_HPP

#include <cstdint>

#include <stddef.h>

#include <vector>

/**
 * @brief Type alias for discrete input values
 *
 * Discrete inputs are read-only boolean values that typically represent
 * the state of physical inputs to the device.
 */
using DiscreteInputType = bool;

/**
 * @brief Type alias for coil values
 *
 * Coils are read/write boolean values that typically control physical
 * outputs of the device.
 */
using CoilType = bool;

/**
 * @brief Type alias for input register values
 *
 * Input registers are read-only 16-bit values that typically contain
 * analog input values or other measured data.
 */
using InputRegisterType = uint16_t;

/**
 * @brief Type alias for holding register values
 *
 * Holding registers are read/write 16-bit values that can store
 * configuration parameters, setpoints, or other control data.
 */
using HoldingRegisterType = uint16_t;

/**
 * @class ModbusDataModel
 * @brief Configurable Modbus data model with support for all four data types
 *
 * This class implements a complete Modbus data model that stores:
 * - Coils (read/write discrete outputs)
 * - Discrete Inputs (read-only discrete inputs)
 * - Holding Registers (read/write 16-bit registers)
 * - Input Registers (read-only 16-bit registers)
 *
 * The implementation uses vectors for dynamic sizing and supports both
 * default limits that comply with Modbus specifications and custom limits
 * for specialized applications.
 *
 * @note All data addresses are 0-based internally, following common
 * programming conventions, even though Modbus addresses are traditionally 1-based.
 *
 * @see ModbusFunctionCode for supported operations
 */
class ModbusDataModel {
public:
// Configurable limits - can be set at construction or runtime
/** @brief Default maximum number of discrete inputs (2000 per Modbus spec) */
static constexpr size_t DEFAULT_MAX_DISCRETE_INPUTS = 2000;

/** @brief Default maximum number of coils (2000 per Modbus spec) */
static constexpr size_t DEFAULT_MAX_COILS = 2000;

/** @brief Default maximum number of input registers (125 per Modbus spec) */
static constexpr size_t DEFAULT_MAX_INPUT_REGISTERS = 125;

/** @brief Default maximum number of holding registers (125 per Modbus spec) */
static constexpr size_t DEFAULT_MAX_HOLDING_REGISTERS = 125;

// Legacy constants for backward compatibility
/** @brief Legacy constant for maximum discrete inputs @deprecated Use DEFAULT_MAX_DISCRETE_INPUTS */
static constexpr size_t MAX_DISCREET_INPUT = DEFAULT_MAX_DISCRETE_INPUTS;

/** @brief Legacy constant for maximum coils @deprecated Use DEFAULT_MAX_COILS */
static constexpr size_t MAX_COILS = DEFAULT_MAX_COILS;

/** @brief Legacy constant for maximum input registers @deprecated Use DEFAULT_MAX_INPUT_REGISTERS */
static constexpr size_t MAX_INPUT_REGISTERS = DEFAULT_MAX_INPUT_REGISTERS;

/** @brief Legacy constant for maximum holding registers @deprecated Use DEFAULT_MAX_HOLDING_REGISTERS */
static constexpr size_t MAX_HOLDING_REGISTERS = DEFAULT_MAX_HOLDING_REGISTERS;

private:
std::vector<DiscreteInputType> discreteInputs;     ///< Storage for discrete input values
std::vector<CoilType> coils;                       ///< Storage for coil values
std::vector<InputRegisterType> inputRegisters;     ///< Storage for input register values
std::vector<HoldingRegisterType> holdingRegisters; ///< Storage for holding register values

public:
// Constructors

/**
 * @brief Default constructor with standard Modbus limits
 *
 * Creates a data model with the default limits as specified in the
 * Modbus application protocol specification:
 * - 2000 discrete inputs
 * - 2000 coils
 * - 125 input registers
 * - 125 holding registers
 *
 * All values are initialized to false/0.
 */
ModbusDataModel()
    : discreteInputs(DEFAULT_MAX_DISCRETE_INPUTS, false)
    , coils(DEFAULT_MAX_COILS, false)
    , inputRegisters(DEFAULT_MAX_INPUT_REGISTERS, 0)
    , holdingRegisters(DEFAULT_MAX_HOLDING_REGISTERS, 0)
{}

/**
 * @brief Custom constructor with user-defined limits
 *
 * Creates a data model with custom limits for specialized applications
 * that may need different capacity than the standard Modbus limits.
 *
 * @param maxDiscreteInputs Maximum number of discrete inputs
 * @param maxCoils Maximum number of coils
 * @param maxInputRegisters Maximum number of input registers
 * @param maxHoldingRegisters Maximum number of holding registers
 *
 * @note All values are initialized to false/0.
 */
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

/**
 * @brief Get the maximum number of discrete inputs
 * @return Maximum number of discrete inputs this data model can store
 */
size_t getMaxDiscreteInputs() const {
    return discreteInputs.size();
}

/**
 * @brief Get the maximum number of coils
 * @return Maximum number of coils this data model can store
 */
size_t getMaxCoils() const {
    return coils.size();
}

/**
 * @brief Get the maximum number of input registers
 * @return Maximum number of input registers this data model can store
 */
size_t getMaxInputRegisters() const {
    return inputRegisters.size();
}

/**
 * @brief Get the maximum number of holding registers
 * @return Maximum number of holding registers this data model can store
 */
size_t getMaxHoldingRegisters() const {
    return holdingRegisters.size();
}

// Read operations

/**
 * @brief Read a discrete input value
 *
 * @param index 0-based index of the discrete input to read
 * @return Value of the discrete input (true/false), or false if index is out of range
 *
 * @note This function performs bounds checking and returns false for invalid indices
 */
DiscreteInputType readDiscreteInput(uint16_t index) const;

/**
 * @brief Read a coil value
 *
 * @param index 0-based index of the coil to read
 * @return Value of the coil (true/false), or false if index is out of range
 *
 * @note This function performs bounds checking and returns false for invalid indices
 */
CoilType readCoil(uint16_t index) const;

/**
 * @brief Read an input register value
 *
 * @param index 0-based index of the input register to read
 * @return Value of the input register (0-65535), or 0 if index is out of range
 *
 * @note This function performs bounds checking and returns 0 for invalid indices
 */
InputRegisterType readInputRegister(uint16_t index) const;

/**
 * @brief Read a holding register value
 *
 * @param index 0-based index of the holding register to read
 * @return Value of the holding register (0-65535), or 0 if index is out of range
 *
 * @note This function performs bounds checking and returns 0 for invalid indices
 */
HoldingRegisterType readHoldingRegister(uint16_t index) const;

// Write operations

/**
 * @brief Write a single coil value
 *
 * @param index 0-based index of the coil to write
 * @param value New value for the coil (true/false)
 *
 * @note This function performs bounds checking and ignores writes to invalid indices
 */
void writeCoil(uint16_t index, CoilType value);

/**
 * @brief Write a single holding register value
 *
 * @param index 0-based index of the holding register to write
 * @param value New value for the holding register (0-65535)
 *
 * @note This function performs bounds checking and ignores writes to invalid indices
 */
void writeHoldingRegister(uint16_t index, HoldingRegisterType value);

/**
 * @brief Write multiple coil values starting at a specified address
 *
 * @param start_index 0-based starting index for the write operation
 * @param values Array of coil values to write
 * @param num_values Number of values to write from the array
 *
 * @note This function performs bounds checking and only writes values that fit
 * within the valid address range. Partial writes may occur if the range extends
 * beyond the maximum address.
 */
void writeMultipleCoils(uint16_t start_index, const CoilType values[], size_t num_values);

/**
 * @brief Write multiple holding register values starting at a specified address
 *
 * @param start_index 0-based starting index for the write operation
 * @param values Array of holding register values to write
 * @param num_values Number of values to write from the array
 *
 * @note This function performs bounds checking and only writes values that fit
 * within the valid address range. Partial writes may occur if the range extends
 * beyond the maximum address.
 */
void writeMultipleHoldingRegisters(
    uint16_t start_index,
    const HoldingRegisterType values[],
    size_t num_values
);

// New: Direct access for discrete inputs (for testing/initialization)

/**
 * @brief Set a discrete input value (for testing/initialization)
 *
 * @param index 0-based index of the discrete input to set
 * @param value New value for the discrete input (true/false)
 *
 * @note This function is primarily intended for testing and initialization.
 * In a real Modbus device, discrete inputs would typically be updated by
 * hardware or other system components, not by Modbus commands.
 *
 * @note This function performs bounds checking and ignores writes to invalid indices
 */
void setDiscreteInput(uint16_t index, DiscreteInputType value);

/**
 * @brief Set an input register value (for testing/initialization)
 *
 * @param index 0-based index of the input register to set
 * @param value New value for the input register (0-65535)
 *
 * @note This function is primarily intended for testing and initialization.
 * In a real Modbus device, input registers would typically be updated by
 * hardware or other system components, not by Modbus commands.
 *
 * @note This function performs bounds checking and ignores writes to invalid indices
 */
void setInputRegister(uint16_t index, InputRegisterType value);

// For legacy compatibility - public access (deprecated but maintained)

/**
 * @brief Get direct access to discrete inputs vector
 * @return Reference to the internal discrete inputs vector
 * @deprecated This method is deprecated. Use the read/write methods instead.
 * @warning Direct access bypasses bounds checking and may lead to undefined behavior
 */
std::vector<DiscreteInputType>& getDiscreteInputs(){
    return discreteInputs;
}

/**
 * @brief Get direct access to coils vector
 * @return Reference to the internal coils vector
 * @deprecated This method is deprecated. Use the read/write methods instead.
 * @warning Direct access bypasses bounds checking and may lead to undefined behavior
 */
std::vector<CoilType>& getCoils(){
    return coils;
}

/**
 * @brief Get direct access to input registers vector
 * @return Reference to the internal input registers vector
 * @deprecated This method is deprecated. Use the read/write methods instead.
 * @warning Direct access bypasses bounds checking and may lead to undefined behavior
 */
std::vector<InputRegisterType>& getInputRegisters(){
    return inputRegisters;
}

/**
 * @brief Get direct access to holding registers vector
 * @return Reference to the internal holding registers vector
 * @deprecated This method is deprecated. Use the read/write methods instead.
 * @warning Direct access bypasses bounds checking and may lead to undefined behavior
 */
std::vector<HoldingRegisterType>& getHoldingRegisters(){
    return holdingRegisters;
}
};

#endif