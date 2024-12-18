#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include "Modbus.hpp"
#include <cstdint>
#include <memory>
#include <variant>
#include <vector>

enum class ModbusFrameType {
    NONE,
    REQUEST,
    RESPONSE,
    EXCEPTION
};

class ModbusFrame {
public:
    ModbusFrameType frameType;
    ModbusFunctionCode functionCode;
    std::vector<uint8_t> frameData;
    ModbusExceptionCode exceptionCode;
};

class ModbusRtuFrame {
public:
    // --------------------------------------------------------
    // | Slave address (1 byte) | Function code (1 byte) | Data (n bytes) | CRC check (2 bytes) |
    // --------------------------------------------------------
    // or
    // --------------------------------------------------------
    // | Slave address (1 byte) | Function code (1 byte) | Exception code (1 byte) | CRC check (2 bytes) |
    // --------------------------------------------------------
    uint8_t slaveaddr;
    ModbusFrame pdu;
    uint16_t checksum;

    std::vector<uint8_t> serialize();
    void deserialize(ModbusFrameType frameType, const std::vector<uint8_t>& data);
};

class ModbusAsciiFrame {
public:
    // -------------------------------------------------------------------------------------------
    // | Start (1 character) | Slave address (2 characters) | Function code (2 characters) | Data (multiple characters) | LRC check (2 characters) | End (2 characters) |
    // -------------------------------------------------------------------------------------------
    uint8_t start;
    uint8_t address;
    ModbusFrame pdu;
    uint16_t checksum;
    uint16_t end;

    std::vector<uint8_t> serialize();
    void deserialize(const std::vector<uint8_t>& data);
};

class MbapHeader {
public:
    uint16_t transactionId;
    uint16_t protocolId;
    uint16_t length;
    uint8_t unitId;

    MbapHeader()
        : transactionId(0)
        , protocolId(0)
        , length(0)
        , unitId(0)
    {
    }

    MbapHeader(uint16_t transactionId, uint16_t protocolId, uint16_t length, uint8_t unitId)
        : transactionId(transactionId)
        , protocolId(protocolId)
        , length(length)
        , unitId(unitId)
    {
    }
};

class ModbusTcpFrame {
public:
    MbapHeader mbapHeader;
    ModbusFrame pdu;

    std::vector<uint8_t> serialize();
    void deserialize(const std::vector<uint8_t>& data);
};

#endif
