//! Modbus RTU server implementation.
//!
//! Handles the complete RTU request processing pipeline:
//! 1. Frame validation (minimum size)
//! 2. CRC-16 verification
//! 3. Slave address matching
//! 4. Function code dispatch
//! 5. Response frame generation with CRC

use crate::command;
use crate::data_model::DataModel;
use crate::frame::FrameType;
use crate::frame::rtu::RtuFrame;
use crate::server::ModbusServer;

/// Modbus RTU server with configurable slave address.
///
/// Unlike the C++ implementation (which hardcodes address 1), this server
/// supports any slave address via the builder pattern.
///
/// # Examples
/// ```
/// use libmodbus::server::rtu::RtuServer;
/// use libmodbus::server::ModbusServer;
///
/// let mut server = RtuServer::new(1);
/// server.data.write_coil(0, true);
///
/// // Process an FC 01 Read Coils request
/// let request = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0xFD, 0xCA];
/// if let Some(response) = server.process(&request) {
///     println!("Response: {:02X?}", response);
/// }
/// ```
#[derive(Debug)]
pub struct RtuServer {
    /// The slave address this server responds to (1–247).
    pub address: u8,
    /// The Modbus data model holding coils, inputs, and registers.
    pub data: DataModel,
}

impl RtuServer {
    /// Create a new RTU server with the given slave address and default data model.
    pub fn new(address: u8) -> Self {
        Self {
            address,
            data: DataModel::default(),
        }
    }

    /// Create a new RTU server with the given slave address and custom data model.
    pub fn with_data_model(address: u8, data: DataModel) -> Self {
        Self { address, data }
    }
}

impl ModbusServer for RtuServer {
    fn process(&mut self, request_data: &[u8]) -> Option<Vec<u8>> {
        // Minimum frame: slave addr (1) + function code (1) + CRC (2) = 4 bytes
        if request_data.len() < 4 {
            return None;
        }

        // Deserialize the request frame
        let request = match RtuFrame::deserialize(FrameType::Request, request_data) {
            Ok(frame) => frame,
            Err(_) => return None,
        };

        // Validate CRC
        if request.validate_checksum(request_data).is_err() {
            return None;
        }

        // Check slave address — only respond to our address
        if request.slave_address != self.address {
            return None;
        }

        // Don't respond to broadcast (address 0) per Modbus spec
        if request.slave_address == 0 {
            return None;
        }

        // Execute the command
        let response_pdu = command::execute(&mut self.data, &request.pdu);
        let response_frame = RtuFrame::new(request.slave_address, response_pdu);

        Some(response_frame.serialize())
    }
}
