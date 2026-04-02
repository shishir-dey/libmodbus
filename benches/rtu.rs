use criterion::{Criterion, black_box};
use libmodbus::data_model::{DataModel, DataModelConfig};
use libmodbus::frame::Pdu;
use libmodbus::frame::rtu::RtuFrame;
use libmodbus::protocol::FunctionCode;
use libmodbus::server::ModbusServer;
use libmodbus::server::rtu::RtuServer;

fn setup_rtu_server() -> RtuServer {
    let mut server = RtuServer::with_data_model(
        1,
        DataModel::with_config(DataModelConfig {
            max_holding_registers: 125,
            ..Default::default()
        }),
    );

    // 0xD5 = 11010101 (LSB first)
    server.data.write_coil(0, true);
    server.data.write_coil(1, false);
    server.data.write_coil(2, true);
    server.data.write_coil(3, false);
    server.data.write_coil(4, true);
    server.data.write_coil(5, false);
    server.data.write_coil(6, true);
    server.data.write_coil(7, true);

    server.data.write_holding_register(0, 0x1234);
    server.data.write_holding_register(1, 0x5678);

    for index in 0..125u16 {
        server
            .data
            .write_holding_register(index, index.wrapping_mul(3).wrapping_add(0x0100));
    }

    server
}

fn build_rtu_request(slave_address: u8, function_code: FunctionCode, data: Vec<u8>) -> Vec<u8> {
    let pdu = Pdu::new_request(function_code, data);
    RtuFrame::new(slave_address, pdu).serialize()
}

pub fn bench_rtu_end_to_end(c: &mut Criterion) {
    let mut group = c.benchmark_group("rtu/end_to_end");

    let mut fc01_server = setup_rtu_server();
    let fc01_request = build_rtu_request(1, FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
    group.bench_function("process_fc01_read_coils_valid", |b| {
        // Measures full RTU request handling for a small FC01 read.
        b.iter(|| {
            black_box(fc01_server.process(black_box(fc01_request.as_slice())));
        });
    });

    let mut fc03_server = setup_rtu_server();
    let fc03_request = build_rtu_request(
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );
    group.bench_function("process_fc03_read_holding_registers_valid", |b| {
        // Measures full RTU request handling for a common FC03 register read.
        b.iter(|| {
            black_box(fc03_server.process(black_box(fc03_request.as_slice())));
        });
    });

    let mut large_fc03_server = setup_rtu_server();
    let large_fc03_request = build_rtu_request(
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x7D],
    );
    group.bench_function("process_fc03_read_holding_registers_125", |b| {
        // Measures response generation for the maximum standard FC03 quantity.
        b.iter(|| {
            black_box(large_fc03_server.process(black_box(large_fc03_request.as_slice())));
        });
    });

    group.finish();
}

pub fn bench_rtu_edge_cases(c: &mut Criterion) {
    let mut group = c.benchmark_group("rtu/edge_cases");

    let mut invalid_crc_server = setup_rtu_server();
    let mut invalid_crc_request =
        build_rtu_request(1, FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
    let last = invalid_crc_request.len() - 1;
    invalid_crc_request[last] ^= 0xFF;
    group.bench_function("process_invalid_crc", |b| {
        // Measures reject-path cost after frame parsing on a bad CRC.
        b.iter(|| {
            black_box(invalid_crc_server.process(black_box(invalid_crc_request.as_slice())));
        });
    });

    let mut wrong_slave_server = setup_rtu_server();
    let wrong_slave_request =
        build_rtu_request(2, FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
    group.bench_function("process_wrong_slave_address", |b| {
        // Measures reject-path cost for a valid frame addressed to another slave.
        b.iter(|| {
            black_box(wrong_slave_server.process(black_box(wrong_slave_request.as_slice())));
        });
    });

    group.finish();
}
