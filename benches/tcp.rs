use criterion::{Criterion, black_box};
use libmodbus::data_model::{DataModel, DataModelConfig};
use libmodbus::frame::Pdu;
use libmodbus::frame::tcp::TcpFrame;
use libmodbus::protocol::FunctionCode;
use libmodbus::server::ModbusServer;
use libmodbus::server::tcp::TcpServer;

fn setup_tcp_server() -> TcpServer {
    let mut server = TcpServer::with_data_model(
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
            .write_holding_register(index, index.wrapping_mul(5).wrapping_add(0x0200));
    }

    server
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

pub fn bench_tcp_end_to_end(c: &mut Criterion) {
    let mut group = c.benchmark_group("tcp/end_to_end");

    let mut fc03_server = setup_tcp_server();
    let fc03_request = build_tcp_request(
        1,
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x02],
    );
    group.bench_function("process_fc03_read_holding_registers_valid", |b| {
        // Measures full TCP request handling for a common FC03 register read.
        b.iter(|| {
            black_box(fc03_server.process(black_box(fc03_request.as_slice())));
        });
    });

    let mut fc01_server = setup_tcp_server();
    let fc01_request =
        build_tcp_request(2, 1, FunctionCode::ReadCoils, vec![0x00, 0x00, 0x00, 0x08]);
    group.bench_function("process_fc01_read_coils_valid", |b| {
        // Measures full TCP request handling for a small FC01 read.
        b.iter(|| {
            black_box(fc01_server.process(black_box(fc01_request.as_slice())));
        });
    });

    let mut large_fc03_server = setup_tcp_server();
    let large_fc03_request = build_tcp_request(
        3,
        1,
        FunctionCode::ReadHoldingRegisters,
        vec![0x00, 0x00, 0x00, 0x7D],
    );
    group.bench_function("process_fc03_read_holding_registers_125", |b| {
        // Measures MBAP parsing plus large FC03 response generation.
        b.iter(|| {
            black_box(large_fc03_server.process(black_box(large_fc03_request.as_slice())));
        });
    });

    group.finish();
}
