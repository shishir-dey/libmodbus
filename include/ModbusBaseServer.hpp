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
        commands[ModbusFunctionCode::READ_DISCRETE_INPUTS] = std::make_unique<ReadDiscreteInputCommand>();
        commands[ModbusFunctionCode::READ_HOLDING_REGISTERS] = std::make_unique<ReadHoldingRegisterCommand>();
        commands[ModbusFunctionCode::READ_INPUT_REGISTERS] = std::make_unique<ReadInputRegisterCommand>();
        commands[ModbusFunctionCode::WRITE_SINGLE_COIL] = std::make_unique<WriteCoilCommand>();
        commands[ModbusFunctionCode::WRITE_SINGLE_REGISTER] = std::make_unique<WriteHoldingRegisterCommand>();
    }

    virtual std::vector<uint8_t> process(const std::vector<uint8_t>& requestData)
    {
        return {};
    }
};

class ModbusRtuServer : public ModbusBaseServer {
public:
    std::vector<uint8_t> process(const std::vector<uint8_t>& requestData);
};

#endif
