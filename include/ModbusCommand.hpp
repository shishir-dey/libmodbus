/**
 * @file ModbusCommand.hpp
 * @brief Modbus command implementations for all supported function codes
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file contains the command pattern implementation for all supported
 * Modbus function codes. Each command class handles the specific logic
 * for processing requests and generating appropriate responses or exceptions.
 */

#ifndef MODBUSCOMMAND_HPP
#define MODBUSCOMMAND_HPP

#include "ModbusDataModel.hpp"
#include "ModbusFrame.hpp"

/**
 * @class ModbusCommand
 * @brief Abstract base class for all Modbus command implementations
 *
 * This class defines the interface for Modbus command processing using
 * the Command pattern. Each specific function code has its own derived
 * command class that implements the execute() method.
 *
 * The class also provides common validation methods that are used by
 * multiple command implementations to ensure consistent error handling
 * and compliance with Modbus specifications.
 */
class ModbusCommand {
public:
/**
 * @brief Virtual destructor for proper polymorphic destruction
 */
virtual ~ModbusCommand() = default;

/**
 * @brief Execute the command with given data model and request
 *
 * This is the main entry point for command execution. Each derived class
 * implements this method to handle its specific function code.
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming Modbus request frame
 * @return Response frame (normal response or exception)
 */
virtual ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request) = 0;

protected:
/**
 * @brief Generic quantity validation for Modbus requests
 *
 * Validates that the quantity field in a Modbus request falls within
 * the specified minimum and maximum values.
 *
 * @param request The incoming request frame
 * @param minQuantity Minimum allowed quantity
 * @param maxQuantity Maximum allowed quantity
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 *
 * @note This method assumes the quantity is in bytes 2-3 of the frame data
 */
bool validateQuantity(
    const ModbusFrame& request,
    uint16_t minQuantity,
    uint16_t maxQuantity,
    ModbusFrame& response
);

/**
 * @brief Address range validation for Modbus requests
 *
 * Validates that the starting address and quantity combination is within
 * the valid address range for the data type.
 *
 * @param request The incoming request frame
 * @param maxValidAddress Maximum valid address for this data type
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 *
 * @note This method assumes address is in bytes 0-1 and quantity in bytes 2-3
 */
bool validateAddress(const ModbusFrame& request, uint16_t maxValidAddress, ModbusFrame& response);

// Function-specific quantity validation methods

/**
 * @brief Validate quantity for Read Coils function (FC 01)
 *
 * Validates quantity range for coil reading operations (1-2000 coils).
 *
 * @param request The incoming request frame
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 */
bool validateReadCoilsQuantity(const ModbusFrame& request, ModbusFrame& response);

/**
 * @brief Validate quantity for Read Registers functions (FC 03, 04)
 *
 * Validates quantity range for register reading operations (1-125 registers).
 *
 * @param request The incoming request frame
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 */
bool validateReadRegistersQuantity(const ModbusFrame& request, ModbusFrame& response);

/**
 * @brief Validate quantity for Write Multiple Coils function (FC 15)
 *
 * Validates quantity range for multiple coil writing operations (1-1968 coils).
 *
 * @param request The incoming request frame
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 */
bool validateWriteMultipleCoilsQuantity(const ModbusFrame& request, ModbusFrame& response);

/**
 * @brief Validate quantity for Write Multiple Registers function (FC 16)
 *
 * Validates quantity range for multiple register writing operations (1-123 registers).
 *
 * @param request The incoming request frame
 * @param response Reference to response frame (populated with exception if validation fails)
 * @return true if validation passes, false if exception was generated
 */
bool validateWriteMultipleRegistersQuantity(const ModbusFrame& request, ModbusFrame& response);
};

/**
 * @class ReadCoilCommand
 * @brief Implementation of Read Coils function (FC 01)
 *
 * Reads the status of coils (discrete outputs) from the data model.
 * Returns a bit-packed response where each bit represents one coil state.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Coils (2 bytes)
 *
 * Response format:
 * - Byte Count (1 byte)
 * - Coil Status (n bytes, bit-packed)
 *
 * @note Coils are packed 8 per byte, LSB first
 */
class ReadCoilCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Read Coils command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing coil status or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class ReadDiscreteInputCommand
 * @brief Implementation of Read Discrete Inputs function (FC 02)
 *
 * Reads the status of discrete inputs from the data model.
 * Returns a bit-packed response where each bit represents one input state.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Inputs (2 bytes)
 *
 * Response format:
 * - Byte Count (1 byte)
 * - Input Status (n bytes, bit-packed)
 *
 * @note Inputs are packed 8 per byte, LSB first
 */
class ReadDiscreteInputCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Read Discrete Inputs command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing input status or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class ReadInputRegisterCommand
 * @brief Implementation of Read Input Registers function (FC 04)
 *
 * Reads input register values from the data model.
 * Returns register values in big-endian format.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Registers (2 bytes)
 *
 * Response format:
 * - Byte Count (1 byte)
 * - Register Values (n*2 bytes, big-endian)
 */
class ReadInputRegisterCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Read Input Registers command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing register values or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class ReadHoldingRegisterCommand
 * @brief Implementation of Read Holding Registers function (FC 03)
 *
 * Reads holding register values from the data model.
 * Returns register values in big-endian format.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Registers (2 bytes)
 *
 * Response format:
 * - Byte Count (1 byte)
 * - Register Values (n*2 bytes, big-endian)
 */
class ReadHoldingRegisterCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Read Holding Registers command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing register values or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class WriteCoilCommand
 * @brief Implementation of Write Single Coil function (FC 05)
 *
 * Writes a single coil value to the data model.
 *
 * Request format:
 * - Output Address (2 bytes)
 * - Output Value (2 bytes: 0x0000 for OFF, 0xFF00 for ON)
 *
 * Response format:
 * - Echo of the complete request
 */
class WriteCoilCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Write Single Coil command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame (echo of request) or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class WriteHoldingRegisterCommand
 * @brief Implementation of Write Single Register function (FC 06)
 *
 * Writes a single holding register value to the data model.
 *
 * Request format:
 * - Register Address (2 bytes)
 * - Register Value (2 bytes)
 *
 * Response format:
 * - Echo of the complete request
 */
class WriteHoldingRegisterCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Write Single Register command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame (echo of request) or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class WriteMultipleCoilsCommand
 * @brief Implementation of Write Multiple Coils function (FC 15)
 *
 * Writes multiple coil values to the data model.
 * Coil values are transmitted as bit-packed bytes.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Outputs (2 bytes)
 * - Byte Count (1 byte)
 * - Outputs Value (n bytes, bit-packed)
 *
 * Response format:
 * - Starting Address (2 bytes)
 * - Quantity of Outputs (2 bytes)
 *
 * @note Coils are packed 8 per byte, LSB first
 */
class WriteMultipleCoilsCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Write Multiple Coils command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing address and quantity or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class WriteMultipleRegistersCommand
 * @brief Implementation of Write Multiple Registers function (FC 16)
 *
 * Writes multiple holding register values to the data model.
 * Register values are transmitted in big-endian format.
 *
 * Request format:
 * - Starting Address (2 bytes)
 * - Quantity of Registers (2 bytes)
 * - Byte Count (1 byte)
 * - Registers Value (n*2 bytes, big-endian)
 *
 * Response format:
 * - Starting Address (2 bytes)
 * - Quantity of Registers (2 bytes)
 */
class WriteMultipleRegistersCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Write Multiple Registers command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing address and quantity or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

/**
 * @class DiagnosticsCommand
 * @brief Implementation of Diagnostics function (FC 08)
 *
 * Provides a series of diagnostic and testing functions for
 * Modbus communication. Currently supports basic sub-functions
 * like Return Query Data and communication restart.
 *
 * Request format:
 * - Sub-function (2 bytes)
 * - Data (2 bytes, sub-function specific)
 *
 * Response format:
 * - Sub-function (2 bytes, echo)
 * - Data (2 bytes, sub-function specific)
 *
 * @note Only basic diagnostic sub-functions are currently implemented
 */
class DiagnosticsCommand : public ModbusCommand {
public:
/**
 * @brief Execute the Diagnostics command
 *
 * @param data Reference to the Modbus data model
 * @param request The incoming request frame
 * @return Response frame containing diagnostic response or exception
 */
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

#endif