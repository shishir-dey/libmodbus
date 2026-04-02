//! Core Modbus protocol definitions and enumerations.
//!
//! Contains the fundamental Modbus protocol definitions including function codes,
//! exception codes, and diagnostic sub-function codes as specified in the
//! Modbus Application Protocol Specification V1.1b3.

use crate::error::ModbusError;

/// Modbus function codes as defined in the Modbus specification.
///
/// Each function code corresponds to a specific operation that can be performed
/// on Modbus data. The error bit (0x80) is handled separately during frame parsing.
///
/// # Examples
/// ```
/// use libmodbuzz::FunctionCode;
///
/// let code = FunctionCode::ReadCoils;
/// assert_eq!(u8::from(code), 0x01);
/// ```
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
#[repr(u8)]
pub enum FunctionCode {
    /// Read coils (discrete outputs) — FC 01
    ReadCoils = 0x01,
    /// Read discrete inputs — FC 02
    ReadDiscreteInputs = 0x02,
    /// Read holding registers — FC 03
    ReadHoldingRegisters = 0x03,
    /// Read input registers — FC 04
    ReadInputRegisters = 0x04,
    /// Write single coil — FC 05
    WriteSingleCoil = 0x05,
    /// Write single register — FC 06
    WriteSingleRegister = 0x06,
    /// Read exception status — FC 07
    ReadExceptionStatus = 0x07,
    /// Diagnostics — FC 08
    Diagnostics = 0x08,
    /// Write multiple coils — FC 15 (0x0F)
    WriteMultipleCoils = 0x0F,
    /// Write multiple registers — FC 16 (0x10)
    WriteMultipleRegisters = 0x10,
}

impl From<FunctionCode> for u8 {
    fn from(code: FunctionCode) -> u8 {
        code as u8
    }
}

impl TryFrom<u8> for FunctionCode {
    type Error = ModbusError;

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0x01 => Ok(FunctionCode::ReadCoils),
            0x02 => Ok(FunctionCode::ReadDiscreteInputs),
            0x03 => Ok(FunctionCode::ReadHoldingRegisters),
            0x04 => Ok(FunctionCode::ReadInputRegisters),
            0x05 => Ok(FunctionCode::WriteSingleCoil),
            0x06 => Ok(FunctionCode::WriteSingleRegister),
            0x07 => Ok(FunctionCode::ReadExceptionStatus),
            0x08 => Ok(FunctionCode::Diagnostics),
            0x0F => Ok(FunctionCode::WriteMultipleCoils),
            0x10 => Ok(FunctionCode::WriteMultipleRegisters),
            _ => Err(ModbusError::UnsupportedFunctionCode(value)),
        }
    }
}

/// Modbus exception codes returned in error responses.
///
/// When a Modbus request cannot be processed successfully, the server responds
/// with an exception frame containing one of these codes to indicate the
/// specific error condition.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u8)]
pub enum ExceptionCode {
    /// Function code not supported by the device
    IllegalFunction = 0x01,
    /// Data address not valid for the device
    IllegalDataAddress = 0x02,
    /// Data value not valid for the request
    IllegalDataValue = 0x03,
    /// Unrecoverable error in slave device
    SlaveDeviceFailure = 0x04,
    /// Request accepted, processing (long duration)
    Acknowledge = 0x05,
    /// Slave device is busy processing another request
    SlaveDeviceBusy = 0x06,
    /// Request cannot be performed (programming failures)
    NegativeAcknowledgment = 0x07,
    /// Memory parity error detected
    MemoryParityError = 0x08,
    /// Gateway path unavailable
    GatewayPathUnavailable = 0x0A,
    /// Gateway target device failed to respond
    GatewayTargetDeviceFailedToRespond = 0x0B,
}

impl From<ExceptionCode> for u8 {
    fn from(code: ExceptionCode) -> u8 {
        code as u8
    }
}

impl TryFrom<u8> for ExceptionCode {
    type Error = ModbusError;

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        match value {
            0x01 => Ok(ExceptionCode::IllegalFunction),
            0x02 => Ok(ExceptionCode::IllegalDataAddress),
            0x03 => Ok(ExceptionCode::IllegalDataValue),
            0x04 => Ok(ExceptionCode::SlaveDeviceFailure),
            0x05 => Ok(ExceptionCode::Acknowledge),
            0x06 => Ok(ExceptionCode::SlaveDeviceBusy),
            0x07 => Ok(ExceptionCode::NegativeAcknowledgment),
            0x08 => Ok(ExceptionCode::MemoryParityError),
            0x0A => Ok(ExceptionCode::GatewayPathUnavailable),
            0x0B => Ok(ExceptionCode::GatewayTargetDeviceFailedToRespond),
            _ => Err(ModbusError::InvalidExceptionCode(value)),
        }
    }
}

/// Diagnostic sub-function codes for function code 08 (Diagnostics).
///
/// The diagnostics function (FC 08) supports various sub-functions for testing
/// communication and retrieving diagnostic information from Modbus devices.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
#[repr(u16)]
pub enum DiagnosticsCode {
    /// Echo back query data (loopback test)
    ReturnQueryData = 0x0000,
    /// Restart communications option
    RestartCommunicationsOption = 0x0001,
    /// Return diagnostic register contents
    ReturnDiagnosticRegister = 0x0002,
    /// Change ASCII input delimiter
    ChangeAsciiInputDelimiter = 0x0003,
    /// Force listen only mode
    ForceListenOnlyMode = 0x0004,
    /// Clear all counters and diagnostic register
    ClearCountersAndDiagnosticRegister = 0x000A,
    /// Return bus message count
    ReturnBusMessageCount = 0x000B,
    /// Return bus communication error count
    ReturnBusCommunicationErrorCount = 0x000C,
    /// Return bus exception error count
    ReturnBusExceptionErrorCount = 0x000D,
    /// Return slave message count
    ReturnSlaveMessageCount = 0x000E,
    /// Return slave no response count
    ReturnSlaveNoResponseCount = 0x000F,
    /// Return slave NAK count
    ReturnSlaveNakCount = 0x0010,
    /// Return slave busy count
    ReturnSlaveBusyCount = 0x0011,
    /// Return bus character overrun count
    ReturnBusCharacterOverrunCount = 0x0012,
}

impl From<DiagnosticsCode> for u16 {
    fn from(code: DiagnosticsCode) -> u16 {
        code as u16
    }
}

impl TryFrom<u16> for DiagnosticsCode {
    type Error = ModbusError;

    fn try_from(value: u16) -> Result<Self, Self::Error> {
        match value {
            0x0000 => Ok(DiagnosticsCode::ReturnQueryData),
            0x0001 => Ok(DiagnosticsCode::RestartCommunicationsOption),
            0x0002 => Ok(DiagnosticsCode::ReturnDiagnosticRegister),
            0x0003 => Ok(DiagnosticsCode::ChangeAsciiInputDelimiter),
            0x0004 => Ok(DiagnosticsCode::ForceListenOnlyMode),
            0x000A => Ok(DiagnosticsCode::ClearCountersAndDiagnosticRegister),
            0x000B => Ok(DiagnosticsCode::ReturnBusMessageCount),
            0x000C => Ok(DiagnosticsCode::ReturnBusCommunicationErrorCount),
            0x000D => Ok(DiagnosticsCode::ReturnBusExceptionErrorCount),
            0x000E => Ok(DiagnosticsCode::ReturnSlaveMessageCount),
            0x000F => Ok(DiagnosticsCode::ReturnSlaveNoResponseCount),
            0x0010 => Ok(DiagnosticsCode::ReturnSlaveNakCount),
            0x0011 => Ok(DiagnosticsCode::ReturnSlaveBusyCount),
            0x0012 => Ok(DiagnosticsCode::ReturnBusCharacterOverrunCount),
            _ => Err(ModbusError::UnsupportedDiagnosticsCode(value)),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn function_code_round_trip() {
        for code in [
            FunctionCode::ReadCoils,
            FunctionCode::ReadDiscreteInputs,
            FunctionCode::ReadHoldingRegisters,
            FunctionCode::ReadInputRegisters,
            FunctionCode::WriteSingleCoil,
            FunctionCode::WriteSingleRegister,
            FunctionCode::Diagnostics,
            FunctionCode::WriteMultipleCoils,
            FunctionCode::WriteMultipleRegisters,
        ] {
            let byte = u8::from(code);
            let parsed = FunctionCode::try_from(byte).unwrap();
            assert_eq!(code, parsed);
        }
    }

    #[test]
    fn invalid_function_code() {
        assert!(FunctionCode::try_from(0xFF).is_err());
        assert!(FunctionCode::try_from(0x00).is_err());
    }

    #[test]
    fn exception_code_round_trip() {
        let code = ExceptionCode::IllegalFunction;
        assert_eq!(u8::from(code), 0x01);
        assert_eq!(ExceptionCode::try_from(0x01).unwrap(), code);
    }

    #[test]
    fn diagnostics_code_round_trip() {
        let code = DiagnosticsCode::ReturnQueryData;
        assert_eq!(u16::from(code), 0x0000);
        assert_eq!(DiagnosticsCode::try_from(0x0000).unwrap(), code);
    }
}
