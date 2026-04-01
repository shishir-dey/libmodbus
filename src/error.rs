//! Error types for the libmodbus crate.
//!
//! Provides a unified error enum that covers protocol exceptions, frame parsing
//! errors, checksum mismatches, and IO failures, enabling clean error propagation
//! with the `?` operator throughout the crate.

use crate::protocol::{ExceptionCode, FunctionCode};
use std::fmt;

/// Unified error type for all Modbus operations.
///
/// This enum covers both protocol-level exceptions (which are sent back to the
/// Modbus client as exception responses) and library-level errors (frame parsing,
/// checksum failures, etc.).
///
/// # Examples
/// ```
/// use libmodbus::ModbusError;
///
/// let err = ModbusError::InvalidFrame {
///     reason: "frame too short".into(),
///     actual_len: 2,
/// };
/// assert!(format!("{}", err).contains("frame too short"));
/// ```
#[derive(Debug)]
pub enum ModbusError {
    /// A Modbus protocol exception to be returned to the client.
    ///
    /// This variant is used when the request is valid at the frame level but
    /// cannot be fulfilled due to protocol-level issues (invalid address,
    /// invalid value, unsupported function, etc.).
    Exception {
        /// The function code from the original request.
        function_code: FunctionCode,
        /// The specific exception code describing the error.
        exception_code: ExceptionCode,
    },

    /// The received frame is malformed or too short.
    InvalidFrame {
        /// Human-readable description of what's wrong.
        reason: String,
        /// Actual length of the received data.
        actual_len: usize,
    },

    /// CRC or LRC checksum mismatch.
    ChecksumMismatch {
        /// The checksum value calculated from the received data.
        expected: u16,
        /// The checksum value extracted from the frame.
        actual: u16,
    },

    /// The slave address in the request doesn't match this server.
    AddressMismatch {
        /// This server's configured address.
        expected: u8,
        /// The address from the received frame.
        actual: u8,
    },

    /// Received a function code that is not recognized.
    UnsupportedFunctionCode(u8),

    /// Received an exception code that is not recognized.
    InvalidExceptionCode(u8),

    /// Received a diagnostics sub-function code that is not supported.
    UnsupportedDiagnosticsCode(u16),

    /// An IO error from the underlying transport.
    Io(std::io::Error),
}

impl fmt::Display for ModbusError {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        match self {
            ModbusError::Exception {
                function_code,
                exception_code,
            } => write!(
                f,
                "Modbus exception on FC {:?}: {:?}",
                function_code, exception_code
            ),
            ModbusError::InvalidFrame { reason, actual_len } => {
                write!(f, "Invalid frame (len={}): {}", actual_len, reason)
            }
            ModbusError::ChecksumMismatch { expected, actual } => {
                write!(
                    f,
                    "Checksum mismatch: expected 0x{:04X}, got 0x{:04X}",
                    expected, actual
                )
            }
            ModbusError::AddressMismatch { expected, actual } => {
                write!(f, "Address mismatch: expected {}, got {}", expected, actual)
            }
            ModbusError::UnsupportedFunctionCode(code) => {
                write!(f, "Unsupported function code: 0x{:02X}", code)
            }
            ModbusError::InvalidExceptionCode(code) => {
                write!(f, "Invalid exception code: 0x{:02X}", code)
            }
            ModbusError::UnsupportedDiagnosticsCode(code) => {
                write!(f, "Unsupported diagnostics sub-function: 0x{:04X}", code)
            }
            ModbusError::Io(err) => write!(f, "IO error: {}", err),
        }
    }
}

impl std::error::Error for ModbusError {
    fn source(&self) -> Option<&(dyn std::error::Error + 'static)> {
        match self {
            ModbusError::Io(err) => Some(err),
            _ => None,
        }
    }
}

impl From<std::io::Error> for ModbusError {
    fn from(err: std::io::Error) -> Self {
        ModbusError::Io(err)
    }
}

/// Convenience type alias for operations that may produce a [`ModbusError`].
pub type Result<T> = std::result::Result<T, ModbusError>;
