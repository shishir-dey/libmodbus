#ifndef MODBUSFRAME_HPP
#define MODBUSFRAME_HPP

#include <array>
#include <cstdint>

class ModbusFrame {
public:
    virtual ~ModbusFrame() = default;
    virtual std::array<uint8_t, 256> serialize() = 0;
    virtual void deserialize(const std::array<uint8_t, 256>& data) = 0;
};

#endif
