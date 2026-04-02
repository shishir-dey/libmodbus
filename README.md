<div align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="assets/libmodbuzz-dark.png" width="1280">
  <source media="(prefers-color-scheme: light)" srcset="assets/libmodbuzz-light.png" width="1280">
  <img alt="libmodbuzz logo" src="assets/libmodbuzz-light.png" width="1280">
</picture>
</div>

<hr>

# libmodbuzz

Safe, dependency-free Rust Modbus primitives and servers for RTU and TCP.

## Installation

```toml
[dependencies]
libmodbuzz = { git = "https://github.com/shishir-dey/libmodbuzz" }
```

## Example

```rust
use libmodbuzz::server::rtu::RtuServer;
use libmodbuzz::server::ModbusServer;

let mut server = RtuServer::new(1);
server.data.write_coil(0, true);
server.data.write_holding_register(0, 0x1234);

// FC 01 Read Coils, start=0, qty=8
let request = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
let response = server.process(&request);
assert!(response.is_some());
```

## Included

- `DataModel` for coils, discrete inputs, holding registers, and input registers
- `frame::rtu::RtuFrame` and `frame::tcp::TcpFrame` for manual framing
- `server::rtu::RtuServer` and `server::tcp::TcpServer`
- `checksum::crc16` and `checksum::lrc`

## Supported Function Codes

Implemented: `01`, `02`, `03`, `04`, `05`, `06`, `15`, `16`  
Partial: `08` Diagnostics  
Defined but not implemented: `07`

All public APIs use zero-based addresses.

## License

Licensed under the MIT license. See [`LICENSE`](LICENSE).
