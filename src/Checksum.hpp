#ifndef CHECKSUM_HPP
#define CHECKSUM_HPP

class Checksum {
public:
    static unsigned char calculateLRC(const unsigned char* pucFrame, unsigned short usLen);
    static unsigned short calculateCRC16(const unsigned char* buffer, unsigned short length);
};

#endif
