//! Frame serialization/deserialization round-trip tests.

use libmodbuzz::checksum;
use libmodbuzz::frame::rtu::RtuFrame;
use libmodbuzz::frame::tcp::TcpFrame;
use libmodbuzz::frame::{FrameType, Pdu};
use libmodbuzz::protocol::{ExceptionCode, FunctionCode};

// ── RTU Frame Tests ─────────────────────────────────────────────────────

#[test]
fn rtu_request_round_trip() {
    let pdu = Pdu::new_request(
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x0A],
    );
    let frame = RtuFrame::new(0x01, pdu);
    let bytes = frame.serialize();

    let parsed = RtuFrame::deserialize(FrameType::Request, &bytes).unwrap();
    assert_eq!(parsed.slave_address, 0x01);
    assert_eq!(parsed.pdu.function_code, FunctionCode::ReadHoldingRegisters);
    assert_eq!(parsed.pdu.data, vec![0x00, 0x00, 0x00, 0x0A]);

    // CRC should validate
    assert!(parsed.validate_checksum(&bytes).is_ok());
}

#[test]
fn rtu_response_round_trip() {
    let pdu = Pdu::new_response(
        FunctionCode::ReadCoils,
        vec![0x01, 0xD5], // byte count + data
    );
    let frame = RtuFrame::new(0x01, pdu);
    let bytes = frame.serialize();

    let parsed = RtuFrame::deserialize(FrameType::Response, &bytes).unwrap();
    assert_eq!(parsed.pdu.function_code, FunctionCode::ReadCoils);
    assert!(parsed.validate_checksum(&bytes).is_ok());
}

#[test]
fn rtu_exception_round_trip() {
    let pdu = Pdu::new_exception(FunctionCode::ReadCoils, ExceptionCode::IllegalDataAddress);
    let frame = RtuFrame::new(0x01, pdu);
    let bytes = frame.serialize();

    assert_eq!(bytes[1], 0x81); // FC | 0x80
    assert_eq!(bytes[2], 0x02); // exception code

    let parsed = RtuFrame::deserialize(FrameType::Request, &bytes).unwrap();
    assert_eq!(parsed.pdu.frame_type, FrameType::Exception);
    assert_eq!(parsed.pdu.function_code, FunctionCode::ReadCoils);
    assert_eq!(
        parsed.pdu.exception_code,
        Some(ExceptionCode::IllegalDataAddress)
    );
}

#[test]
fn rtu_known_crc_vectors() {
    // Test against specific known CRC values from the C++ test suite
    let test_vectors: Vec<(&[u8], u16)> = vec![
        (&[0x01, 0x01, 0x00, 0x00, 0x00, 0x08], 0xCC3D),
        (&[0x01, 0x03, 0x00, 0x00, 0x00, 0x02], 0x0BC4),
        (&[0x01, 0x04, 0x00, 0x00, 0x00, 0x02], 0xCB71),
        (&[0x01, 0x02, 0x00, 0x00, 0x00, 0x03], 0x0B38),
    ];

    for (data, expected_crc) in test_vectors {
        assert_eq!(
            checksum::crc16(data),
            expected_crc,
            "CRC mismatch for {:02X?}",
            data
        );
    }
}

// ── TCP Frame Tests ─────────────────────────────────────────────────────

#[test]
fn tcp_request_round_trip() {
    let pdu = Pdu::new_request(
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x0A],
    );
    let frame = TcpFrame::new(1, 0x01, pdu);
    let bytes = frame.serialize();

    // MBAP header (7) + FC (1) + data (4) = 12 bytes
    assert_eq!(bytes.len(), 12);

    let parsed = TcpFrame::deserialize(&bytes).unwrap();
    assert_eq!(parsed.header.transaction_id, 1);
    assert_eq!(parsed.header.unit_id, 0x01);
    assert_eq!(parsed.header.protocol_id, 0x0000);
    assert_eq!(parsed.pdu.function_code, FunctionCode::ReadHoldingRegisters);
    assert_eq!(parsed.pdu.data, vec![0x00, 0x00, 0x00, 0x0A]);
}

#[test]
fn tcp_exception_round_trip() {
    let pdu = Pdu::new_exception(
        FunctionCode::WriteSingleCoil,
        ExceptionCode::IllegalFunction,
    );
    let frame = TcpFrame::new(42, 0x03, pdu);
    let bytes = frame.serialize();

    let parsed = TcpFrame::deserialize(&bytes).unwrap();
    assert_eq!(parsed.header.transaction_id, 42);
    assert_eq!(parsed.header.unit_id, 0x03);
    assert_eq!(parsed.pdu.frame_type, FrameType::Exception);
    assert_eq!(parsed.pdu.function_code, FunctionCode::WriteSingleCoil);
    assert_eq!(
        parsed.pdu.exception_code,
        Some(ExceptionCode::IllegalFunction)
    );
}

#[test]
fn tcp_mbap_length_field() {
    let pdu = Pdu::new_request(FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
    let frame = TcpFrame::new(1, 0x01, pdu);
    let bytes = frame.serialize();

    // Length field should be: unit_id (1) + FC (1) + data (4) = 6
    let length = u16::from(bytes[4]) << 8 | u16::from(bytes[5]);
    assert_eq!(length, 6);
}

// ── Checksum Tests ──────────────────────────────────────────────────────

#[test]
fn lrc_known_vector() {
    // Standard Modbus ASCII LRC test
    let data = [0x01, 0x03, 0x00, 0x00, 0x00, 0x0A];
    let lrc_val = checksum::lrc(&data);
    assert_eq!(lrc_val, 0xF2);
}
