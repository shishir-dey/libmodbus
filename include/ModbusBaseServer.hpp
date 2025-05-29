/**
 * @file ModbusBaseServer.hpp
 * @brief Base Modbus server implementation with protocol-specific derivatives
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file contains the base Modbus server class that provides common
 * functionality for all Modbus server implementations, along with the
 * RTU-specific server implementation.
 */

#ifndef MODBASESERVER_HPP
#define MODBASESERVER_HPP

#include <map>
#include <memory>

#include "Modbus.hpp"
#include "ModbusCommand.hpp"

/**
 * @class ModbusBaseServer
 * @brief Abstract base class for all Modbus server implementations
 *
 * This class provides the foundation for Modbus server implementations
 * across different transport protocols (RTU, ASCII, TCP). It manages
 * the data model and command registry that are common to all protocols.
 *
 * The class uses the Command pattern to handle different function codes,
 * with each function code mapped to a specific command implementation.
 * This design allows for easy extension and modification of supported
 * function codes.
 *
 * @note This is an abstract base class. Use the protocol-specific
 * derived classes (ModbusRtuServer, etc.) for actual server implementation.
 */
class ModbusBaseServer {
public:
/**
 * @brief Modbus data model instance
 *
 * Contains all four types of Modbus data: coils, discrete inputs,
 * holding registers, and input registers. This data model is shared
 * across all command implementations.
 */
ModbusDataModel data;

/**
 * @brief Command registry mapping function codes to command implementations
 *
 * This map contains the association between Modbus function codes and
 * their corresponding command implementations. New function codes can
 * be supported by adding entries to this map.
 */
std::map<ModbusFunctionCode, std::unique_ptr<ModbusCommand> > commands;

/**
 * @brief Constructor initializes the command registry
 *
 * Registers all supported Modbus function codes with their corresponding
 * command implementations. Currently supports:
 * - FC 01: Read Coils
 * - FC 02: Read Discrete Inputs
 * - FC 03: Read Holding Registers
 * - FC 04: Read Input Registers
 * - FC 05: Write Single Coil
 * - FC 06: Write Single Register
 * - FC 08: Diagnostics
 * - FC 15: Write Multiple Coils
 * - FC 16: Write Multiple Registers
 */
ModbusBaseServer()
{
    commands[ModbusFunctionCode::READ_COILS] = std::make_unique<ReadCoilCommand>();
    commands[ModbusFunctionCode::READ_DISCRETE_INPUTS] = std::make_unique<ReadDiscreteInputCommand>();
    commands[ModbusFunctionCode::READ_HOLDING_REGISTERS] =
        std::make_unique<ReadHoldingRegisterCommand>();
    commands[ModbusFunctionCode::READ_INPUT_REGISTERS] = std::make_unique<ReadInputRegisterCommand>();
    commands[ModbusFunctionCode::WRITE_SINGLE_COIL] = std::make_unique<WriteCoilCommand>();
    commands[ModbusFunctionCode::WRITE_SINGLE_REGISTER] =
        std::make_unique<WriteHoldingRegisterCommand>();
    commands[ModbusFunctionCode::WRITE_MULTIPLE_COILS] = std::make_unique<WriteMultipleCoilsCommand>();
    commands[ModbusFunctionCode::WRITE_MULTIPLE_REGISTERS] = std::make_unique<WriteMultipleRegistersCommand>();
    commands[ModbusFunctionCode::DIAGNOSTICS] = std::make_unique<DiagnosticsCommand>();
}

/**
 * @brief Process a Modbus request and generate a response
 *
 * This is the main entry point for request processing. Each protocol-specific
 * derived class must implement this method to handle the protocol-specific
 * frame formatting and validation.
 *
 * @param requestData Raw request data as received from the transport layer
 * @return Raw response data to be sent back, or empty vector if no response
 *
 * @note The base implementation returns an empty vector. Derived classes
 * must override this method to provide actual functionality.
 */
virtual std::vector<uint8_t> process(const std::vector<uint8_t>& requestData)
{
    return {};
}
};

/**
 * @class ModbusRtuServer
 * @brief Modbus RTU server implementation
 *
 * Implements the Modbus RTU (Remote Terminal Unit) protocol for serial
 * communication. This class handles RTU-specific frame processing including:
 * - CRC validation and generation
 * - Slave address checking
 * - Binary frame formatting
 * - Exception response generation
 *
 * The RTU protocol uses binary encoding and CRC-16 checksums for error
 * detection. Frames are transmitted without start/stop delimiters, relying
 * on timing gaps for frame synchronization.
 *
 * @note This server is configured to respond to slave address 1 by default.
 * Broadcast messages (address 0) are processed but no response is sent.
 */
class ModbusRtuServer : public ModbusBaseServer {
public:
/**
 * @brief Process an RTU request and generate an RTU response
 *
 * This method handles the complete RTU request processing pipeline:
 * 1. Frame validation (minimum size, CRC check)
 * 2. Slave address verification
 * 3. Function code lookup and command execution
 * 4. Response frame generation and CRC calculation
 *
 * @param requestData Raw RTU frame data including slave address and CRC
 * @return Complete RTU response frame, or empty vector if no response needed
 *
 * The method performs the following validations:
 * - Minimum frame size (4 bytes: address + function + CRC)
 * - CRC integrity check
 * - Slave address matching (responds to address 1 only)
 * - Function code support check
 *
 * @note No response is generated for:
 * - Invalid CRC
 * - Wrong slave address
 * - Broadcast messages (address 0)
 *
 * Exception responses are generated for:
 * - Unsupported function codes
 * - Invalid request parameters
 * - Data access errors
 */
std::vector<uint8_t> process(const std::vector<uint8_t>& requestData);
};

#endif