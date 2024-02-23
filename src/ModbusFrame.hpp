#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include "Modbus.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <variant>

class ModbusFrame {
public:
    // -----------------------------------------------
    // | Function code (1 byte) | Data (n bytes)  |
    // -----------------------------------------------
    ModbusFunctionCode functionCode;
    std::array<uint8_t, 256> frameData;
};

class ModbusExceptionFrame {
public:
    // ------------------------------------------------
    // | Function code (1 byte) | Exception code (1 byte) |
    // ------------------------------------------------
    ModbusFunctionCode exceptionfunctionCode;
    ModbusExceptionCode exceptionCode;
};

using ModbusRequestFrame = ModbusFrame;
using ModbusResponseFrame = ModbusFrame;

using ModbusPDU = std::variant<ModbusResponseFrame, ModbusExceptionFrame>;

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
    ModbusPDU pdu;
    uint16_t checksum;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
};

using ModbusRtuRequestFrame = ModbusRtuFrame;
using ModbusRtuResponseFrame = ModbusRtuFrame;

class ModbusAsciiFrame {
public:
    // -------------------------------------------------------------------------------------------
    // | Start (1 character) | Slave address (2 characters) | Function code (2 characters) | Data (multiple characters) | LRC check (2 characters) | End (2 characters) |
    // -------------------------------------------------------------------------------------------
    uint8_t start;
    uint8_t address;
    ModbusPDU pdu;
    uint16_t checksum;
    uint16_t end;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
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
    ModbusPDU pdu;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
};

#endif
