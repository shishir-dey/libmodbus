#ifndef MODBASESERVER_HPP
#define MODBASESERVER_HPP

#include "Modbus.hpp"
#include "ModbusCommand.hpp"
#include <map>

class ModbusBaseServer {
public:
    ModbusDataModel data;
    std::map<ModbusFunctionCode, ModbusCommand*> commands;

    ModbusBaseServer()
    {
        commands[ModbusFunctionCode::READ_COILS] = new ReadCoilCommand();
    }

    ~ModbusBaseServer()
    {
        for (auto& command : commands) {
            delete command.second;
        }
    }

    virtual std::array<uint8_t, 256> handleRequest(const std::array<uint8_t, 256>& requestData)
    {
    }
};

class ModbusRtuServer : public ModbusBaseServer {
public:
    std::array<uint8_t, 256> handleRequest(const std::array<uint8_t, 256>& requestData);
};

#endif
