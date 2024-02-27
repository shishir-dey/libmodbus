#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::array<uint8_t, 256> ModbusRtuServer::process(const std::array<uint8_t, 256>& requestData)
{
    ModbusRtuFrame request, response;

    request.deserialize(ModbusFrameType::REQUEST, requestData);
    response.slaveaddr = request.slaveaddr;
    response.pdu = commands[request.pdu.functionCode]->execute(data, request.pdu);
    response.checksum = 0;

    return response.serialize();
}
