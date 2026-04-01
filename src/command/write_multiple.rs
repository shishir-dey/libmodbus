//! Multiple-write command implementations (FC 15, 16).
//!
//! Handles writing multiple coils or multiple holding registers in a single request.

use crate::command::{require_data_len, start_address, validate_quantity};
use crate::data_model::DataModel;
use crate::error::{ModbusError, Result};
use crate::frame::Pdu;
use crate::protocol::{ExceptionCode, FunctionCode};

/// Write Multiple Coils — FC 15 (0x0F).
///
/// Writes 1–1968 coils from bit-packed data. The request includes a byte count
/// field that must match `ceil(quantity / 8)`.
///
/// # Request Data Layout
/// ```text
/// [0..2] Start Address (big-endian)
/// [2..4] Quantity of Coils (big-endian)
/// [4]    Byte Count
/// [5..]  Coil Values (bit-packed, LSB first)
/// ```
///
/// # Response
/// Echoes the start address and quantity (4 bytes).
pub fn write_multiple_coils(data: &mut DataModel, request: &Pdu) -> Result<Pdu> {
    require_data_len(request, 5)?;

    let addr = start_address(request);
    let qty = validate_quantity(request, 1, 1968)?;
    let byte_count = request.data[4];

    // Validate byte count matches quantity
    let expected_byte_count = qty.div_ceil(8) as u8;
    if byte_count != expected_byte_count {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }

    // Ensure the request data actually contains all the declared coil bytes
    if request.data.len() < 5 + usize::from(byte_count) {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }

    // Validate address range
    let end = u32::from(addr) + u32::from(qty);
    if end > data.max_coils() as u32 {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataAddress,
        });
    }

    // Extract and write coil values from bit-packed data
    for i in 0..qty {
        let byte_idx = 5 + (i as usize / 8);
        let bit_idx = i as usize % 8;
        let coil_value = (request.data[byte_idx] >> bit_idx) & 0x01 != 0;
        data.write_coil(addr + i, coil_value);
    }

    // Response: echo start address and quantity
    Ok(Pdu::new_response(
        FunctionCode::WriteMultipleCoils,
        request.data[..4].to_vec(),
    ))
}

/// Write Multiple Registers — FC 16 (0x10).
///
/// Writes 1–123 holding registers from big-endian 16-bit values. The request
/// includes a byte count field that must equal `quantity * 2`.
///
/// # Request Data Layout
/// ```text
/// [0..2] Start Address (big-endian)
/// [2..4] Quantity of Registers (big-endian)
/// [4]    Byte Count
/// [5..]  Register Values (big-endian, 2 bytes each)
/// ```
///
/// # Response
/// Echoes the start address and quantity (4 bytes).
pub fn write_multiple_registers(data: &mut DataModel, request: &Pdu) -> Result<Pdu> {
    require_data_len(request, 5)?;

    let addr = start_address(request);
    let qty = validate_quantity(request, 1, 123)?;
    let byte_count = request.data[4];

    // Each register is 2 bytes
    let expected_byte_count = (qty * 2) as u8;
    if byte_count != expected_byte_count {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }

    // Ensure the request data contains all declared register bytes
    if request.data.len() < 5 + usize::from(byte_count) {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataValue,
        });
    }

    // Validate address range
    let end = u32::from(addr) + u32::from(qty);
    if end > data.max_holding_registers() as u32 {
        return Err(ModbusError::Exception {
            function_code: request.function_code,
            exception_code: ExceptionCode::IllegalDataAddress,
        });
    }

    // Extract and write register values (big-endian)
    for i in 0..qty {
        let offset = 5 + (i as usize * 2);
        let value = (u16::from(request.data[offset]) << 8) | u16::from(request.data[offset + 1]);
        data.write_holding_register(addr + i, value);
    }

    // Response: echo start address and quantity
    Ok(Pdu::new_response(
        FunctionCode::WriteMultipleRegisters,
        request.data[..4].to_vec(),
    ))
}
