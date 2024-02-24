#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::array<uint8_t, 256> ModbusRtuServer::handleRequest(const std::array<uint8_t, 256>& requestData)
{
    ModbusRtuFrame rtuRequest, rtuResponse;

    rtuRequest.deserialize(ModbusFrameType::REQUEST, requestData);
    rtuResponse.slaveaddr = rtuRequest.slaveaddr;
    rtuResponse.pdu = commands[rtuRequest.pdu.functionCode]->execute(data, rtuRequest.pdu);
    rtuResponse.checksum = 0;

    return rtuResponse.serialize();
}
