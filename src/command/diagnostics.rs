//! Diagnostics command implementation (FC 08).
//!
//! Supports a subset of Modbus diagnostic sub-functions for testing
//! communication and retrieving device diagnostic information.

use crate::command::require_data_len;
use crate::data_model::DataModel;
use crate::error::{ModbusError, Result};
use crate::frame::Pdu;
use crate::protocol::{DiagnosticsCode, ExceptionCode, FunctionCode};

/// Diagnostics — FC 08.
///
/// Dispatches to the appropriate diagnostic sub-function. Currently supports:
/// - `ReturnQueryData` (0x0000) — echo back the data field
/// - `RestartCommunicationsOption` (0x0001) — clear counters
/// - `ReturnDiagnosticRegister` (0x0002) — return diagnostic register
/// - `ClearCountersAndDiagnosticRegister` (0x000A) — clear all counters
///
/// # Errors
/// - `IllegalDataValue` if the request data is too short
/// - `IllegalFunction` if the sub-function code is not supported
pub fn diagnostics(_data: &mut DataModel, request: &Pdu) -> Result<Pdu> {
    require_data_len(request, 4)?;

    let sub_function = (u16::from(request.data[0]) << 8) | u16::from(request.data[1]);

    let mut response_data = vec![
        request.data[0], // Echo sub-function high byte
        request.data[1], // Echo sub-function low byte
        0x00,
        0x00,
    ];

    match DiagnosticsCode::try_from(sub_function) {
        Ok(DiagnosticsCode::ReturnQueryData) => {
            // Echo back the data field
            response_data[2] = request.data[2];
            response_data[3] = request.data[3];
        }
        Ok(DiagnosticsCode::RestartCommunicationsOption)
        | Ok(DiagnosticsCode::ReturnDiagnosticRegister)
        | Ok(DiagnosticsCode::ClearCountersAndDiagnosticRegister) => {
            // Clear counters / return zeros
            response_data[2] = 0x00;
            response_data[3] = 0x00;
        }
        _ => {
            // Unsupported diagnostic sub-function
            return Err(ModbusError::Exception {
                function_code: FunctionCode::Diagnostics,
                exception_code: ExceptionCode::IllegalFunction,
            });
        }
    }

    Ok(Pdu::new_response(FunctionCode::Diagnostics, response_data))
}
