Functions
=========

This section documents all functions available in the libmodbus library.

🧮 **Checksum Functions**
-------------------------

CRC-16 Calculation
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: Checksum::calculateCRC16
   :project: libmodbus

LRC Calculation
~~~~~~~~~~~~~~~

.. doxygenfunction:: Checksum::calculateLRC
   :project: libmodbus

📊 **Data Model Functions**
---------------------------

Read Functions
~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusDataModel::readCoil
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::readDiscreteInput
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::readHoldingRegister
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::readInputRegister
   :project: libmodbus

Write Functions
~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusDataModel::writeCoil
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::writeHoldingRegister
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::writeMultipleCoils
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::writeMultipleHoldingRegisters
   :project: libmodbus

Set Functions (Testing/Initialization)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusDataModel::setDiscreteInput
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::setInputRegister
   :project: libmodbus

Size Getters
~~~~~~~~~~~~

.. doxygenfunction:: ModbusDataModel::getMaxCoils
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::getMaxDiscreteInputs
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::getMaxHoldingRegisters
   :project: libmodbus

.. doxygenfunction:: ModbusDataModel::getMaxInputRegisters
   :project: libmodbus

🖼️ **Frame Processing Functions**
---------------------------------

RTU Frame Functions
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusRtuFrame::serialize
   :project: libmodbus

.. doxygenfunction:: ModbusRtuFrame::deserialize
   :project: libmodbus

ASCII Frame Functions
~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusAsciiFrame::serialize
   :project: libmodbus

.. doxygenfunction:: ModbusAsciiFrame::deserialize
   :project: libmodbus

TCP Frame Functions
~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusTcpFrame::serialize
   :project: libmodbus

.. doxygenfunction:: ModbusTcpFrame::deserialize
   :project: libmodbus

🖥️ **Server Functions**
-----------------------

Request Processing
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusBaseServer::process
   :project: libmodbus

.. doxygenfunction:: ModbusRtuServer::process
   :project: libmodbus

🎮 **Command Validation Functions**
----------------------------------

Generic Validation
~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusCommand::validateQuantity
   :project: libmodbus

.. doxygenfunction:: ModbusCommand::validateAddress
   :project: libmodbus

Function-Specific Validation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusCommand::validateReadCoilsQuantity
   :project: libmodbus

.. doxygenfunction:: ModbusCommand::validateReadRegistersQuantity
   :project: libmodbus

.. doxygenfunction:: ModbusCommand::validateWriteMultipleCoilsQuantity
   :project: libmodbus

.. doxygenfunction:: ModbusCommand::validateWriteMultipleRegistersQuantity
   :project: libmodbus

Command Execution
~~~~~~~~~~~~~~~~~

.. doxygenfunction:: ModbusCommand::execute
   :project: libmodbus

.. doxygenfunction:: ReadCoilCommand::execute
   :project: libmodbus

.. doxygenfunction:: ReadDiscreteInputCommand::execute
   :project: libmodbus

.. doxygenfunction:: ReadHoldingRegisterCommand::execute
   :project: libmodbus

.. doxygenfunction:: ReadInputRegisterCommand::execute
   :project: libmodbus

.. doxygenfunction:: WriteCoilCommand::execute
   :project: libmodbus

.. doxygenfunction:: WriteHoldingRegisterCommand::execute
   :project: libmodbus

.. doxygenfunction:: WriteMultipleCoilsCommand::execute
   :project: libmodbus

.. doxygenfunction:: WriteMultipleRegistersCommand::execute
   :project: libmodbus

.. doxygenfunction:: DiagnosticsCommand::execute
   :project: libmodbus 