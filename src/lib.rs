#![warn(missing_docs)]

//! # libmodbuzz
//!
//! A safe, idiomatic Rust implementation of the Modbus protocol supporting
//! RTU (serial) and TCP (Ethernet) transports.
//!
//! The crate exposes protocol enums, transport-independent PDUs, RTU and TCP
//! frame codecs, an in-memory Modbus data model, and ready-to-use RTU/TCP
//! server implementations.
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
//! - [`data_model`] — Four-zone data model (coils, inputs, registers, all zero-based)
//! - [`command`] — Function code dispatch and processing
//! - [`server`] — Shared server trait with RTU and TCP implementations
//!
//! ## Supported Function Codes
//!
//! | Code | Name | Status | Notes |
//! |------|------|--------|-------|
//! | FC 01 | Read Coils | Implemented | |
//! | FC 02 | Read Discrete Inputs | Implemented | |
//! | FC 03 | Read Holding Registers | Implemented | |
//! | FC 04 | Read Input Registers | Implemented | |
//! | FC 05 | Write Single Coil | Implemented | |
//! | FC 06 | Write Single Register | Implemented | |
//! | FC 07 | Read Exception Status | Defined only | Present in [`FunctionCode`], currently returns `IllegalFunction`. |
//! | FC 08 | Diagnostics | Partially implemented | Supports a subset of [`DiagnosticsCode`] values. |
//! | FC 15 | Write Multiple Coils | Implemented | |
//! | FC 16 | Write Multiple Registers | Implemented | |
//!
//! ## Addressing
//!
//! Public APIs in this crate use zero-based addresses. If your device map is
//! documented with Modbus reference numbers such as `00001` or `40001`, convert
//! those to zero-based offsets before calling library functions.

pub mod checksum;
pub mod command;
pub mod data_model;
pub mod error;
pub mod frame;
pub mod protocol;
pub mod server;

// ── Public re-exports for ergonomic use ────────────────────────────────

/// In-memory storage for coils, discrete inputs, and registers.
pub use data_model::DataModel;
/// Error type returned by frame parsing, command dispatch, and transport code.
pub use error::ModbusError;
/// Diagnostic sub-function codes for Modbus function code 08.
pub use protocol::DiagnosticsCode;
/// Modbus exception codes used in exception responses.
pub use protocol::ExceptionCode;
/// Modbus function codes supported or recognized by the crate.
pub use protocol::FunctionCode;
