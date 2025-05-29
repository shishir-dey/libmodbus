/**
 * @file Modbus.hpp
 * @brief Core Modbus protocol definitions and enumerations
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file contains the fundamental Modbus protocol definitions including
 * function codes, exception codes, and diagnostic sub-function codes as
 * specified in the Modbus Application Protocol Specification V1.1b3.
 */

#ifndef MODBUS_HPP
#define MODBUS_HPP

#include <cstdint>

/**
 * @brief Modbus function codes as defined in the Modbus specification
 *
 * This enumeration contains all the standard Modbus function codes supported
 * by this library. Each function code corresponds to a specific operation
 * that can be performed on Modbus data.
 */
enum class ModbusFunctionCode : uint8_t {
    NONE = 0,                        ///< No function code (invalid/uninitialized)
    READ_COILS = 1,                  ///< Read coils (discrete outputs) - FC 01
    READ_DISCRETE_INPUTS = 2,        ///< Read discrete inputs - FC 02
    READ_HOLDING_REGISTERS = 3,      ///< Read holding registers - FC 03
    READ_INPUT_REGISTERS = 4,        ///< Read input registers - FC 04
    WRITE_SINGLE_COIL = 5,           ///< Write single coil - FC 05
    WRITE_SINGLE_REGISTER = 6,       ///< Write single register - FC 06
    READ_EXCEPTION_STATUS = 7,       ///< Read exception status - FC 07
    DIAGNOSTICS = 8,                 ///< Diagnostics - FC 08
    WRITE_MULTIPLE_COILS = 15,       ///< Write multiple coils - FC 15 (0x0F)
    WRITE_MULTIPLE_REGISTERS = 16,   ///< Write multiple registers - FC 16 (0x10)
};

/**
 * @brief Modbus exception codes returned in error responses
 *
 * When a Modbus request cannot be processed successfully, the server
 * responds with an exception frame containing one of these exception codes
 * to indicate the specific error condition.
 */
enum class ModbusExceptionCode : uint8_t {
    NONE = 0x00,                                        ///< No exception (success)
    ILLEGAL_FUNCTION = 0x01,                            ///< Function code not supported
    ILLEGAL_DATA_ADDRESS = 0x02,                        ///< Data address not valid
    ILLEGAL_DATA_VALUE = 0x03,                          ///< Data value not valid
    SLAVE_DEVICE_FAILURE = 0x04,                        ///< Unrecoverable error in slave device
    ACKNOWLEDGE = 0x05,                                 ///< Request accepted, processing
    SLAVE_DEVICE_BUSY = 0x06,                           ///< Slave device busy
    NEGATIVE_ACKNOWLEDGMENT = 0x07,                     ///< Request cannot be performed
    MEMORY_PARITY_ERROR = 0x08,                         ///< Memory parity error
    GATEWAY_PATH_UNAVAILABLE = 0x0A,                    ///< Gateway path unavailable
    GATEWAY_TARGET_DEVICE_FAILED_TO_RESPOND = 0x0B,    ///< Gateway target device failed to respond
};

/**
 * @brief Diagnostic sub-function codes for function code 08 (Diagnostics)
 *
 * The diagnostics function (FC 08) supports various sub-functions for
 * testing communication and retrieving diagnostic information from
 * Modbus devices. This enumeration defines the standard sub-function codes.
 */
enum class ModbusDiagnosticsCode : uint16_t {
    RETURN_QUERY_DATA = 0x0000,                         ///< Echo back query data
    RESTART_COMMUNICATIONS_OPTION = 0x0001,             ///< Restart communications option
    RETURN_DIAGNOSTIC_REGISTER = 0x0002,                ///< Return diagnostic register
    CHANGE_ASCII_INPUT_DELIMITER = 0x0003,              ///< Change ASCII input delimiter
    FORCE_LISTEN_ONLY_MODE = 0x0004,                    ///< Force listen only mode
    CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER = 0x000A,    ///< Clear counters and diagnostic register
    RETURN_BUS_MESSAGE_COUNT = 0x000B,                  ///< Return bus message count
    RETURN_BUS_COMMUNICATION_ERROR_COUNT = 0x000C,      ///< Return bus communication error count
    RETURN_BUS_EXCEPTION_ERROR_COUNT = 0x000D,          ///< Return bus exception error count
    RETURN_SLAVE_MESSAGE_COUNT = 0x000E,                ///< Return slave message count
    RETURN_SLAVE_NO_RESPONSE_COUNT = 0x000F,            ///< Return slave no response count
    RETURN_SLAVE_NAK_COUNT = 0x0010,                    ///< Return slave NAK count
    RETURN_SLAVE_BUSY_COUNT = 0x0011,                   ///< Return slave busy count
    RETURN_BUS_CHARACTER_OVERRUN_COUNT = 0x0012,        ///< Return bus character overrun count
};

#endif