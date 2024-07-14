#include "Checksum.hpp"

uint8_t Checksum::calculateLRC(const uint8_t* pucFrame, uint16_t usLen)
{
    uint8_t ucLRC = 0;
    while (usLen--) {
        ucLRC += *pucFrame++;
    }
    return static_cast<uint8_t>(-static_cast<int8_t>(ucLRC));
}

uint16_t Checksum::calculateCRC16(const uint8_t* buffer, uint16_t length)
{
    uint16_t cur_crc = 0xFFFF;
    do {
        uint8_t i = 8;
        cur_crc = cur_crc ^ *buffer++;
        do {
            if (cur_crc & 0x0001) {
                cur_crc >>= 1;
                cur_crc ^= 0xA001;
            } else {
                cur_crc >>= 1;
            }
        } while (--i);
    } while (--length);
    return cur_crc;
}