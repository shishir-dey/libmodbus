libmodbus - C++ Modbus Protocol Implementation
==============================================

.. image:: https://github.com/shishir-dey/libmodbus/actions/workflows/cmake-single-platform.yml/badge.svg
   :target: https://github.com/shishir-dey/libmodbus/actions/workflows/cmake-single-platform.yml
   :alt: CMake Build Status

.. image:: https://github.com/shishir-dey/libmodbus/actions/workflows/sphinx.yml/badge.svg
   :target: https://github.com/shishir-dey/libmodbus/actions/workflows/sphinx.yml
   :alt: Documentation Status

.. image:: https://img.shields.io/badge/license-MIT-blue.svg
   :target: https://opensource.org/licenses/MIT
   :alt: License: MIT

A comprehensive C++ implementation of the Modbus protocol supporting RTU, ASCII, and TCP variants.
The library provides a clean, modern C++ interface for building Modbus servers and clients with
full compliance to Modbus specifications.

🚀 **Key Features**
-------------------

- **Complete Function Code Support**: All standard Modbus function codes (FC 01-08, 15-16)
- **Multiple Transport Protocols**: RTU (complete), ASCII and TCP (planned)
- **Standards Compliance**: Full compliance with Modbus Application Protocol V1.1b3
- **Modern C++ Design**: C++17 compatible with RAII principles and type safety
- **Configurable Data Model**: Supports standard and custom Modbus limits
- **Comprehensive Testing**: Full unit test coverage with Google Test
- **Header-Only Design**: Easy integration into existing projects
- **Cross-Platform**: Works on Windows, Linux, and macOS

📋 **Supported Function Codes**
-------------------------------

==== ================================= ==========
Code Function                          Status
==== ================================= ==========
01   Read Coils                        ✅ Complete
02   Read Discrete Inputs              ✅ Complete
03   Read Holding Registers            ✅ Complete
04   Read Input Registers              ✅ Complete
05   Write Single Coil                 ✅ Complete
06   Write Single Register             ✅ Complete
08   Diagnostics                       ✅ Complete
15   Write Multiple Coils              ✅ Complete
16   Write Multiple Registers          ✅ Complete
==== ================================= ==========

🏗️ **Architecture**
-------------------

The library is built around several key components:

* **ModbusDataModel**: Stores all four types of Modbus data (coils, inputs, registers)
* **ModbusCommand**: Command pattern implementation for function codes
* **ModbusFrame**: Protocol Data Unit and transport-specific frames
* **ModbusBaseServer**: Abstract server base with protocol-specific implementations
* **Checksum**: Utilities for CRC-16 and LRC calculations

📖 **Documentation Sections**
-----------------------------

.. toctree::
   :maxdepth: 2
   :caption: User Guide

   overview
   examples
   testing

.. toctree::
   :maxdepth: 2
   :caption: API Reference

   api/index
   api/core_classes
   api/enumerations
   api/functions

🚀 **Quick Start**
------------------

**Building the Library**

.. code-block:: bash

   mkdir build && cd build
   cmake ..
   make

**Basic Server Example**

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   int main() {
       // Create RTU server
       ModbusRtuServer server;
       
       // Initialize some data
       server.data.writeCoil(0, true);
       server.data.writeHoldingRegister(0, 0x1234);
       
       // Process a request (normally from serial port)
       std::vector<uint8_t> request = {0x01, 0x01, 0x00, 0x00, 0x00, 0x08, 0x3D, 0xCC};
       auto response = server.process(request);
       
       return 0;
   }

📦 **Installation**
-------------------

**Prerequisites**

* C++17 compatible compiler
* CMake 3.10 or higher
* Google Test (included as submodule)

**Clone and Build**

.. code-block:: bash

   git clone --recursive https://github.com/shishir-dey/libmodbus.git
   cd libmodbus
   mkdir build && cd build
   cmake ..
   make

**Running Tests**

.. code-block:: bash

   make test

📄 **License**
--------------

This project is licensed under the MIT License - see the `LICENSE <https://github.com/shishir-dey/libmodbus/blob/main/LICENSE>`_ file for details.

🤝 **Contributing**
-------------------

Contributions are welcome! Please feel free to submit a Pull Request.

📞 **Support**
--------------

For questions, issues, or suggestions, please open an issue on the 
`GitHub repository <https://github.com/shishir-dey/libmodbus>`_.

**Version**: |version|

**Last Updated**: |today|

Indices and tables
==================

* :ref:`genindex`
* :ref:`search` 