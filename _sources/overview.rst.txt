Overview
========

libmodbus is a comprehensive C++ implementation of the Modbus protocol that provides a clean,
modern interface for building Modbus servers and clients. This library supports multiple
transport protocols and maintains full compliance with Modbus specifications.

📖 **What is Modbus?**
----------------------

Modbus is a serial communication protocol developed in 1979 for use with programmable logic
controllers (PLCs). It has become a de facto standard communication protocol for industrial
electronic devices and is widely used in industrial automation systems.

Key characteristics of Modbus:

* **Simple**: Easy to implement and understand
* **Reliable**: Robust error detection and handling
* **Widely Supported**: Available on many industrial devices
* **Open Standard**: Freely available specification

🏗️ **Library Architecture**
---------------------------

The libmodbus library is designed using modern C++ principles and patterns:

Component Overview
~~~~~~~~~~~~~~~~~~

.. doxygenclass:: ModbusDataModel
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: ModbusBaseServer
   :project: libmodbus
   :members:
   :undoc-members:

Data Model
~~~~~~~~~~

The library implements the four standard Modbus data types:

1. **Coils** (Read/Write Discrete Outputs)
   - Boolean values that can be read and written
   - Typically control physical outputs like relays or indicators
   - Standard limit: 2000 coils

2. **Discrete Inputs** (Read-Only Discrete Inputs)
   - Boolean values that can only be read
   - Typically represent physical input states like switch positions
   - Standard limit: 2000 discrete inputs

3. **Holding Registers** (Read/Write 16-bit Registers)
   - 16-bit values that can be read and written
   - Store configuration parameters, setpoints, or other control data
   - Standard limit: 125 registers

4. **Input Registers** (Read-Only 16-bit Registers)
   - 16-bit values that can only be read
   - Typically contain measured values like temperature or pressure
   - Standard limit: 125 registers

🌐 **Supported Protocols**
--------------------------

RTU (Remote Terminal Unit)
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenclass:: ModbusRtuServer
   :project: libmodbus
   :members:

RTU is the most common Modbus variant for serial communication:

* **Binary encoding** for compact transmission
* **CRC-16 error detection** for data integrity
* **Variable frame length** based on function code
* **No frame delimiters** - relies on timing gaps

Frame Structure:

.. code-block:: text

   ┌─────────────┬──────────────┬──────────────┬─────────────┐
   │ Slave Addr  │ Function     │ Data         │ CRC Check   │
   │ (1 byte)    │ Code (1 byte)│ (0-252 bytes)│ (2 bytes)   │
   └─────────────┴──────────────┴──────────────┴─────────────┘

ASCII (Planned)
~~~~~~~~~~~~~~~

ASCII variant uses hexadecimal character encoding:

* **Human-readable** transmission format
* **LRC error detection** (Longitudinal Redundancy Check)
* **Start/end delimiters** (: and CRLF)
* **Longer frames** due to character encoding

TCP (Planned)
~~~~~~~~~~~~~

TCP variant for Ethernet networks:

* **MBAP header** for transaction management
* **No checksum needed** (TCP provides reliability)
* **Connection-oriented** communication

⚙️ **Function Codes**
---------------------

The library implements all standard Modbus function codes:

Read Functions
~~~~~~~~~~~~~~

.. doxygenclass:: ReadCoilCommand
   :project: libmodbus
   :members:

.. doxygenclass:: ReadDiscreteInputCommand
   :project: libmodbus
   :members:

.. doxygenclass:: ReadHoldingRegisterCommand
   :project: libmodbus
   :members:

.. doxygenclass:: ReadInputRegisterCommand
   :project: libmodbus
   :members:

Write Functions
~~~~~~~~~~~~~~~

.. doxygenclass:: WriteCoilCommand
   :project: libmodbus
   :members:

.. doxygenclass:: WriteHoldingRegisterCommand
   :project: libmodbus
   :members:

.. doxygenclass:: WriteMultipleCoilsCommand
   :project: libmodbus
   :members:

.. doxygenclass:: WriteMultipleRegistersCommand
   :project: libmodbus
   :members:

Diagnostic Functions
~~~~~~~~~~~~~~~~~~~~

.. doxygenclass:: DiagnosticsCommand
   :project: libmodbus
   :members:

🔧 **Frame Processing**
-----------------------

Frame Types
~~~~~~~~~~~

.. doxygenenum:: ModbusFrameType
   :project: libmodbus

.. doxygenclass:: ModbusFrame
   :project: libmodbus
   :members:

.. doxygenclass:: ModbusRtuFrame
   :project: libmodbus
   :members:

🛡️ **Error Handling**
---------------------

Exception Codes
~~~~~~~~~~~~~~~

.. doxygenenum:: ModbusExceptionCode
   :project: libmodbus

The library provides comprehensive error handling through exception responses:

* **Validation**: All requests are validated for proper format and ranges
* **Address Checking**: Ensures addresses are within valid ranges
* **Quantity Limits**: Enforces Modbus specification limits
* **CRC Verification**: Validates frame integrity for RTU protocol

🧮 **Checksum Utilities**
-------------------------

.. doxygenclass:: Checksum
   :project: libmodbus
   :members:

The library includes robust checksum implementations:

* **CRC-16**: Standard Modbus CRC using polynomial 0xA001
* **LRC**: Longitudinal Redundancy Check for ASCII protocol

📊 **Standards Compliance**
--------------------------

This implementation is fully compliant with:

* **Modbus Application Protocol Specification V1.1b3**
* **Modbus over Serial Line Specification and Implementation Guide V1.02**
* **Modbus Messaging on TCP/IP Implementation Guide V1.0b** (planned)

Compliance features:

* Correct function code implementations
* Proper exception handling
* Standard quantity limits
* Accurate checksum calculations
* Frame format compliance

🚀 **Performance Features**
--------------------------

* **Vector-based storage** for efficient data access
* **RAII principles** for automatic resource management
* **Move semantics** for efficient data transfer
* **Template-based design** for compile-time optimization
* **Minimal memory overhead** with configurable limits

🔧 **Extensibility**
--------------------

The library is designed for easy extension:

* **Command Pattern**: Easy to add new function codes
* **Virtual interfaces**: Simple to add new transport protocols
* **Configurable limits**: Adaptable to different applications
* **Modular design**: Components can be used independently 