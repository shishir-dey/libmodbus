//! Read command implementations (FC 01, 02, 03, 04).
//!
//! Handles reading coils, discrete inputs, holding registers, and input registers
//! from the data model.

use crate::command::{validate_address_range, validate_quantity};
use crate::data_model::DataModel;
use crate::error::Result;
use crate::frame::Pdu;
use crate::protocol::FunctionCode;

/// Read Coils — FC 01.
///
/// Reads the status of 1–2000 coils starting at the given address.
/// Response contains a byte count followed by bit-packed coil values (LSB first).
pub fn read_coils(data: &DataModel, request: &Pdu) -> Result<Pdu> {
    validate_quantity(request, 1, 2000)?;
    let (addr, qty) = validate_address_range(request, data.max_coils())?;

    let byte_count = qty.div_ceil(8) as usize;
    let mut response_data = vec![0u8; 1 + byte_count];
    response_data[0] = byte_count as u8;

    for i in 0..qty {
        if let Some(true) = data.read_coil(addr + i) {
            let byte_idx = 1 + (i as usize / 8);
            let bit_idx = i as usize % 8;
            response_data[byte_idx] |= 1 << bit_idx;
        }
    }

    Ok(Pdu::new_response(FunctionCode::ReadCoils, response_data))
}

/// Read Discrete Inputs — FC 02.
///
/// Reads the status of 1–2000 discrete inputs starting at the given address.
/// Response format is identical to Read Coils.
pub fn read_discrete_inputs(data: &DataModel, request: &Pdu) -> Result<Pdu> {
    validate_quantity(request, 1, 2000)?;
    let (addr, qty) = validate_address_range(request, data.max_discrete_inputs())?;

    let byte_count = qty.div_ceil(8) as usize;
    let mut response_data = vec![0u8; 1 + byte_count];
    response_data[0] = byte_count as u8;

    for i in 0..qty {
        if let Some(true) = data.read_discrete_input(addr + i) {
            let byte_idx = 1 + (i as usize / 8);
            let bit_idx = i as usize % 8;
            response_data[byte_idx] |= 1 << bit_idx;
        }
    }

    Ok(Pdu::new_response(
        FunctionCode::ReadDiscreteInputs,
        response_data,
    ))
}

/// Read Holding Registers — FC 03.
///
/// Reads 1–125 holding registers starting at the given address.
/// Response contains a byte count followed by register values in big-endian format.
pub fn read_holding_registers(data: &DataModel, request: &Pdu) -> Result<Pdu> {
    validate_quantity(request, 1, 125)?;
    let (addr, qty) = validate_address_range(request, data.max_holding_registers())?;

    let byte_count = (qty * 2) as usize;
    let mut response_data = Vec::with_capacity(1 + byte_count);
    response_data.push(byte_count as u8);

    for i in 0..qty {
        let value = data.read_holding_register(addr + i).unwrap_or(0);
        response_data.push((value >> 8) as u8);
        response_data.push((value & 0xFF) as u8);
    }

    Ok(Pdu::new_response(
        FunctionCode::ReadHoldingRegisters,
        response_data,
    ))
}

/// Read Input Registers — FC 04.
///
/// Reads 1–125 input registers starting at the given address.
/// Response format is identical to Read Holding Registers.
pub fn read_input_registers(data: &DataModel, request: &Pdu) -> Result<Pdu> {
    validate_quantity(request, 1, 125)?;
    let (addr, qty) = validate_address_range(request, data.max_input_registers())?;

    let byte_count = (qty * 2) as usize;
    let mut response_data = Vec::with_capacity(1 + byte_count);
    response_data.push(byte_count as u8);

    for i in 0..qty {
        let value = data.read_input_register(addr + i).unwrap_or(0);
        response_data.push((value >> 8) as u8);
        response_data.push((value & 0xFF) as u8);
    }

    Ok(Pdu::new_response(
        FunctionCode::ReadInputRegisters,
        response_data,
    ))
}
