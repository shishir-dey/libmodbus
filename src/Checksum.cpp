#include "Checksum.h"

class Checksum {
public:
    /* LRC calculation */
    static unsigned char calculateLRC(const unsigned char* pucFrame, unsigned short usLen)
    {
        unsigned char ucLRC = 0;
        while (usLen--) {
            ucLRC += *pucFrame++;
        }
        // Return twos complement
        return (unsigned char)-((char)ucLRC);
    }

    /* CRC16 calculation with Modbus polynomial and initial value */
    static unsigned short calculateCRC16(const unsigned char* buffer, unsigned short length)
    {
        unsigned int cur_crc = 0xFFFF;
        do {
            unsigned int i = 8;
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
};
