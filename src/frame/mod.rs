//! Modbus Protocol Data Unit (PDU) and frame type definitions.
//!
//! The PDU is the transport-independent core of a Modbus message, containing the
//! function code and data payload. Each transport protocol (RTU, TCP) wraps the
//! PDU with its own header/trailer (address, CRC, MBAP header, etc.).

pub mod rtu;
pub mod tcp;

use crate::protocol::{ExceptionCode, FunctionCode};

/// Identifies the type/role of a Modbus frame.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum FrameType {
    /// A request from client to server.
    Request,
    /// A normal response from server to client.
    Response,
    /// An exception response indicating an error.
    Exception,
}

/// Modbus Protocol Data Unit (PDU).
///
/// The PDU is the transport-independent part of a Modbus message. It contains
/// the function code, data payload, and (for exception frames) the exception code.
///
/// # Frame Layout
/// - Function Code (1 byte)
/// - Data (0–252 bytes)
/// - Exception Code (1 byte, only in exception frames)
///
/// # Examples
/// ```
/// use libmodbuzz::frame::{Pdu, FrameType};
/// use libmodbuzz::FunctionCode;
///
/// let pdu = Pdu::new_request(FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
/// assert_eq!(pdu.frame_type, FrameType::Request);
/// ```
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Pdu {
    /// The type of this frame (request, response, or exception).
    pub frame_type: FrameType,
    /// The Modbus function code.
    pub function_code: FunctionCode,
    /// The frame data payload (contents depend on function code and frame type).
    pub data: Vec<u8>,
    /// Exception code, only meaningful when `frame_type` is [`FrameType::Exception`].
    pub exception_code: Option<ExceptionCode>,
}

impl Pdu {
    /// Create a new request PDU.
    pub fn new_request(function_code: FunctionCode, data: Vec<u8>) -> Self {
        Self {
            frame_type: FrameType::Request,
            function_code,
            data,
            exception_code: None,
        }
    }

    /// Create a new response PDU.
    pub fn new_response(function_code: FunctionCode, data: Vec<u8>) -> Self {
        Self {
            frame_type: FrameType::Response,
            function_code,
            data,
            exception_code: None,
        }
    }

    /// Create a new exception response PDU.
    pub fn new_exception(function_code: FunctionCode, exception_code: ExceptionCode) -> Self {
        Self {
            frame_type: FrameType::Exception,
            function_code,
            data: Vec::new(),
            exception_code: Some(exception_code),
        }
    }
}
