#include "Modbus.hpp"
#include "ModbusBaseServer.hpp"
#include "ModbusCommand.hpp"
#include "ModbusDataModel.hpp"

std::array<uint8_t, 256> ModbusRtuServer::handleRequest(const std::array<uint8_t, 256>& requestData)
{
    ModbusRtuRequestFrame request;
    request.deserialize(requestData);

    if (auto* requestRtuFrame = std::get_if<ModbusRtuFrame>(&request.pdu); requestRtuFrame != nullptr) {
        if (commands.find(requestRtuFrame->functionCode) != commands.end()) {
            ModbusPDU resPdu = commands[requestRtuFrame->functionCode]->execute(data, request);
            ModbusRtuResponseFrame rtuRes;
            rtuRes.pdu = resPdu;
            return rtuRes.serialize();
        } else {
            /* Handle invalid command */
        }
    }
}
