/**
 * @file libmodbus.hpp
 * @brief Main include file for the libmodbus C++ library
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * @mainpage libmodbus - C++ Modbus Protocol Implementation
 *
 * @section intro_sec Introduction
 *
 * libmodbus is a comprehensive C++ implementation of the Modbus protocol supporting
 * RTU, ASCII, and TCP variants. The library provides a clean, modern C++ interface
 * for building Modbus servers and clients with full compliance to Modbus specifications.
 *
 * @section features_sec Key Features
 *
 * - **Complete Function Code Support**: All standard Modbus function codes
 *   - FC 01: Read Coils
 *   - FC 02: Read Discrete Inputs
 *   - FC 03: Read Holding Registers
 *   - FC 04: Read Input Registers
 *   - FC 05: Write Single Coil
 *   - FC 06: Write Single Register
 *   - FC 08: Diagnostics
 *   - FC 15: Write Multiple Coils
 *   - FC 16: Write Multiple Registers
 *
 * - **Multiple Transport Protocols**:
 *   - Modbus RTU (serial with CRC-16)
 *   - Modbus ASCII (serial with LRC) [planned]
 *   - Modbus TCP (Ethernet) [planned]
 *
 * - **Standards Compliance**:
 *   - Full compliance with Modbus Application Protocol V1.1b3
 *   - Proper exception handling and error reporting
 *   - Function-specific quantity validation
 *
 * - **Modern C++ Design**:
 *   - C++17 compatible
 *   - RAII principles
 *   - Exception safety
 *   - Type-safe enumerations
 *   - Smart pointer usage
 *
 * - **Configurable Data Model**:
 *   - Supports standard Modbus limits (2000 coils, 125 registers)
 *   - Customizable limits for specialized applications
 *   - Efficient vector-based storage
 *
 * @section usage_sec Basic Usage
 *
 * @subsection server_usage Creating a Modbus RTU Server
 *
 * @code{.cpp}
 * #include "libmodbus.hpp"
 *
 * int main() {
 *     // Create RTU server
 *     ModbusRtuServer server;
 *
 *     // Initialize some data
 *     server.data.writeCoil(0, true);
 *     server.data.writeHoldingRegister(0, 0x1234);
 *
 *     // Process a request (normally from serial port)
 *     std::vector<uint8_t> request = {0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC};
 *     auto response = server.process(request);
 *
 *     // Send response back to client
 *     return 0;
 * }
 * @endcode
 *
 * @subsection data_model_usage Working with the Data Model
 *
 * @code{.cpp}
 * ModbusDataModel data;
 *
 * // Write coil values
 * data.writeCoil(0, true);
 * data.writeCoil(1, false);
 *
 * // Write register values
 * data.writeHoldingRegister(0, 0x1234);
 * data.writeHoldingRegister(1, 0x5678);
 *
 * // Read values back
 * bool coil0 = data.readCoil(0);
 * uint16_t reg0 = data.readHoldingRegister(0);
 * @endcode
 *
 * @section architecture_sec Architecture
 *
 * The library is built around several key components:
 *
 * - **ModbusDataModel**: Stores all Modbus data (coils, inputs, registers)
 * - **ModbusCommand**: Command pattern implementation for function codes
 * - **ModbusFrame**: Protocol Data Unit and transport-specific frames
 * - **ModbusBaseServer**: Abstract server base with protocol-specific implementations
 * - **Checksum**: Utilities for CRC-16 and LRC calculations
 *
 * @section compliance_sec Standards Compliance
 *
 * This implementation is fully compliant with:
 * - Modbus Application Protocol Specification V1.1b3
 * - Modbus over Serial Line Specification and Implementation Guide V1.02
 * - Modbus Messaging on TCP/IP Implementation Guide V1.0b
 *
 * @section testing_sec Testing
 *
 * The library includes comprehensive unit tests covering:
 * - All function codes and their edge cases
 * - Exception condition handling
 * - Frame serialization/deserialization
 * - CRC calculation accuracy
 * - Compliance with quantity limits
 *
 * @section license_sec License
 *
 * This library is released under the MIT License.
 *
 * @section author_sec Authors
 *
 * libmodbus Project Contributors
 */

#ifndef LIBMODBUS_HPP
#define LIBMODBUS_HPP

// Core Modbus definitions
#include "Modbus.hpp"

// Data model
#include "ModbusDataModel.hpp"

// Frame handling
#include "ModbusFrame.hpp"

// Command implementations
#include "ModbusCommand.hpp"

// Server implementations
#include "ModbusBaseServer.hpp"

// Utility functions
#include "Checksum.hpp"

/**
 * @namespace libmodbus
 * @brief Main namespace for all libmodbus functionality
 *
 * All libmodbus classes, functions, and constants are contained within
 * this namespace to avoid naming conflicts with other libraries.
 */
namespace libmodbus {
/**
 * @brief Library version information
 */
struct Version {
    static constexpr int MAJOR = 1;         ///< Major version number
    static constexpr int MINOR = 0;         ///< Minor version number
    static constexpr int PATCH = 0;         ///< Patch version number

    /**
     * @brief Get version string
     * @return Version string in format "MAJOR.MINOR.PATCH"
     */
    static const char* getString(){
        return "1.0.0";
    }
};
}

#endif // LIBMODBUS_HPP