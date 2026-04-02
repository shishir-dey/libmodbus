use libmodbuzz::server::ModbusServer;
use libmodbuzz::server::rtu::RtuServer;

fn setup_server() -> RtuServer {
    let mut server = RtuServer::new(1);

    // Pattern for 0xD5 = 11010101 (LSB first): T,F,T,F,T,F,T,T
    server.data.write_coil(0, true);
    server.data.write_coil(1, false);
    server.data.write_coil(2, true);
    server.data.write_coil(3, false);
    server.data.write_coil(4, true);
    server.data.write_coil(5, false);
    server.data.write_coil(6, true);
    server.data.write_coil(7, true);

    // Holding registers
    server.data.write_holding_register(0, 0x1234);
    server.data.write_holding_register(1, 0x5678);
    server.data.write_holding_register(2, 0x9ABC);

    // Input registers
    server.data.set_input_register(0, 0xDEAD);
    server.data.set_input_register(1, 0xBEEF);

    // Discrete inputs
    server.data.set_discrete_input(0, true);
    server.data.set_discrete_input(1, false);
    server.data.set_discrete_input(2, true);

    server
}

#[test]
fn read_coils_valid_request() {
    let mut server = setup_server();
    let request = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x01, 0x01, 0xD5, 0x90, 0x17];
    assert_eq!(response, expected);
}

#[test]
fn read_holding_registers_valid_request() {
    let mut server = setup_server();
    let request = vec![0x01, 0x03, 0x00, 0x00, 0x00, 0x02, 0xC4, 0x0B];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x03, 0x04, 0x12, 0x34, 0x56, 0x78, 0x81, 0x07];
    assert_eq!(response, expected);
}

#[test]
fn read_input_registers_valid_request() {
    let mut server = setup_server();
    let request = vec![0x01, 0x04, 0x00, 0x00, 0x00, 0x02, 0x71, 0xCB];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x04, 0x04, 0xDE, 0xAD, 0xBE, 0xEF, 0x60, 0x61];
    assert_eq!(response, expected);
}

#[test]
fn invalid_slave_address() {
    let mut server = setup_server();
    let request = vec![0x02, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC];
    let response = server.process(&request);
    // Server should not respond to requests with wrong slave address
    assert!(response.is_none());
}

#[test]
fn invalid_crc() {
    let mut server = setup_server();
    let request = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x00, 0x00];
    let response = server.process(&request);
    // Server should not respond to requests with invalid CRC
    assert!(response.is_none());
}

#[test]
fn read_discrete_inputs_valid_request() {
    let mut server = setup_server();
    let request = vec![0x01, 0x02, 0x00, 0x00, 0x00, 0x03, 0x38, 0x0B];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x02, 0x01, 0x05, 0x61, 0x8B];
    assert_eq!(response, expected);
}

#[test]
fn write_multiple_coils_valid_request() {
    let mut server = setup_server();
    // Write 3 coils starting at address 10: T,F,T = 0x05
    let request = vec![0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x01, 0x05, 0xD7, 0x55];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x35, 0xC8];
    assert_eq!(response, expected);

    // Verify coils were written correctly
    assert_eq!(server.data.read_coil(10), Some(true));
    assert_eq!(server.data.read_coil(11), Some(false));
    assert_eq!(server.data.read_coil(12), Some(true));
}

#[test]
fn write_multiple_registers_valid_request() {
    let mut server = setup_server();
    // Write 2 registers starting at address 5: 0x1122, 0x3344
    let request = vec![
        0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44, 0x82, 0x65,
    ];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x51, 0xC9];
    assert_eq!(response, expected);

    // Verify registers were written correctly
    assert_eq!(server.data.read_holding_register(5), Some(0x1122));
    assert_eq!(server.data.read_holding_register(6), Some(0x3344));
}

#[test]
fn diagnostics_return_query_data() {
    let mut server = setup_server();
    // Diagnostics function 0x0000 (Return Query Data) with data 0x1234
    let request = vec![0x01, 0x08, 0x00, 0x00, 0x12, 0x34, 0xED, 0x7C];
    let response = server.process(&request).unwrap();
    let expected = vec![0x01, 0x08, 0x00, 0x00, 0x12, 0x34, 0xED, 0x7C];
    assert_eq!(response, expected);
}

#[test]
fn write_multiple_coils_invalid_byte_count() {
    let mut server = setup_server();
    // Invalid byte count: should be 1 for 3 coils, but sending 2
    let request = vec![
        0x01, 0x0F, 0x00, 0x0A, 0x00, 0x03, 0x02, 0x05, 0x00, 0xE5, 0x5E,
    ];
    let response = server.process(&request).unwrap();

    // Should return exception response
    assert_eq!(response.len(), 5); // exception frame size
    assert_eq!(response[0], 0x01); // slave address
    assert_eq!(response[1], 0x8F); // exception function code (0x0F | 0x80)
    assert_eq!(response[2], 0x03); // exception code: ILLEGAL_DATA_VALUE
}

#[test]
fn write_multiple_registers_invalid_byte_count() {
    let mut server = setup_server();
    // Invalid byte count: should be 4 for 2 registers, but sending 3
    let request = vec![
        0x01, 0x10, 0x00, 0x05, 0x00, 0x02, 0x03, 0x11, 0x22, 0x33, 0xC8, 0x36,
    ];
    let response = server.process(&request).unwrap();

    // Should return exception response
    assert_eq!(response.len(), 5); // exception frame size
    assert_eq!(response[0], 0x01); // slave address
    assert_eq!(response[1], 0x90); // exception function code (0x10 | 0x80)
    assert_eq!(response[2], 0x03); // exception code: ILLEGAL_DATA_VALUE
}

// ── Additional edge-case tests ───────────────────────────────────────────

#[test]
fn frame_too_short() {
    let mut server = setup_server();
    let request = vec![0x01, 0x01];
    assert!(server.process(&request).is_none());
}

#[test]
fn broadcast_address_no_response() {
    let mut server = setup_server();
    // address 0 = broadcast, CRC doesn't matter here since address check happens first
    // Actually, CRC check happens before address check in our impl, so we need valid CRC
    // but for address 0 with FC 01, the server should still not respond
    // Let's send a valid frame with address 0
    let data_without_crc = vec![0x00, 0x01, 0x00, 0x00, 0x00, 0x08];
    let crc = libmodbuzz::checksum::crc16(&data_without_crc);
    let mut request = data_without_crc;
    request.push((crc & 0xFF) as u8);
    request.push(((crc >> 8) & 0xFF) as u8);
    assert!(server.process(&request).is_none());
}

#[test]
fn read_coils_single_coil() {
    let mut server = setup_server();
    // Read 1 coil at address 0 (should be true)
    let data_without_crc = vec![0x01, 0x01, 0x00, 0x00, 0x00, 0x01];
    let crc = libmodbuzz::checksum::crc16(&data_without_crc);
    let mut request = data_without_crc;
    request.push((crc & 0xFF) as u8);
    request.push(((crc >> 8) & 0xFF) as u8);

    let response = server.process(&request).unwrap();
    assert_eq!(response[2], 0x01); // byte count = 1
    assert_eq!(response[3] & 0x01, 0x01); // coil 0 is true
}
