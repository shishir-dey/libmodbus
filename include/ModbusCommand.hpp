#ifndef MODBUSCOMMAND_HPP
#define MODBUSCOMMAND_HPP

#include "ModbusDataModel.hpp"
#include "ModbusFrame.hpp"

class ModbusCommand {
public:
virtual ~ModbusCommand() = default;
virtual ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request) = 0;

protected:
bool validateQuantity(
    const ModbusFrame& request,
    uint16_t minQuantity,
    uint16_t maxQuantity,
    ModbusFrame& response
);
bool validateAddress(const ModbusFrame& request, uint16_t maxValidAddress, ModbusFrame& response);

// Function-specific quantity validation
bool validateReadCoilsQuantity(const ModbusFrame& request, ModbusFrame& response);
bool validateReadRegistersQuantity(const ModbusFrame& request, ModbusFrame& response);
bool validateWriteMultipleCoilsQuantity(const ModbusFrame& request, ModbusFrame& response);
bool validateWriteMultipleRegistersQuantity(const ModbusFrame& request, ModbusFrame& response);
};

class ReadCoilCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class ReadDiscreteInputCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class ReadInputRegisterCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class ReadHoldingRegisterCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class WriteCoilCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class WriteHoldingRegisterCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class WriteMultipleCoilsCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class WriteMultipleRegistersCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

class DiagnosticsCommand : public ModbusCommand {
public:
ModbusFrame execute(ModbusDataModel& data, const ModbusFrame& request);
};

#endif