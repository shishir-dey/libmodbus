#ifndef CHECKSUM_HPP
#define CHECKSUM_HPP

#include <cstdint>

class Checksum {
public:
static uint8_t calculateLRC(const uint8_t* pucFrame, uint16_t usLen);
static uint16_t calculateCRC16(const uint8_t* buffer, uint16_t length);
};

#endif