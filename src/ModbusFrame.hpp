#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include "Modbus.hpp"
#include <array>
#include <cstdint>
#include <memory>

class ModbusFrame {
public:
    ModbusFunctionCode functionCode;
    std::array<uint8_t, 256> frameData;

    virtual std::array<uint8_t, 256> serialize() = 0;
    virtual void deserialize(const std::array<uint8_t, 256>& data) = 0;
};

using ModbusRequestFrame = ModbusFrame;
using ModbusResponseFrame = ModbusFrame;

class ModbusExceptionFrame : public ModbusFrame {
public:
    uint8_t slaveaddr;
    ModbusFunctionCode exceptionFunctionCode;
    ModbusExceptionCode exceptionCode;
    uint16_t checksum;

    std::array<uint8_t, 256> serialize();
    void deserialize(const std::array<uint8_t, 256>& data);
};

class ModbusRtuFrame : public ModbusFrame {
public:
    uint8_t slaveaddr;
    uint16_t checksum;

    std::array<uint8_t, 256> serialize() override;
    void deserialize(const std::array<uint8_t, 256>& data) override;
};

class ModbusAsciiFrame : public ModbusFrame {
public:
    uint8_t start;
    uint8_t address;
    uint16_t checksum;
    uint8_t end;

    std::array<uint8_t, 256> serialize() override;
    void deserialize(const std::array<uint8_t, 256>& data) override;
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

class ModbusTcpFrame : public ModbusFrame {
public:
    MbapHeader mbapHeader;

    std::array<uint8_t, 256> serialize() override;
    void deserialize(const std::array<uint8_t, 256>& data) override;
};

class ModbusFrameFactory {
public:
    enum class FrameType {
        RTU,
        ASCII,
        TCPIP
    };

    static std::unique_ptr<ModbusFrame> createFrame(FrameType type)
    {
        switch (type) {
        case FrameType::RTU:
            return std::make_unique<ModbusRtuFrame>();
        case FrameType::ASCII:
            return std::make_unique<ModbusAsciiFrame>();
        case FrameType::TCPIP:
            return std::make_unique<ModbusTcpFrame>();
        }
    }
};

#endif
