#ifndef MODBUSCOMMAND_HPP
#define MODBUSCOMMAND_HPP

#include "ModbusDataModel.hpp"
#include "ModbusFrame.hpp"

class ModbusCommand {
public:
    virtual ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request) = 0;
};

class ReadCoilCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

class ReadDiscreteInputCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

class ReadInputRegisterCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

class ReadHoldingRegisterCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

class WriteCoilCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

class WriteHoldingRegisterCommand : public ModbusCommand {
public:
    ModbusPDU execute(ModbusDataModel& data, const ModbusRequestFrame& request);
};

#endif
