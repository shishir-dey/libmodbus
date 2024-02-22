#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include "Modbus.hpp"
#include <array>
#include <cstdint>
#include <memory>
#include <variant>

class ModbusFrame {
public:
    ModbusFunctionCode functionCode;
    std::array<uint8_t, 256> frameData;
};

class ModbusExceptionFrame {
public:
    ModbusFunctionCode exceptionfunctionCode;
    ModbusExceptionCode exceptionFunctionCode;
};

using ModbusRequestFrame = ModbusFrame;
using ModbusResponseFrame = ModbusFrame;

using ModbusPDU = std::variant<ModbusResponseFrame, ModbusExceptionFrame>;

class ModbusRtuFrame {
public:
    ModbusPDU pdu;
    uint8_t slaveaddr;
    uint16_t checksum;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
};

using ModbusRtuRequestFrame = ModbusRtuFrame;
using ModbusRtuResponseFrame = ModbusRtuFrame;

class ModbusAsciiFrame {
public:
    ModbusPDU pdu;
    uint8_t start;
    uint8_t address;
    uint16_t checksum;
    uint8_t end;

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
    ModbusPDU pdu;
    MbapHeader mbapHeader;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
};

#endif
