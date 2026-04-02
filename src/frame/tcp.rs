//! Modbus TCP frame serialization and deserialization.
//!
//! TCP frames use the MBAP (Modbus Application Protocol) header followed by the
//! PDU. No checksum is needed since TCP provides reliable delivery.
//!
//! # Frame Layout
//! ```text
//! | Transaction ID (2) | Protocol ID (2) | Length (2) | Unit ID (1) | PDU (N) |
//! ```

use crate::error::{ModbusError, Result};
use crate::frame::{FrameType, Pdu};
use crate::protocol::{ExceptionCode, FunctionCode};

/// MBAP (Modbus Application Protocol) header for TCP frames.
///
/// The 7-byte header provides transaction tracking, protocol identification,
/// length information, and unit identification.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct MbapHeader {
    /// Transaction identifier for matching requests to responses.
    pub transaction_id: u16,
    /// Protocol identifier (always 0x0000 for Modbus).
    pub protocol_id: u16,
    /// Number of following bytes (unit ID + PDU length).
    pub length: u16,
    /// Unit identifier (equivalent to slave address in RTU).
    pub unit_id: u8,
}

impl MbapHeader {
    /// MBAP header size in bytes.
    pub const SIZE: usize = 7;

    /// Create a new MBAP header.
    pub fn new(transaction_id: u16, unit_id: u8, pdu_length: u16) -> Self {
        Self {
            transaction_id,
            protocol_id: 0x0000,
            length: pdu_length + 1, // +1 for unit_id byte
            unit_id,
        }
    }

    /// Serialize the MBAP header to bytes (big-endian).
    pub fn serialize(&self) -> [u8; 7] {
        let mut buf = [0u8; 7];
        buf[0] = (self.transaction_id >> 8) as u8;
        buf[1] = (self.transaction_id & 0xFF) as u8;
        buf[2] = (self.protocol_id >> 8) as u8;
        buf[3] = (self.protocol_id & 0xFF) as u8;
        buf[4] = (self.length >> 8) as u8;
        buf[5] = (self.length & 0xFF) as u8;
        buf[6] = self.unit_id;
        buf
    }

    /// Deserialize an MBAP header from a byte slice.
    ///
    /// # Errors
    /// Returns [`ModbusError::InvalidFrame`] if the slice is shorter than 7 bytes
    /// or the protocol ID is not 0x0000.
    pub fn deserialize(data: &[u8]) -> Result<Self> {
        if data.len() < Self::SIZE {
            return Err(ModbusError::InvalidFrame {
                reason: format!("MBAP header requires {} bytes", Self::SIZE),
                actual_len: data.len(),
            });
        }

        let protocol_id = u16::from(data[2]) << 8 | u16::from(data[3]);
        if protocol_id != 0x0000 {
            return Err(ModbusError::InvalidFrame {
                reason: format!(
                    "invalid protocol ID: expected 0x0000, got 0x{:04X}",
                    protocol_id
                ),
                actual_len: data.len(),
            });
        }

        Ok(Self {
            transaction_id: u16::from(data[0]) << 8 | u16::from(data[1]),
            protocol_id,
            length: u16::from(data[4]) << 8 | u16::from(data[5]),
            unit_id: data[6],
        })
    }
}

/// A complete Modbus TCP frame (MBAP header + PDU).
///
/// # Examples
/// ```
/// use libmodbuzz::frame::tcp::TcpFrame;
/// use libmodbuzz::frame::Pdu;
/// use libmodbuzz::FunctionCode;
///
/// let pdu = Pdu::new_request(
///     FunctionCode::ReadHoldingRegisters,
///     vec![0x00, 0x00, 0x00, 0x0A],
/// );
/// let frame = TcpFrame::new(1, 0x01, pdu);
/// let bytes = frame.serialize();
/// assert_eq!(bytes.len(), 7 + 1 + 4); // MBAP + FC + data
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct TcpFrame {
    /// The MBAP header.
    pub header: MbapHeader,
    /// The protocol data unit.
    pub pdu: Pdu,
}

impl TcpFrame {
    /// Create a new TCP frame.
    ///
    /// Automatically calculates the MBAP `length` field from the PDU.
    pub fn new(transaction_id: u16, unit_id: u8, pdu: Pdu) -> Self {
        let pdu_wire_len = match pdu.frame_type {
            FrameType::Exception => 2, // FC + exception code
            _ => 1 + pdu.data.len(),   // FC + data
        };

        Self {
            header: MbapHeader::new(transaction_id, unit_id, pdu_wire_len as u16),
            pdu,
        }
    }

    /// Serialize the TCP frame to a byte vector.
    pub fn serialize(&self) -> Vec<u8> {
        let mut output = Vec::with_capacity(MbapHeader::SIZE + 1 + self.pdu.data.len());
        output.extend_from_slice(&self.header.serialize());

        match self.pdu.frame_type {
            FrameType::Exception => {
                output.push(u8::from(self.pdu.function_code) | 0x80);
                output.push(self.pdu.exception_code.map(u8::from).unwrap_or(0));
            }
            _ => {
                output.push(u8::from(self.pdu.function_code));
                output.extend_from_slice(&self.pdu.data);
            }
        }

        output
    }

    /// Deserialize a byte slice into a TCP frame.
    ///
    /// # Errors
    /// Returns [`ModbusError::InvalidFrame`] if the data is too short or malformed.
    pub fn deserialize(data: &[u8]) -> Result<Self> {
        let header = MbapHeader::deserialize(data)?;

        let pdu_start = MbapHeader::SIZE;
        if data.len() < pdu_start + 1 {
            return Err(ModbusError::InvalidFrame {
                reason: "TCP frame has no PDU after MBAP header".into(),
                actual_len: data.len(),
            });
        }

        let raw_fc = data[pdu_start];

        if raw_fc & 0x80 != 0 {
            // Exception response
            let function_code = FunctionCode::try_from(raw_fc & 0x7F)?;
            let exception_code = if data.len() > pdu_start + 1 {
                Some(ExceptionCode::try_from(data[pdu_start + 1])?)
            } else {
                None
            };

            Ok(Self {
                header,
                pdu: Pdu {
                    frame_type: FrameType::Exception,
                    function_code,
                    data: Vec::new(),
                    exception_code,
                },
            })
        } else {
            let function_code = FunctionCode::try_from(raw_fc)?;
            let payload = if data.len() > pdu_start + 1 {
                data[pdu_start + 1..].to_vec()
            } else {
                Vec::new()
            };

            Ok(Self {
                header,
                pdu: Pdu {
                    frame_type: FrameType::Response, // Inferred; caller can override
                    function_code,
                    data: payload,
                    exception_code: None,
                },
            })
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn mbap_header_round_trip() {
        let header = MbapHeader::new(0x0001, 0x01, 6);
        let bytes = header.serialize();
        let parsed = MbapHeader::deserialize(&bytes).unwrap();
        assert_eq!(header.transaction_id, parsed.transaction_id);
        assert_eq!(header.unit_id, parsed.unit_id);
        assert_eq!(header.length, parsed.length);
    }

    #[test]
    fn tcp_frame_round_trip() {
        let pdu = Pdu::new_request(
            FunctionCode::ReadHoldingRegisters,
            vec![0x00, 0x00, 0x00, 0x0A],
        );
        let frame = TcpFrame::new(1, 0x01, pdu);
        let bytes = frame.serialize();
        let parsed = TcpFrame::deserialize(&bytes).unwrap();

        assert_eq!(parsed.header.transaction_id, 1);
        assert_eq!(parsed.header.unit_id, 0x01);
        assert_eq!(parsed.pdu.function_code, FunctionCode::ReadHoldingRegisters);
        assert_eq!(parsed.pdu.data, vec![0x00, 0x00, 0x00, 0x0A]);
    }

    #[test]
    fn tcp_exception_round_trip() {
        let pdu = Pdu::new_exception(FunctionCode::ReadCoils, ExceptionCode::IllegalDataAddress);
        let frame = TcpFrame::new(42, 0x01, pdu);
        let bytes = frame.serialize();
        let parsed = TcpFrame::deserialize(&bytes).unwrap();

        assert_eq!(parsed.pdu.frame_type, FrameType::Exception);
        assert_eq!(parsed.pdu.function_code, FunctionCode::ReadCoils);
        assert_eq!(
            parsed.pdu.exception_code,
            Some(ExceptionCode::IllegalDataAddress)
        );
    }

    #[test]
    fn mbap_too_short() {
        assert!(MbapHeader::deserialize(&[0x00, 0x01]).is_err());
    }

    #[test]
    fn mbap_wrong_protocol_id() {
        let mut bytes = MbapHeader::new(1, 1, 5).serialize();
        bytes[2] = 0xFF; // corrupt protocol ID
        assert!(MbapHeader::deserialize(&bytes).is_err());
    }
}
