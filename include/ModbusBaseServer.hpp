#ifndef MODBASESERVER_HPP
#define MODBASESERVER_HPP

#include "Modbus.hpp"
#include "ModbusCommand.hpp"
#include <map>
#include <memory>

class ModbusBaseServer {
public:
    ModbusDataModel data;
    std::map<ModbusFunctionCode, std::unique_ptr<ModbusCommand>> commands;

    ModbusBaseServer()
    {
        commands[ModbusFunctionCode::READ_COILS] = std::make_unique<ReadCoilCommand>();
    }

    virtual std::array<uint8_t, 256> process(const std::array<uint8_t, 256>& requestData)
    {
        return {};
    }
};

class ModbusRtuServer : public ModbusBaseServer {
public:
    std::array<uint8_t, 256> process(const std::array<uint8_t, 256>& requestData);
};

#endif
