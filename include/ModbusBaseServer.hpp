#ifndef MODBASESERVER_HPP
#define MODBASESERVER_HPP

#include <map>
#include <memory>

#include "Modbus.hpp"
#include "ModbusCommand.hpp"

class ModbusBaseServer {
public:
ModbusDataModel data;
std::map<ModbusFunctionCode, std::unique_ptr<ModbusCommand> > commands;

ModbusBaseServer()
{
    commands[ModbusFunctionCode::READ_COILS] = std::make_unique<ReadCoilCommand>();
    commands[ModbusFunctionCode::READ_DISCRETE_INPUTS] = std::make_unique<ReadDiscreteInputCommand>();
    commands[ModbusFunctionCode::READ_HOLDING_REGISTERS] =
        std::make_unique<ReadHoldingRegisterCommand>();
    commands[ModbusFunctionCode::READ_INPUT_REGISTERS] = std::make_unique<ReadInputRegisterCommand>();
    commands[ModbusFunctionCode::WRITE_SINGLE_COIL] = std::make_unique<WriteCoilCommand>();
    commands[ModbusFunctionCode::WRITE_SINGLE_REGISTER] =
        std::make_unique<WriteHoldingRegisterCommand>();
    commands[ModbusFunctionCode::WRITE_MULTIPLE_COILS] = std::make_unique<WriteMultipleCoilsCommand>();
    commands[ModbusFunctionCode::WRITE_MULTIPLE_REGISTERS] = std::make_unique<WriteMultipleRegistersCommand>();
    commands[ModbusFunctionCode::DIAGNOSTICS] = std::make_unique<DiagnosticsCommand>();
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