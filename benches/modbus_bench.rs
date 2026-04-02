mod frame;
mod rtu;
mod tcp;

use criterion::{criterion_group, criterion_main};

criterion_group!(
    frame_benches,
    frame::bench_rtu_frame_deserialize,
    frame::bench_rtu_crc_validation,
    frame::bench_tcp_frame_deserialize,
    frame::bench_command_execute,
);
criterion_group!(
    rtu_benches,
    rtu::bench_rtu_end_to_end,
    rtu::bench_rtu_edge_cases,
);
criterion_group!(tcp_benches, tcp::bench_tcp_end_to_end,);
criterion_main!(frame_benches, rtu_benches, tcp_benches);
