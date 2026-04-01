//! Modbus command processing and dispatch.
//!
//! This module replaces the C++ Command pattern (virtual inheritance) with
//! function-based dispatch via `match` on [`FunctionCode`]. Each function code
//! maps to a dedicated handler function in the submodules.
//!
//! # Architecture
//! Instead of `std::map<FunctionCode, unique_ptr<Command>>`, Rust uses a simple
//! `match` expression — no heap allocation, no vtable, zero overhead.

pub mod diagnostics;
pub mod read;
pub mod write;
pub mod write_multiple;

use crate::data_model::DataModel;
use crate::error::ModbusError;
use crate::frame::Pdu;
use crate::protocol::{ExceptionCode, FunctionCode};

/// Execute a Modbus command based on the function code in the request PDU.
///
/// This is the central dispatch function that routes each function code to
/// its handler. Returns a response PDU on success, or an exception PDU
/// on protocol-level errors.
///
/// # Arguments
/// * `data` — mutable reference to the data model
/// * `request` — the incoming request PDU
///
/// # Returns
/// A response [`Pdu`] (either normal or exception).
pub fn execute(data: &mut DataModel, request: &Pdu) -> Pdu {
    let result = match request.function_code {
        FunctionCode::ReadCoils => read::read_coils(data, request),
        FunctionCode::ReadDiscreteInputs => read::read_discrete_inputs(data, request),
        FunctionCode::ReadHoldingRegisters => read::read_holding_registers(data, request),
        FunctionCode::ReadInputRegisters => read::read_input_registers(data, request),
        FunctionCode::WriteSingleCoil => write::write_single_coil(data, request),
        FunctionCode::WriteSingleRegister => write::write_single_register(data, request),
        FunctionCode::WriteMultipleCoils => write_multiple::write_multiple_coils(data, request),
        FunctionCode::WriteMultipleRegisters => {
            write_multiple::write_multiple_registers(data, request)
        }
        FunctionCode::Diagnostics => diagnostics::diagnostics(data, request),
        _ => Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalFunction,
        }),
    };

    match result {
        Ok(pdu) => pdu,
        Err(ModbusError::Exception {
            function_code,
            exception_code,
        }) => Pdu::new_exception(function_code, exception_code),
        Err(_) => {
            // Non-protocol errors become SlaveDeviceFailure
            Pdu::new_exception(request.function_code, ExceptionCode::SlaveDeviceFailure)
        }
    }
}

// ── Validation helpers ──────────────────────────────────────────────────

/// Validate that the request PDU has at least `min_bytes` of data.
pub(crate) fn require_data_len(request: &Pdu, min_bytes: usize) -> Result<(), ModbusError> {
    if request.data.len() < min_bytes {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }
    Ok(())
}

/// Extract the start address from request data bytes [0..2] (big-endian).
pub(crate) fn start_address(request: &Pdu) -> u16 {
    (u16::from(request.data[0]) << 8) | u16::from(request.data[1])
}

/// Extract the quantity from request data bytes [2..4] (big-endian).
pub(crate) fn quantity(request: &Pdu) -> u16 {
    (u16::from(request.data[2]) << 8) | u16::from(request.data[3])
}

/// Validate the quantity is within [min, max].
pub(crate) fn validate_quantity(request: &Pdu, min: u16, max: u16) -> Result<u16, ModbusError> {
    require_data_len(request, 4)?;
    let qty = quantity(request);

    if qty < min || qty > max {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }
    Ok(qty)
}

/// Validate that the address range [start, start+quantity) fits within [0, max_address).
pub(crate) fn validate_address_range(
    request: &Pdu,
    max_address: usize,
) -> Result<(u16, u16), ModbusError> {
    require_data_len(request, 4)?;

    let addr = start_address(request);
    let qty = quantity(request);

    // Overflow check
    let end = u32::from(addr) + u32::from(qty);
    if end > max_address as u32 {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataAddress,
        });
    }

    Ok((addr, qty))
}
