/**
 * @file Checksum.hpp
 * @brief Checksum calculation utilities for Modbus protocols
 * @author libmodbus Project
 * @version 1.0
 * @date 2024
 *
 * This file provides checksum calculation functions for different Modbus
 * transport protocols. It includes implementations for:
 * - CRC-16 (used by Modbus RTU)
 * - LRC (Longitudinal Redundancy Check, used by Modbus ASCII)
 */

#ifndef CHECKSUM_HPP
#define CHECKSUM_HPP

#include <cstdint>

/**
 * @class Checksum
 * @brief Static utility class for Modbus checksum calculations
 *
 * This class provides static methods for calculating checksums used in
 * different Modbus transport protocols. The checksums are essential for
 * error detection in serial communications where data corruption can occur.
 *
 * @note All methods are static and the class is not meant to be instantiated.
 */
class Checksum {
public:
/**
 * @brief Calculate LRC (Longitudinal Redundancy Check) for Modbus ASCII
 *
 * Calculates the LRC checksum used by Modbus ASCII protocol. The LRC is
 * computed by adding all bytes in the message and taking the two's complement
 * of the result.
 *
 * Algorithm:
 * 1. Add all bytes in the frame (excluding the LRC itself)
 * 2. Take the two's complement of the sum
 * 3. Return the result as a single byte
 *
 * @param pucFrame Pointer to the frame data
 * @param usLen Length of the frame data in bytes
 * @return LRC checksum value (8-bit)
 *
 * @note The LRC is calculated over the binary representation of the ASCII
 * characters, not the ASCII characters themselves.
 */
static uint8_t calculateLRC(const uint8_t* pucFrame, uint16_t usLen);

/**
 * @brief Calculate CRC-16 checksum for Modbus RTU
 *
 * Calculates the CRC-16 checksum used by Modbus RTU protocol. This implementation
 * uses the standard Modbus CRC-16 algorithm with polynomial 0xA001 (reversed
 * representation of 0x8005).
 *
 * Algorithm:
 * 1. Initialize CRC register to 0xFFFF
 * 2. For each byte:
 *    a. XOR byte with low byte of CRC register
 *    b. For each bit (8 iterations):
 *       - If LSB of CRC is 1: shift right and XOR with 0xA001
 *       - If LSB of CRC is 0: shift right only
 * 3. Return final CRC value
 *
 * @param buffer Pointer to the data buffer
 * @param length Length of the data buffer in bytes
 * @return CRC-16 checksum value (16-bit)
 *
 * @note The CRC is transmitted LSB first in RTU frames. This function returns
 * the CRC in native byte order; the caller is responsible for proper byte ordering.
 *
 * @note This is the standard Modbus CRC-16 algorithm as specified in the
 * Modbus over Serial Line Specification and Implementation Guide V1.02.
 */
static uint16_t calculateCRC16(const uint8_t* buffer, uint16_t length);
};

#endif