#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::array<uint8_t, 256> ModbusRtuServer::handleRequest(const std::array<uint8_t, 256>& requestData)
{
    ModbusRtuRequestFrame request;
    request.deserialize(requestData);

    if (commands.find(request.functionCode) != commands.end()) {
        ModbusPDU res = commands[request.functionCode]->execute(data, request);
        if (res.index() == 0) {
            ModbusResponseFrame response = std::get<ModbusResponseFrame>(res);
            return response.serialize();
        } else {
            ModbusExceptionFrame exception = std::get<ModbusExceptionFrame>(res);
            return exception.serialize();
        }
    } else {

        ModbusExceptionFrame exception;
        exception.exceptionCode = ModbusExceptionCode::ILLEGAL_FUNCTION;
        return exception.serialize();
    }
}
