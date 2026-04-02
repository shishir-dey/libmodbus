//! Modbus RTU frame serialization and deserialization.
//!
//! RTU frames use binary encoding with CRC-16 checksums for error detection,
//! designed for serial communication.
//!
//! # Frame Layout
//! ```text
//! Normal:    | Slave Addr (1) | Function Code (1) | Data (N) | CRC-16 (2) |
//! Exception: | Slave Addr (1) | Exception Function Code (1) | Exception Code (1) | CRC-16 (2) |
//! ```

use crate::checksum;
use crate::error::{ModbusError, Result};
use crate::frame::{FrameType, Pdu};
use crate::protocol::{ExceptionCode, FunctionCode};

/// A complete Modbus RTU frame including slave address and CRC.
///
/// # Examples
/// ```
/// use libmodbuzz::frame::rtu::RtuFrame;
/// use libmodbuzz::frame::{Pdu, FrameType};
/// use libmodbuzz::FunctionCode;
///
/// let pdu = Pdu::new_response(
///     FunctionCode::ReadCoils,
///     vec![0x01, 0xD5],
/// );
/// let frame = RtuFrame::new(0x01, pdu);
/// let bytes = frame.serialize();
/// assert_eq!(bytes[0], 0x01); // slave address
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct RtuFrame {
    /// Slave address (1–247, 0 for broadcast).
    pub slave_address: u8,
    /// The protocol data unit.
    pub pdu: Pdu,
    /// CRC-16 checksum (calculated during serialization).
    pub checksum: u16,
}

impl RtuFrame {
    /// Create a new RTU frame with the given slave address and PDU.
    ///
    /// The checksum field is initialized to `0` and recalculated during
    /// [`serialize`](Self::serialize).
    pub fn new(slave_address: u8, pdu: Pdu) -> Self {
        Self {
            slave_address,
            pdu,
            checksum: 0,
        }
    }

    /// Serialize this RTU frame to a byte vector ready for transmission.
    ///
    /// Automatically calculates and appends the CRC-16 checksum.
    /// The CRC is appended low byte first, then high byte, per the Modbus spec.
    pub fn serialize(&self) -> Vec<u8> {
        let mut output = Vec::new();

        match self.pdu.frame_type {
            FrameType::Request | FrameType::Response => {
                output.push(self.slave_address);
                output.push(u8::from(self.pdu.function_code));

                // Determine how many data bytes to include
                let data_len = if self.pdu.frame_type == FrameType::Response {
                    self.response_data_length()
                } else {
                    self.pdu.data.len()
                };

                // Append data bytes (clamped to actual data length)
                let actual_len = data_len.min(self.pdu.data.len());
                output.extend_from_slice(&self.pdu.data[..actual_len]);
            }
            FrameType::Exception => {
                output.push(self.slave_address);
                // Set MSB of function code for exception
                output.push(u8::from(self.pdu.function_code) | 0x80);
                output.push(self.pdu.exception_code.map(u8::from).unwrap_or(0));
            }
        }

        // Calculate and append CRC (low byte first)
        let crc = checksum::crc16(&output);
        output.push((crc & 0xFF) as u8);
        output.push(((crc >> 8) & 0xFF) as u8);

        output
    }

    /// Deserialize a byte slice into an RTU frame.
    ///
    /// Parses the slave address, function code (including exception detection),
    /// data payload, and CRC. Does **not** validate the CRC — use
    /// [`validate_checksum`](Self::validate_checksum) for that.
    ///
    /// # Arguments
    /// * `frame_type` — Expected frame type (used when the error bit is not set)
    /// * `data` — Raw received bytes including CRC
    ///
    /// # Errors
    /// Returns [`ModbusError::InvalidFrame`] if the data is too short (< 4 bytes).
    pub fn deserialize(frame_type: FrameType, data: &[u8]) -> Result<Self> {
        if data.len() < 4 {
            return Err(ModbusError::InvalidFrame {
                reason: "RTU frame must be at least 4 bytes (addr + FC + CRC)".into(),
                actual_len: data.len(),
            });
        }

        let slave_address = data[0];
        let raw_fc = data[1];

        // Extract CRC from last two bytes (low byte first)
        let crc = u16::from(data[data.len() - 2]) | (u16::from(data[data.len() - 1]) << 8);

        // Check if exception flag (MSB) is set
        if raw_fc & 0x80 != 0 {
            let function_code = FunctionCode::try_from(raw_fc & 0x7F)?;
            let exception_code = if data.len() >= 3 {
                Some(ExceptionCode::try_from(data[2])?)
            } else {
                None
            };

            Ok(Self {
                slave_address,
                pdu: Pdu {
                    frame_type: FrameType::Exception,
                    function_code,
                    data: Vec::new(),
                    exception_code,
                },
                checksum: crc,
            })
        } else {
            let function_code = FunctionCode::try_from(raw_fc)?;

            // Data is everything between FC and CRC
            let payload = if data.len() > 4 {
                data[2..data.len() - 2].to_vec()
            } else {
                Vec::new()
            };

            Ok(Self {
                slave_address,
                pdu: Pdu {
                    frame_type,
                    function_code,
                    data: payload,
                    exception_code: None,
                },
                checksum: crc,
            })
        }
    }

    /// Validate the CRC-16 checksum of received data.
    ///
    /// Computes the CRC over the data (excluding the trailing 2 CRC bytes) and
    /// compares it against the checksum stored in the frame.
    ///
    /// # Arguments
    /// * `raw_data` — The complete raw frame bytes as received (including CRC)
    ///
    /// # Errors
    /// Returns [`ModbusError::ChecksumMismatch`] if the CRC doesn't match.
    pub fn validate_checksum(&self, raw_data: &[u8]) -> Result<()> {
        if raw_data.len() < 4 {
            return Err(ModbusError::InvalidFrame {
                reason: "data too short for CRC validation".into(),
                actual_len: raw_data.len(),
            });
        }

        let data_without_crc = &raw_data[..raw_data.len() - 2];
        let expected = checksum::crc16(data_without_crc);

        if expected != self.checksum {
            return Err(ModbusError::ChecksumMismatch {
                expected,
                actual: self.checksum,
            });
        }

        Ok(())
    }

    /// Determine the response data length based on function code.
    ///
    /// For read responses, the first data byte is the byte count, so total
    /// payload length is byte_count + 1. For write/diagnostics responses,
    /// the data is a fixed 4 bytes (echoed address + quantity).
    fn response_data_length(&self) -> usize {
        if self.pdu.data.is_empty() {
            return 0;
        }

        match self.pdu.function_code {
            FunctionCode::ReadCoils
            | FunctionCode::ReadDiscreteInputs
            | FunctionCode::ReadHoldingRegisters
            | FunctionCode::ReadInputRegisters => {
                // First byte is byte count
                usize::from(self.pdu.data[0]) + 1
            }
            FunctionCode::WriteSingleCoil
            | FunctionCode::WriteSingleRegister
            | FunctionCode::WriteMultipleCoils
            | FunctionCode::WriteMultipleRegisters
            | FunctionCode::Diagnostics => 4,
            _ => self.pdu.data.len(),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn serialize_request() {
        let pdu = Pdu::new_request(FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
        let frame = RtuFrame::new(0x01, pdu);
        let bytes = frame.serialize();
        assert_eq!(bytes, vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC]);
    }

    #[test]
    fn serialize_exception() {
        let pdu = Pdu::new_exception(FunctionCode::ReadCoils, ExceptionCode::IllegalFunction);
        let frame = RtuFrame::new(0x01, pdu);
        let bytes = frame.serialize();
        assert_eq!(bytes[0], 0x01); // slave addr
        assert_eq!(bytes[1], 0x81); // FC | 0x80
        assert_eq!(bytes[2], 0x01); // exception code
        assert_eq!(bytes.len(), 5); // addr + fc + exc + crc(2)
    }

    #[test]
    fn deserialize_request() {
        let data = [0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
        let frame = RtuFrame::deserialize(FrameType::Request, &data).unwrap();
        assert_eq!(frame.slave_address, 0x01);
        assert_eq!(frame.pdu.function_code, FunctionCode::ReadCoils);
        assert_eq!(frame.pdu.data, vec![0x00, 0x00, 0x00, 0x08]);
        assert_eq!(frame.checksum, 0xCC3D);
    }

    #[test]
    fn deserialize_too_short() {
        let data = [0x01, 0x01];
        assert!(RtuFrame::deserialize(FrameType::Request, &data).is_err());
    }

    #[test]
    fn crc_validation_pass() {
        let data = [0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
        let frame = RtuFrame::deserialize(FrameType::Request, &data).unwrap();
        assert!(frame.validate_checksum(&data).is_ok());
    }

    #[test]
    fn crc_validation_fail() {
        let data = [0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00];
        let frame = RtuFrame::deserialize(FrameType::Request, &data).unwrap();
        assert!(frame.validate_checksum(&data).is_err());
    }

    #[test]
    fn round_trip() {
        let original: Vec<u8> = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
        let frame = RtuFrame::deserialize(FrameType::Request, &original).unwrap();
        let reserialized = frame.serialize();
        assert_eq!(original, reserialized);
    }
}
