#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::vector<uint8_t> ModbusRtuServer::process(const std::vector<uint8_t>& requestData)
{
    ModbusRtuFrame request, response;
    request.deserialize(ModbusFrameType::REQUEST, requestData);

    response.slaveaddr = request.slaveaddr;
    response.pdu = commands[request.pdu.functionCode]->execute(data, request.pdu);
    response.checksum = 0;

    return response.serialize();
}
