//! Modbus TCP integration tests over real POSIX sockets.
//!
//! Uses `std::net` TCP sockets with one thread acting as the Modbus server
//! and another as the client, communicating over localhost to verify the
//! complete Modbus TCP stack end-to-end.

use std::io::{Read, Write};
use std::net::{TcpListener, TcpStream};
use std::sync::mpsc;
use std::thread;
use std::time::Duration;

use libmodbuzz::frame::Pdu;
use libmodbuzz::frame::tcp::{MbapHeader, TcpFrame};
use libmodbuzz::protocol::FunctionCode;
use libmodbuzz::server::ModbusServer;
use libmodbuzz::server::tcp::TcpServer;

/// Build a Modbus TCP request frame from a transaction ID, unit ID, and PDU data.
fn build_tcp_request(txn_id: u16, unit_id: u8, fc: FunctionCode, data: Vec<u8>) -> Vec<u8> {
    let pdu = Pdu::new_request(fc, data);
    TcpFrame::new(txn_id, unit_id, pdu).serialize()
}

/// Parse a Modbus TCP response from raw bytes.
fn parse_tcp_response(data: &[u8]) -> TcpFrame {
    TcpFrame::deserialize(data).expect("failed to parse TCP response")
}

/// Spawn a Modbus TCP server on a random port, returns (port, shutdown_sender).
///
/// The server accepts one connection, processes requests until the client
/// disconnects, then exits.
fn spawn_server(
    ready_tx: mpsc::Sender<u16>,
    setup_fn: impl FnOnce(&mut TcpServer) + Send + 'static,
) -> thread::JoinHandle<()> {
    thread::spawn(move || {
        // Bind to port 0 → OS picks a free port
        let listener = TcpListener::bind("127.0.0.1:0").expect("failed to bind");
        let port = listener.local_addr().unwrap().port();
        ready_tx.send(port).unwrap();

        // Accept one connection
        let (mut stream, _addr) = listener.accept().expect("failed to accept");
        stream
            .set_read_timeout(Some(Duration::from_secs(5)))
            .unwrap();

        let mut server = TcpServer::new(1);
        setup_fn(&mut server);

        let mut buf = [0u8; 512];

        loop {
            // Read MBAP header first (7 bytes)
            match stream.read(&mut buf[..MbapHeader::SIZE]) {
                Ok(0) => break, // Client disconnected
                Ok(n) if n < MbapHeader::SIZE => break,
                Err(_) => break,
                Ok(_) => {}
            }

            // Parse length from MBAP header to know how much more to read
            let length = u16::from(buf[4]) << 8 | u16::from(buf[5]);
            let remaining = length as usize - 1; // -1 because unit_id is part of header read
            let total = MbapHeader::SIZE + remaining;

            // Read the rest (unit_id already read, need FC + data)
            if remaining > 0 {
                match stream.read_exact(&mut buf[MbapHeader::SIZE..total]) {
                    Ok(_) => {}
                    Err(_) => break,
                }
            }

            // Process through the Modbus TCP server
            if let Some(response) = server.process(&buf[..total]) {
                stream.write_all(&response).expect("failed to write");
                stream.flush().expect("failed to flush");
            }
        }
    })
}

/// Helper: connect a client to the server and return the stream.
fn connect_client(port: u16) -> TcpStream {
    let stream = TcpStream::connect(format!("127.0.0.1:{}", port)).expect("failed to connect");
    stream
        .set_read_timeout(Some(Duration::from_secs(5)))
        .unwrap();
    stream
}

/// Helper: send a request and read the response.
fn send_request(stream: &mut TcpStream, request: &[u8]) -> Vec<u8> {
    stream.write_all(request).expect("write failed");
    stream.flush().expect("flush failed");

    // Read MBAP header
    let mut header_buf = [0u8; MbapHeader::SIZE];
    stream
        .read_exact(&mut header_buf)
        .expect("read header failed");

    let length = u16::from(header_buf[4]) << 8 | u16::from(header_buf[5]);
    let pdu_len = length as usize - 1; // subtract unit_id (already in header)

    // Read remaining PDU bytes
    let mut pdu_buf = vec![0u8; pdu_len];
    stream.read_exact(&mut pdu_buf).expect("read PDU failed");

    // Combine into full frame
    let mut full = header_buf.to_vec();
    full.extend_from_slice(&pdu_buf);
    full
}

// ═══════════════════════════════════════════════════════════════════════
// Tests
// ═══════════════════════════════════════════════════════════════════════

#[test]
fn tcp_read_holding_registers() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |server| {
        server.data.write_holding_register(0, 0xCAFE);
        server.data.write_holding_register(1, 0xBEEF);
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 03: Read 2 holding registers starting at address 0
    let request = build_tcp_request(
        1,
        0x01,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );

    let response_bytes = send_request(&mut client, &request);
    let response = parse_tcp_response(&response_bytes);

    // Verify MBAP header
    assert_eq!(response.header.transaction_id, 1);
    assert_eq!(response.header.unit_id, 0x01);
    assert_eq!(response.header.protocol_id, 0x0000);

    // Verify PDU: byte_count=4, then 0xCAFE, 0xBEEF
    assert_eq!(
        response.pdu.function_code,
        FunctionCode::ReadHoldingRegisters
    );
    assert_eq!(response.pdu.data[0], 0x04); // byte count
    assert_eq!(response.pdu.data[1], 0xCA);
    assert_eq!(response.pdu.data[2], 0xFE);
    assert_eq!(response.pdu.data[3], 0xBE);
    assert_eq!(response.pdu.data[4], 0xEF);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_read_coils() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |server| {
        // Pattern 0xD5 = 11010101 (LSB first): T,F,T,F,T,F,T,T
        server.data.write_coil(0, true);
        server.data.write_coil(1, false);
        server.data.write_coil(2, true);
        server.data.write_coil(3, false);
        server.data.write_coil(4, true);
        server.data.write_coil(5, false);
        server.data.write_coil(6, true);
        server.data.write_coil(7, true);
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 01: Read 8 coils starting at address 0
    let request = build_tcp_request(
        2,
        0x01,
        FunctionCode::ReadCoils,
        vec![0x00, 0x00, 0x00, 0x08],
    );

    let response_bytes = send_request(&mut client, &request);
    let response = parse_tcp_response(&response_bytes);

    assert_eq!(response.header.transaction_id, 2);
    assert_eq!(response.pdu.function_code, FunctionCode::ReadCoils);
    assert_eq!(response.pdu.data[0], 0x01); // byte count
    assert_eq!(response.pdu.data[1], 0xD5); // bit-packed coil values

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_write_single_register_and_read_back() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |_server| {
        // Empty — we'll write via Modbus
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // Step 1: FC 06 — Write register 10 = 0x1234
    let write_req = build_tcp_request(
        1,
        0x01,
        FunctionCode::WriteSingleRegister,
        vec![0x00, 0x0A, 0x12, 0x34],
    );
    let write_resp_bytes = send_request(&mut client, &write_req);
    let write_resp = parse_tcp_response(&write_resp_bytes);

    // Write response should echo the request data
    assert_eq!(
        write_resp.pdu.function_code,
        FunctionCode::WriteSingleRegister
    );
    assert_eq!(write_resp.pdu.data, vec![0x00, 0x0A, 0x12, 0x34]);

    // Step 2: FC 03 — Read it back
    let read_req = build_tcp_request(
        2,
        0x01,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x0A, 0x00, 0x01],
    );
    let read_resp_bytes = send_request(&mut client, &read_req);
    let read_resp = parse_tcp_response(&read_resp_bytes);

    assert_eq!(read_resp.header.transaction_id, 2);
    assert_eq!(
        read_resp.pdu.function_code,
        FunctionCode::ReadHoldingRegisters
    );
    assert_eq!(read_resp.pdu.data[0], 0x02); // byte count = 2
    assert_eq!(read_resp.pdu.data[1], 0x12);
    assert_eq!(read_resp.pdu.data[2], 0x34);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_write_multiple_coils_and_read_back() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |_server| {});

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 15: Write 4 coils at address 0: T, F, T, T = 0x0D (bits 0,2,3)
    let write_req = build_tcp_request(
        10,
        0x01,
        FunctionCode::WriteMultipleCoils,
        vec![
            0x00, 0x00, // start address
            0x00, 0x04, // quantity
            0x01, // byte count
            0x0D, // coil values: 1101 (bits 0,2,3 set)
        ],
    );
    let write_resp_bytes = send_request(&mut client, &write_req);
    let write_resp = parse_tcp_response(&write_resp_bytes);

    assert_eq!(
        write_resp.pdu.function_code,
        FunctionCode::WriteMultipleCoils
    );
    assert_eq!(write_resp.pdu.data, vec![0x00, 0x00, 0x00, 0x04]); // echo addr+qty

    // Read back: FC 01, 4 coils at address 0
    let read_req = build_tcp_request(
        11,
        0x01,
        FunctionCode::ReadCoils,
        vec![0x00, 0x00, 0x00, 0x04],
    );
    let read_resp_bytes = send_request(&mut client, &read_req);
    let read_resp = parse_tcp_response(&read_resp_bytes);

    assert_eq!(read_resp.pdu.function_code, FunctionCode::ReadCoils);
    assert_eq!(read_resp.pdu.data[0], 0x01); // byte count
    assert_eq!(read_resp.pdu.data[1] & 0x0F, 0x0D); // T,F,T,T

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_write_multiple_registers_and_read_back() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |_server| {});

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 16: Write 3 registers at address 0: 0xDEAD, 0xBEEF, 0xCAFE
    let write_req = build_tcp_request(
        20,
        0x01,
        FunctionCode::WriteMultipleRegisters,
        vec![
            0x00, 0x00, // start address
            0x00, 0x03, // quantity
            0x06, // byte count (3 * 2)
            0xDE, 0xAD, // register 0
            0xBE, 0xEF, // register 1
            0xCA, 0xFE, // register 2
        ],
    );
    let write_resp_bytes = send_request(&mut client, &write_req);
    let write_resp = parse_tcp_response(&write_resp_bytes);

    assert_eq!(
        write_resp.pdu.function_code,
        FunctionCode::WriteMultipleRegisters
    );

    // Read back all 3 registers
    let read_req = build_tcp_request(
        21,
        0x01,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x03],
    );
    let read_resp_bytes = send_request(&mut client, &read_req);
    let read_resp = parse_tcp_response(&read_resp_bytes);

    assert_eq!(read_resp.header.transaction_id, 21);
    assert_eq!(read_resp.pdu.data[0], 0x06); // byte count = 6
    assert_eq!(read_resp.pdu.data[1..3], [0xDE, 0xAD]);
    assert_eq!(read_resp.pdu.data[3..5], [0xBE, 0xEF]);
    assert_eq!(read_resp.pdu.data[5..7], [0xCA, 0xFE]);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_read_input_registers() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |server| {
        server.data.set_input_register(0, 0x1111);
        server.data.set_input_register(1, 0x2222);
        server.data.set_input_register(2, 0x3333);
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 04: Read 3 input registers at address 0
    let request = build_tcp_request(
        5,
        0x01,
        FunctionCode::ReadInputRegisters,
        vec![0x00, 0x00, 0x00, 0x03],
    );
    let response_bytes = send_request(&mut client, &request);
    let response = parse_tcp_response(&response_bytes);

    assert_eq!(response.pdu.function_code, FunctionCode::ReadInputRegisters);
    assert_eq!(response.pdu.data[0], 0x06); // byte count = 6
    assert_eq!(response.pdu.data[1..3], [0x11, 0x11]);
    assert_eq!(response.pdu.data[3..5], [0x22, 0x22]);
    assert_eq!(response.pdu.data[5..7], [0x33, 0x33]);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_read_discrete_inputs() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |server| {
        server.data.set_discrete_input(0, true);
        server.data.set_discrete_input(1, false);
        server.data.set_discrete_input(2, true);
        server.data.set_discrete_input(3, true);
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 02: Read 4 discrete inputs at address 0
    let request = build_tcp_request(
        3,
        0x01,
        FunctionCode::ReadDiscreteInputs,
        vec![0x00, 0x00, 0x00, 0x04],
    );
    let response_bytes = send_request(&mut client, &request);
    let response = parse_tcp_response(&response_bytes);

    assert_eq!(response.pdu.function_code, FunctionCode::ReadDiscreteInputs);
    assert_eq!(response.pdu.data[0], 0x01); // byte count
    // Bits: T,F,T,T = 1101 = 0x0D
    assert_eq!(response.pdu.data[1] & 0x0F, 0x0D);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_diagnostics_echo() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |_server| {});

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 08, sub-function 0x0000 (Return Query Data), data 0xABCD
    let request = build_tcp_request(
        42,
        0x01,
        FunctionCode::Diagnostics,
        vec![0x00, 0x00, 0xAB, 0xCD],
    );
    let response_bytes = send_request(&mut client, &request);
    let response = parse_tcp_response(&response_bytes);

    assert_eq!(response.header.transaction_id, 42);
    assert_eq!(response.pdu.function_code, FunctionCode::Diagnostics);
    // Echo: sub-function 0x0000 + data 0xABCD
    assert_eq!(response.pdu.data, vec![0x00, 0x00, 0xAB, 0xCD]);

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_wrong_unit_id_no_response() {
    let (ready_tx, ready_rx) = mpsc::channel();

    // Server with unit_id = 5
    let server_handle = spawn_server(ready_tx, |server| {
        server.unit_id = 5;
        server.data.write_holding_register(0, 0x1234);
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // Send request to unit_id = 1 (server is 5)
    let request = build_tcp_request(
        1,
        0x01, // wrong unit ID
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x01],
    );

    client.write_all(&request).unwrap();
    client.flush().unwrap();

    // Server should NOT respond — expect a read timeout
    client
        .set_read_timeout(Some(Duration::from_millis(200)))
        .unwrap();
    let mut buf = [0u8; 64];
    let result = client.read(&mut buf);

    // Should timeout or get 0 bytes (connection closed)
    match result {
        Ok(0) => {} // Connection closed cleanly
        Err(ref e) if e.kind() == std::io::ErrorKind::WouldBlock => {}
        Err(ref e) if e.kind() == std::io::ErrorKind::TimedOut => {}
        other => panic!("Expected timeout or no data, got: {:?}", other),
    }

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_multiple_transactions_sequential() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |server| {
        for i in 0..10 {
            server.data.write_holding_register(i, i * 100);
        }
    });

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // Send 10 sequential read requests, each reading 1 register
    for i in 0u16..10 {
        let request = build_tcp_request(
            i + 1, // transaction IDs 1..10
            0x01,
            FunctionCode::ReadHoldingRegisters,
            vec![(i >> 8) as u8, (i & 0xFF) as u8, 0x00, 0x01],
        );

        let response_bytes = send_request(&mut client, &request);
        let response = parse_tcp_response(&response_bytes);

        // Verify transaction ID matches
        assert_eq!(response.header.transaction_id, i + 1);

        // Verify register value
        let expected_value = i * 100;
        let actual_value = u16::from(response.pdu.data[1]) << 8 | u16::from(response.pdu.data[2]);
        assert_eq!(
            actual_value, expected_value,
            "Register {} expected {}, got {}",
            i, expected_value, actual_value
        );
    }

    drop(client);
    server_handle.join().unwrap();
}

#[test]
fn tcp_write_single_coil_and_read_back() {
    let (ready_tx, ready_rx) = mpsc::channel();

    let server_handle = spawn_server(ready_tx, |_server| {});

    let port = ready_rx.recv_timeout(Duration::from_secs(5)).unwrap();
    let mut client = connect_client(port);

    // FC 05: Write coil 5 = ON (0xFF00)
    let write_req = build_tcp_request(
        1,
        0x01,
        FunctionCode::WriteSingleCoil,
        vec![0x00, 0x05, 0xFF, 0x00],
    );
    let write_resp_bytes = send_request(&mut client, &write_req);
    let write_resp = parse_tcp_response(&write_resp_bytes);

    assert_eq!(write_resp.pdu.function_code, FunctionCode::WriteSingleCoil);
    assert_eq!(write_resp.pdu.data, vec![0x00, 0x05, 0xFF, 0x00]);

    // Read back: FC 01, 8 coils at address 0
    let read_req = build_tcp_request(
        2,
        0x01,
        FunctionCode::ReadCoils,
        vec![0x00, 0x00, 0x00, 0x08],
    );
    let read_resp_bytes = send_request(&mut client, &read_req);
    let read_resp = parse_tcp_response(&read_resp_bytes);

    assert_eq!(read_resp.pdu.function_code, FunctionCode::ReadCoils);
    // Coil 5 should be set: bit 5 = 0x20
    assert_eq!(read_resp.pdu.data[1] & 0x20, 0x20);
    // Coil 0 should be clear
    assert_eq!(read_resp.pdu.data[1] & 0x01, 0x00);

    drop(client);
    server_handle.join().unwrap();
}
