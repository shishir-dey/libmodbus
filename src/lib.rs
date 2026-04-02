//! # libmodbuzz — Safe Modbus Protocol Implementation
//!
//! A safe, idiomatic Rust implementation of the Modbus protocol supporting
//! RTU (serial) and TCP (Ethernet) transports.
//!
//! ## Quick Start
//!
//! ```rust
//! use libmodbuzz::server::rtu::RtuServer;
//! use libmodbuzz::server::ModbusServer;
//!
//! // Create an RTU server on slave address 1
//! let mut server = RtuServer::new(1);
//!
//! // Initialize some data
//! server.data.write_coil(0, true);
//! server.data.write_holding_register(0, 0x1234);
//!
//! // Process a Read Coils request (FC 01)
//! let request = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
//! let response = server.process(&request);
//! assert!(response.is_some());
//! ```
//!
//! ## Architecture
//!
//! The library is organized into clean, separated modules:
//!
//! - [`protocol`] — Modbus function codes, exception codes, diagnostic codes
//! - [`error`] — Unified error handling with [`ModbusError`]
//! - [`checksum`] — CRC-16 (RTU) and LRC (ASCII) calculations
//! - [`frame`] — Protocol Data Unit and transport-specific frame types
//! - [`data_model`] — Four-zone data model (coils, inputs, registers)
//! - [`command`] — Function code dispatch and processing
//! - [`server`] — Server trait and RTU implementation
//!
//! ## Supported Function Codes
//!
//! | Code | Name | Type |
//! |------|------|------|
//! | FC 01 | Read Coils | Read |
//! | FC 02 | Read Discrete Inputs | Read |
//! | FC 03 | Read Holding Registers | Read |
//! | FC 04 | Read Input Registers | Read |
//! | FC 05 | Write Single Coil | Write |
//! | FC 06 | Write Single Register | Write |
//! | FC 08 | Diagnostics | Diagnostic |
//! | FC 15 | Write Multiple Coils | Write |
//! | FC 16 | Write Multiple Registers | Write |

pub mod checksum;
pub mod command;
pub mod data_model;
pub mod error;
pub mod frame;
pub mod protocol;
pub mod server;

// ── Public re-exports for ergonomic use ────────────────────────────────

pub use data_model::DataModel;
pub use error::ModbusError;
pub use protocol::{DiagnosticsCode, ExceptionCode, FunctionCode};
