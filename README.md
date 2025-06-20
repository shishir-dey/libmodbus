<div align="center">
<picture>
  <source media="(prefers-color-scheme: dark)" srcset="assets/libmodbus-dark.svg" width="1280">
  <source media="(prefers-color-scheme: light)" srcset="assets/libmodbus-light.svg" width="1280">
  <img alt="libmodbus logo" src="assets/libmodbus-light.svg" width="1280">
</picture>

# libmodbus - C++ Modbus Protocol Implementation

A comprehensive C++ implementation of the Modbus protocol supporting RTU, ASCII, and TCP variants. The library provides a clean, modern C++ interface for building Modbus servers and clients with full compliance to Modbus specifications.

</div>

## 🚀 Features

- **Complete Function Code Support**: All standard Modbus function codes (FC 01-08, 15-16)
- **Multiple Transport Protocols**: RTU (complete), ASCII and TCP (planned)
- **Standards Compliance**: Full compliance with Modbus Application Protocol V1.1b3
- **Modern C++ Design**: C++17 compatible with RAII principles and type safety
- **Configurable Data Model**: Supports standard and custom Modbus limits
- **Comprehensive Testing**: Full unit test coverage with Google Test

## 📊 Build Status

[![CMake on a single platform](https://github.com/shishir-dey/libmodbus/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/shishir-dey/libmodbus/actions/workflows/cmake-single-platform.yml)
[![Generate Sphinx documentation](https://github.com/shishir-dey/libmodbus/actions/workflows/sphinx.yml/badge.svg)](https://github.com/shishir-dey/libmodbus/actions/workflows/sphinx.yml)
[![Pages build & deployment](https://github.com/shishir-dey/libmodbus/actions/workflows/pages/pages-build-deployment/badge.svg)](https://github.com/shishir-dey/libmodbus/actions/workflows/pages/pages-build-deployment)

## 🚀 Quick Start

### Building the Library

```bash
git clone --recursive https://github.com/shishir-dey/libmodbus.git
cd libmodbus
mkdir build && cd build
cmake ..
make
```

### Running Tests

```bash
cd build
make test
```

## 📖 Documentation

- **[Complete Documentation](https://shishir-dey.github.io/libmodbus/)** - Full API reference and guides
- **[API Reference](https://shishir-dey.github.io/libmodbus/api/)** - Detailed class and function documentation  
- **[Examples](https://shishir-dey.github.io/libmodbus/examples.html)** - Practical usage examples
- **[Testing Guide](https://shishir-dey.github.io/libmodbus/testing.html)** - Testing framework and guidelines

### Generating Documentation Locally

```bash
cd docs
pip install -r requirements.txt
make html
```
