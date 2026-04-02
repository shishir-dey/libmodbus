<div align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="assets/libmodbuzz-dark.png" width="1280">
  <source media="(prefers-color-scheme: light)" srcset="assets/libmodbuzz-light.png" width="1280">
  <img alt="libmodbuzz logo" src="assets/libmodbuzz-light.png" width="1280">
</picture>
</div>

<hr>

A safe, idiomatic Rust implementation of the Modbus protocol supporting RTU and TCP transports. Zero `unsafe`, zero dependencies.

## Usage

```rust
use libmodbuzz::server::rtu::RtuServer;
use libmodbuzz::server::ModbusServer;

let mut server = RtuServer::new(1);
server.data.write_coil(0, true);
server.data.write_holding_register(0, 0x1234);

let request = vec![0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A];
let response = server.process(&request);
```
