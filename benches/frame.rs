use criterion::{BatchSize, Criterion, black_box};
use libmodbus::command;
use libmodbus::data_model::{DataModel, DataModelConfig};
use libmodbus::frame::rtu::RtuFrame;
use libmodbus::frame::tcp::TcpFrame;
use libmodbus::frame::{FrameType, Pdu};
use libmodbus::protocol::FunctionCode;

fn setup_data_model() -> DataModel {
    let mut data = DataModel::with_config(DataModelConfig {
        max_holding_registers: 125,
        ..Default::default()
    });

    // 0xD5 = 11010101 (LSB first)
    data.write_coil(0, true);
    data.write_coil(1, false);
    data.write_coil(2, true);
    data.write_coil(3, false);
    data.write_coil(4, true);
    data.write_coil(5, false);
    data.write_coil(6, true);
    data.write_coil(7, true);

    for index in 0..125u16 {
        data.write_holding_register(index, index.wrapping_mul(7).wrapping_add(0x0300));
    }

    data
}

fn build_rtu_request(slave_address: u8, function_code: FunctionCode, data: Vec<u8>) -> Vec<u8> {
    let pdu = Pdu::new_request(function_code, data);
    RtuFrame::new(slave_address, pdu).serialize()
}

fn build_tcp_request(
    transaction_id: u16,
    unit_id: u8,
    function_code: FunctionCode,
    data: Vec<u8>,
) -> Vec<u8> {
    let pdu = Pdu::new_request(function_code, data);
    TcpFrame::new(transaction_id, unit_id, pdu).serialize()
}

pub fn bench_rtu_frame_deserialize(c: &mut Criterion) {
    let mut group = c.benchmark_group("frame/rtu");
    let request = build_rtu_request(1, FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);

    group.bench_function("deserialize_fc01_request", |b| {
        // Measures RTU frame parsing without CRC verification.
        b.iter(|| {
            black_box(
                RtuFrame::deserialize(FrameType::Request, black_box(request.as_slice())).unwrap(),
            );
        });
    });

    group.finish();
}

pub fn bench_rtu_crc_validation(c: &mut Criterion) {
    let mut group = c.benchmark_group("frame/crc");
    let request = build_rtu_request(
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );
    let frame = RtuFrame::deserialize(FrameType::Request, request.as_slice()).unwrap();

    group.bench_function("validate_rtu_crc_fc03_request", |b| {
        // Measures checksum validation after deserialization.
        b.iter(|| {
            black_box(
                black_box(&frame)
                    .validate_checksum(black_box(request.as_slice()))
                    .unwrap(),
            );
        });
    });

    group.finish();
}

pub fn bench_tcp_frame_deserialize(c: &mut Criterion) {
    let mut group = c.benchmark_group("frame/tcp");
    let request = build_tcp_request(
        1,
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );

    group.bench_function("deserialize_fc03_request", |b| {
        // Measures MBAP header parsing plus TCP PDU extraction.
        b.iter(|| {
            black_box(TcpFrame::deserialize(black_box(request.as_slice())).unwrap());
        });
    });

    group.finish();
}

pub fn bench_command_execute(c: &mut Criterion) {
    let mut group = c.benchmark_group("command/execute");

    let mut fc03_data = setup_data_model();
    let fc03_request = Pdu::new_request(
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );
    group.bench_function("fc03_read_holding_registers", |b| {
        // Measures command dispatch for a common register read.
        b.iter(|| {
            black_box(command::execute(
                black_box(&mut fc03_data),
                black_box(&fc03_request),
            ));
        });
    });

    let mut large_fc03_data = setup_data_model();
    let large_fc03_request = Pdu::new_request(
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x7D],
    );
    group.bench_function("fc03_read_holding_registers_125", |b| {
        // Measures large PDU generation for the maximum standard FC03 quantity.
        b.iter(|| {
            black_box(command::execute(
                black_box(&mut large_fc03_data),
                black_box(&large_fc03_request),
            ));
        });
    });

    let fc06_request = Pdu::new_request(
        FunctionCode::WriteSingleRegister,
        vec![0x00, 0x0A, 0x12, 0x34],
    );
    group.bench_function("fc06_write_single_register_batched", |b| {
        // Measures write-path dispatch with a fresh data model each iteration.
        b.iter_batched(
            setup_data_model,
            |mut data| {
                black_box(command::execute(
                    black_box(&mut data),
                    black_box(&fc06_request),
                ));
            },
            BatchSize::SmallInput,
        );
    });

    group.finish();
}
