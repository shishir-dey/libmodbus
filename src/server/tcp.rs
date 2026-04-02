//! Modbus TCP server implementation.
//!
//! Handles the Modbus TCP request processing pipeline:
//! 1. MBAP header parsing
//! 2. Unit ID (slave address) matching
//! 3. Function code dispatch
//! 4. Response frame generation with MBAP header

use crate::command;
use crate::data_model::DataModel;
use crate::frame::tcp::TcpFrame;
use crate::server::ModbusServer;

/// Modbus TCP server with configurable unit ID.
///
/// Unlike RTU, TCP frames use the MBAP header instead of CRC for framing.
/// The `unit_id` field in the MBAP header serves the same purpose as the
/// slave address in RTU.
///
/// # Examples
/// ```
/// use libmodbuzz::server::tcp::TcpServer;
/// use libmodbuzz::server::ModbusServer;
///
/// let mut server = TcpServer::new(1);
/// server.data.write_holding_register(0, 0xCAFE);
///
/// // Build a Modbus TCP Read Holding Registers request
/// // MBAP: txn=1, proto=0, len=6, unit=1 | FC=03, addr=0x0000, qty=0x0001
/// let request = vec![
///     0x00, 0x01, 0x00, 0x00, 0x00, 0x06, 0x01, // MBAP header
///     0x03, 0x00, 0x00, 0x00, 0x01,              // PDU
/// ];
/// let response = server.process(&request).unwrap();
/// assert!(response.len() > 7); // MBAP header + PDU
/// ```
#[derive(Debug)]
pub struct TcpServer {
    /// The unit ID this server responds to (1–247, or 0 for any).
    pub unit_id: u8,
    /// The Modbus data model holding coils, inputs, and registers.
    pub data: DataModel,
}

impl TcpServer {
    /// Create a new TCP server with the given unit ID and default data model.
    pub fn new(unit_id: u8) -> Self {
        Self {
            unit_id,
            data: DataModel::default(),
        }
    }

    /// Create a new TCP server with the given unit ID and custom data model.
    pub fn with_data_model(unit_id: u8, data: DataModel) -> Self {
        Self { unit_id, data }
    }
}

impl ModbusServer for TcpServer {
    fn process(&mut self, request_data: &[u8]) -> Option<Vec<u8>> {
        // Parse the TCP frame (MBAP header + PDU)
        let request = match TcpFrame::deserialize(request_data) {
            Ok(frame) => frame,
            Err(_) => return None,
        };

        // Check unit ID — respond only to our unit ID (or 0 = any)
        if self.unit_id != 0 && request.header.unit_id != self.unit_id {
            return None;
        }

        // Execute the command
        let response_pdu = command::execute(&mut self.data, &request.pdu);

        // Build response TCP frame, preserving the transaction ID from the request
        let response_frame = TcpFrame::new(
            request.header.transaction_id,
            request.header.unit_id,
            response_pdu,
        );

        Some(response_frame.serialize())
    }
}
