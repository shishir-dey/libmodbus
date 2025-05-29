/**
 * @file ModbusFrame.hpp
 * @brief Modbus frame implementations for RTU, ASCII, and TCP protocols
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file contains the implementation of Modbus frame structures for all
 * three main Modbus variants: RTU (serial), ASCII (serial), and TCP (Ethernet).
 * Each frame type handles the specific formatting, serialization, and
 * deserialization requirements of its respective protocol.
 */

#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include <cstdint>
#include <memory>
#include <variant>
#include <vector>

#include "Modbus.hpp"

/**
 * @brief Enumeration of Modbus frame types
 *
 * This enumeration identifies the type of Modbus frame being processed,
 * which determines how the frame should be interpreted and what response
 * should be generated.
 */
enum class ModbusFrameType {
    NONE,       ///< Uninitialized or invalid frame
    REQUEST,    ///< Request frame from client to server
    RESPONSE,   ///< Normal response frame from server to client
    EXCEPTION   ///< Exception response frame indicating an error
};

/**
 * @class ModbusFrame
 * @brief Core Modbus Protocol Data Unit (PDU) implementation
 *
 * This class represents the core Modbus PDU that is common to all Modbus
 * variants (RTU, ASCII, TCP). It contains the function code, frame data,
 * and exception information. The PDU is embedded within the various
 * transport-specific frame formats.
 *
 * The frame structure is:
 * - Function Code (1 byte)
 * - Data (0-252 bytes)
 * - Exception Code (1 byte, only for exception frames)
 */
class ModbusFrame {
public:
ModbusFrameType frameType;      ///< Type of frame (request/response/exception)
ModbusFunctionCode functionCode; ///< Modbus function code
std::vector<uint8_t> frameData; ///< Frame data payload
ModbusExceptionCode exceptionCode; ///< Exception code (only used for exception frames)

/**
 * @brief Default constructor
 *
 * Creates an uninitialized frame with NONE type, NONE function code,
 * and no exception code.
 */
ModbusFrame()
    : frameType(ModbusFrameType::NONE)
    , functionCode(ModbusFunctionCode::NONE)
    , exceptionCode(ModbusExceptionCode::NONE)
{}

/**
 * @brief Constructor with frame type and function code
 *
 * @param type The type of frame (REQUEST, RESPONSE, or EXCEPTION)
 * @param func The Modbus function code
 */
ModbusFrame(ModbusFrameType type, ModbusFunctionCode func)
    : frameType(type)
    , functionCode(func)
    , exceptionCode(ModbusExceptionCode::NONE)
{}
};

/**
 * @class ModbusRtuFrame
 * @brief Modbus RTU (Remote Terminal Unit) frame implementation
 *
 * Implements the Modbus RTU frame format used for serial communication.
 * RTU frames use binary encoding and CRC-16 checksums for error detection.
 *
 * Normal frame structure:
 * | Slave Address (1 byte) | Function Code (1 byte) | Data (n bytes) | CRC (2 bytes) |
 *
 * Exception frame structure:
 * | Slave Address (1 byte) | Function Code + 0x80 (1 byte) | Exception Code (1 byte) | CRC (2 bytes) |
 *
 * @note The CRC is calculated using the Modbus CRC-16 algorithm over all bytes
 * except the CRC itself, with low byte transmitted first.
 */
class ModbusRtuFrame {
public:
// --------------------------------------------------------
// | Slave address (1 byte) | Function code (1 byte) | Data (n bytes) | CRC check (2 bytes) |
// --------------------------------------------------------
// or
// --------------------------------------------------------
// | Slave address (1 byte) | Function code (1 byte) | Exception code (1 byte) | CRC check (2 bytes) |
// --------------------------------------------------------
uint8_t slaveaddr;    ///< Slave address (1-247, 0 for broadcast)
ModbusFrame pdu;      ///< Protocol Data Unit
uint16_t checksum;    ///< CRC-16 checksum

/**
 * @brief Default constructor
 *
 * Creates an RTU frame with slave address 0 and checksum 0.
 */
ModbusRtuFrame() : slaveaddr(0), checksum(0){}

/**
 * @brief Constructor with slave address
 *
 * @param addr Slave address (1-247, 0 for broadcast)
 */
ModbusRtuFrame(uint8_t addr) : slaveaddr(addr), checksum(0){}

/**
 * @brief Serialize the RTU frame to a byte vector
 *
 * Converts the RTU frame to its binary representation suitable for
 * transmission over a serial link. The function automatically calculates
 * and appends the CRC-16 checksum.
 *
 * @return Vector of bytes representing the serialized frame
 *
 * @note The CRC is calculated using the Modbus CRC-16 algorithm
 */
std::vector<uint8_t> serialize();

/**
 * @brief Deserialize a byte vector into an RTU frame
 *
 * Parses a received byte vector and populates the RTU frame fields.
 * The function extracts the slave address, function code, data, and CRC.
 *
 * @param frameType Expected frame type (REQUEST, RESPONSE, or EXCEPTION)
 * @param data Byte vector containing the raw frame data
 *
 * @note This function does not validate the CRC - that should be done
 * by the caller before calling this function.
 */
void deserialize(ModbusFrameType frameType, const std::vector<uint8_t>& data);
};

/**
 * @class ModbusAsciiFrame
 * @brief Modbus ASCII frame implementation
 *
 * Implements the Modbus ASCII frame format used for serial communication.
 * ASCII frames use hexadecimal character encoding and LRC checksums.
 *
 * Frame structure:
 * | Start ':' | Address (2 hex chars) | Function (2 hex chars) | Data (hex chars) | LRC (2 hex chars) | End CRLF |
 *
 * @note ASCII frames are longer than RTU frames but are human-readable
 * and less susceptible to transmission errors.
 *
 * @warning ASCII implementation is currently incomplete (TODO)
 */
class ModbusAsciiFrame {
public:
// -------------------------------------------------------------------------------------------
// | Start (1 character) | Slave address (2 characters) | Function code (2 characters) | Data (multiple characters) | LRC check (2 characters) | End (2 characters) |
// -------------------------------------------------------------------------------------------
uint8_t start;        ///< Start character (always ':')
uint8_t address;      ///< Slave address
ModbusFrame pdu;      ///< Protocol Data Unit
uint16_t checksum;    ///< LRC checksum
uint16_t end;         ///< End characters (CR LF)

/**
 * @brief Serialize the ASCII frame to a byte vector
 *
 * @return Vector of bytes representing the serialized frame
 * @todo This method is not yet implemented
 */
std::vector<uint8_t> serialize();

/**
 * @brief Deserialize a byte vector into an ASCII frame
 *
 * @param data Byte vector containing the raw frame data
 * @todo This method is not yet implemented
 */
void deserialize(const std::vector<uint8_t>& data);
};

/**
 * @class MbapHeader
 * @brief Modbus Application Protocol (MBAP) header for TCP frames
 *
 * The MBAP header is used in Modbus TCP to provide transaction identification,
 * protocol identification, length information, and unit identification.
 *
 * Header structure (7 bytes):
 * | Transaction ID (2 bytes) | Protocol ID (2 bytes) | Length (2 bytes) | Unit ID (1 byte) |
 */
class MbapHeader {
public:
uint16_t transactionId; ///< Transaction identifier for matching requests/responses
uint16_t protocolId;    ///< Protocol identifier (always 0 for Modbus)
uint16_t length;        ///< Number of following bytes (PDU length + 1)
uint8_t unitId;         ///< Unit identifier (slave address equivalent)

/**
 * @brief Default constructor
 *
 * Creates an MBAP header with all fields set to 0.
 */
MbapHeader()
    : transactionId(0)
    , protocolId(0)
    , length(0)
    , unitId(0)
{}

/**
 * @brief Constructor with all header fields
 *
 * @param transactionId Transaction identifier
 * @param protocolId Protocol identifier (should be 0 for Modbus)
 * @param length Length of following bytes
 * @param unitId Unit identifier
 */
MbapHeader(uint16_t transactionId, uint16_t protocolId, uint16_t length, uint8_t unitId)
    : transactionId(transactionId)
    , protocolId(protocolId)
    , length(length)
    , unitId(unitId)
{}
};

/**
 * @class ModbusTcpFrame
 * @brief Modbus TCP frame implementation
 *
 * Implements the Modbus TCP frame format used for Ethernet communication.
 * TCP frames include an MBAP header followed by the PDU. No checksum is
 * needed as TCP provides reliable delivery.
 *
 * Frame structure:
 * | MBAP Header (7 bytes) | PDU (1-253 bytes) |
 *
 * @warning TCP implementation is currently incomplete (TODO)
 */
class ModbusTcpFrame {
public:
MbapHeader mbapHeader; ///< MBAP header
ModbusFrame pdu;       ///< Protocol Data Unit

/**
 * @brief Serialize the TCP frame to a byte vector
 *
 * @return Vector of bytes representing the serialized frame
 * @todo This method is not yet implemented
 */
std::vector<uint8_t> serialize();

/**
 * @brief Deserialize a byte vector into a TCP frame
 *
 * @param data Byte vector containing the raw frame data
 * @todo This method is not yet implemented
 */
void deserialize(const std::vector<uint8_t>& data);
};

#endif