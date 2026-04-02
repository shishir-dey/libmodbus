//! Checksum calculation utilities for Modbus protocols.
//!
//! Provides checksum functions for different Modbus transport protocols:
//! - **CRC-16** — used by Modbus RTU (polynomial 0xA001, init 0xFFFF)
//! - **LRC** — Longitudinal Redundancy Check, used by Modbus ASCII
//!
//! The crate ships RTU and TCP transports today. [`lrc`] is included as a
//! protocol utility for ASCII-compatible tooling and future transport support.
//!
//! # Examples
//! ```
//! use libmodbuzz::checksum;
//!
//! // CRC-16 for a typical Modbus RTU request
//! let data = [0x01, 0x01, 0x00, 0x00, 0x00, 0x08];
//! let crc = checksum::crc16(&data);
//! assert_eq!(crc, 0xCC3D); // LSB first in wire format: 0x3D, 0xCC
//! ```

/// Calculate CRC-16 checksum for Modbus RTU.
///
/// Uses the standard Modbus CRC-16 algorithm with polynomial 0xA001
/// (bit-reversed 0x8005), initialized to 0xFFFF.
///
/// The returned value is in native byte order. When transmitting on the wire,
/// the **low byte is sent first**, followed by the high byte.
///
/// # Arguments
/// * `data` — byte slice to compute the CRC over (excludes the CRC bytes themselves)
///
/// # Panics
/// Does not panic. Returns 0xFFFF for empty input (which is the CRC init value).
pub fn crc16(data: &[u8]) -> u16 {
    let mut crc: u16 = 0xFFFF;

    for &byte in data {
        crc ^= u16::from(byte);
        for _ in 0..8 {
            if crc & 0x0001 != 0 {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    crc
}

/// Calculate LRC (Longitudinal Redundancy Check) for Modbus ASCII.
///
/// Computes the LRC by summing all bytes and returning the two's complement
/// of the result. This is used by the Modbus ASCII transport protocol and by
/// tooling that needs ASCII-compatible checksum generation.
///
/// # Arguments
/// * `data` — byte slice to compute the LRC over
pub fn lrc(data: &[u8]) -> u8 {
    let sum: u8 = data.iter().fold(0u8, |acc, &b| acc.wrapping_add(b));
    (-(sum as i8)) as u8
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn crc16_read_coils_request() {
        // Slave 1, FC 01, start=0x0000, qty=0x0008
        let data = [0x01, 0x01, 0x00, 0x00, 0x00, 0x08];
        let crc = crc16(&data);
        // Expected CRC: wire bytes 0x3D, 0xCC → value 0xCC3D
        assert_eq!(crc, 0xCC3D);
    }

    #[test]
    fn crc16_read_holding_registers_request() {
        // Slave 1, FC 03, start=0x0000, qty=0x0002
        let data = [0x01, 0x03, 0x00, 0x00, 0x00, 0x02];
        let crc = crc16(&data);
        assert_eq!(crc, 0x0BC4);
    }

    #[test]
    fn crc16_read_input_registers_request() {
        // Slave 1, FC 04, start=0x0000, qty=0x0002
        let data = [0x01, 0x04, 0x00, 0x00, 0x00, 0x02];
        let crc = crc16(&data);
        assert_eq!(crc, 0xCB71);
    }

    #[test]
    fn crc16_empty_data() {
        assert_eq!(crc16(&[]), 0xFFFF);
    }

    #[test]
    fn crc16_single_byte() {
        let crc = crc16(&[0x01]);
        // Manually verified
        assert_eq!(crc, 0x807E);
    }

    #[test]
    fn lrc_basic() {
        // Example: bytes 0x01 + 0x03 + 0x00 + 0x00 + 0x00 + 0x0A = 0x0E
        // LRC = two's complement = 0xF2
        let data = [0x01, 0x03, 0x00, 0x00, 0x00, 0x0A];
        assert_eq!(lrc(&data), 0xF2);
    }

    #[test]
    fn lrc_empty() {
        assert_eq!(lrc(&[]), 0x00);
    }
}
