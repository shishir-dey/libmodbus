Core Classes
============

This section provides detailed documentation for the core classes in libmodbus.

📊 **Data Model Classes**
-------------------------

ModbusDataModel
~~~~~~~~~~~~~~~

The primary data storage class for all Modbus data types.

.. doxygenclass:: ModbusDataModel
   :project: libmodbus
   :members:
   :undoc-members:

🖥️ **Server Classes**
---------------------

ModbusBaseServer
~~~~~~~~~~~~~~~~

Abstract base class for all Modbus server implementations.

.. doxygenclass:: ModbusBaseServer
   :project: libmodbus
   :members:
   :undoc-members:

ModbusRtuServer
~~~~~~~~~~~~~~~

RTU protocol-specific server implementation.

.. doxygenclass:: ModbusRtuServer
   :project: libmodbus
   :members:
   :undoc-members:

🖼️ **Frame Classes**
--------------------

ModbusFrame
~~~~~~~~~~~

Core Protocol Data Unit (PDU) implementation.

.. doxygenclass:: ModbusFrame
   :project: libmodbus
   :members:
   :undoc-members:

ModbusRtuFrame
~~~~~~~~~~~~~~

RTU protocol-specific frame implementation.

.. doxygenclass:: ModbusRtuFrame
   :project: libmodbus
   :members:
   :undoc-members:

ModbusAsciiFrame
~~~~~~~~~~~~~~~~

ASCII protocol frame implementation (planned).

.. doxygenclass:: ModbusAsciiFrame
   :project: libmodbus
   :members:
   :undoc-members:

ModbusTcpFrame
~~~~~~~~~~~~~~

TCP protocol frame implementation (planned).

.. doxygenclass:: ModbusTcpFrame
   :project: libmodbus
   :members:
   :undoc-members:

MbapHeader
~~~~~~~~~~

MBAP header for Modbus TCP frames.

.. doxygenclass:: MbapHeader
   :project: libmodbus
   :members:
   :undoc-members:

🎮 **Command Classes**
---------------------

ModbusCommand
~~~~~~~~~~~~~

Abstract base class for all command implementations.

.. doxygenclass:: ModbusCommand
   :project: libmodbus
   :members:
   :undoc-members:

Read Command Classes
~~~~~~~~~~~~~~~~~~~~

.. doxygenclass:: ReadCoilCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: ReadDiscreteInputCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: ReadHoldingRegisterCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: ReadInputRegisterCommand
   :project: libmodbus
   :members:
   :undoc-members:

Write Command Classes
~~~~~~~~~~~~~~~~~~~~~

.. doxygenclass:: WriteCoilCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: WriteHoldingRegisterCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: WriteMultipleCoilsCommand
   :project: libmodbus
   :members:
   :undoc-members:

.. doxygenclass:: WriteMultipleRegistersCommand
   :project: libmodbus
   :members:
   :undoc-members:

Diagnostic Command Classes
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenclass:: DiagnosticsCommand
   :project: libmodbus
   :members:
   :undoc-members:

🧮 **Utility Classes**
----------------------

Checksum
~~~~~~~~

Static utility class for checksum calculations.

.. doxygenclass:: Checksum
   :project: libmodbus
   :members:
   :undoc-members: 