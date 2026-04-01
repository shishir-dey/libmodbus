//! Single-write command implementations (FC 05, 06).
//!
//! Handles writing a single coil or a single holding register.

use crate::command::require_data_len;
use crate::data_model::DataModel;
use crate::error::{ModbusError, Result};
use crate::frame::Pdu;
use crate::protocol::{ExceptionCode, FunctionCode};

/// Write Single Coil — FC 05.
///
/// Writes a single coil to ON (`0xFF00`) or OFF (`0x0000`).
/// The response echoes the request data.
///
/// # Errors
/// - `IllegalDataValue` if the coil value is not `0x0000` or `0xFF00`
/// - `IllegalDataAddress` if the address is out of range
pub fn write_single_coil(data: &mut DataModel, request: &Pdu) -> Result<Pdu> {
    require_data_len(request, 4)?;

    let address = (u16::from(request.data[0]) << 8) | u16::from(request.data[1]);
    let value = (u16::from(request.data[2]) << 8) | u16::from(request.data[3]);

    // Only 0x0000 (OFF) and 0xFF00 (ON) are valid per Modbus spec
    if value != 0x0000 && value != 0xFF00 {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }

    // Validate address
    if usize::from(address) >= data.max_coils() {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataAddress,
        });
    }

    data.write_coil(address, value == 0xFF00);

    // Response echoes the request
    Ok(Pdu::new_response(
        FunctionCode::WriteSingleCoil,
        request.data[..4].to_vec(),
    ))
}

/// Write Single Register — FC 06.
///
/// Writes a single holding register with any 16-bit value.
/// The response echoes the request data.
///
/// # Errors
/// - `IllegalDataAddress` if the address is out of range
pub fn write_single_register(data: &mut DataModel, request: &Pdu) -> Result<Pdu> {
    require_data_len(request, 4)?;

    let address = (u16::from(request.data[0]) << 8) | u16::from(request.data[1]);
    let value = (u16::from(request.data[2]) << 8) | u16::from(request.data[3]);

    // Validate address
    if usize::from(address) >= data.max_holding_registers() {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataAddress,
        });
    }

    data.write_holding_register(address, value);

    // Response echoes the request
    Ok(Pdu::new_response(
        FunctionCode::WriteSingleRegister,
        request.data[..4].to_vec(),
    ))
}
