//! Modbus server trait and implementations.
//!
//! Provides a common [`ModbusServer`] trait that abstracts over different
//! transport protocols (RTU, TCP).

pub mod rtu;
pub mod tcp;

/// Trait for Modbus server implementations.
///
/// Each transport protocol (RTU, TCP, ASCII) implements this trait to handle
/// protocol-specific framing while sharing the same command processing logic.
///
/// # Returns
/// - `Some(response)` — a response frame to send back to the client
/// - `None` — no response should be sent (wrong address, broadcast, invalid CRC, etc.)
pub trait ModbusServer {
    /// Process a raw request and produce an optional response.
    ///
    /// # Arguments
    /// * `request_data` — raw bytes as received from the transport layer
    ///
    /// # Returns
    /// `Some(Vec<u8>)` containing the response frame, or `None` if no response
    /// should be sent.
    fn process(&mut self, request_data: &[u8]) -> Option<Vec<u8>>;
}
