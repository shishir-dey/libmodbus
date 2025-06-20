API Reference
=============

This section provides comprehensive API documentation for all libmodbus classes, functions,
and data types. The documentation is automatically generated from the source code comments
using Doxygen and Breathe.

📚 **Core Components**
---------------------

Main Library Header
~~~~~~~~~~~~~~~~~~~

.. doxygenfile:: libmodbus.hpp
   :project: libmodbus

📊 **Data Model**
----------------

.. doxygenfile:: ModbusDataModel.hpp
   :project: libmodbus

🗂️ **Protocol Definitions**
---------------------------

.. doxygenfile:: Modbus.hpp
   :project: libmodbus

🖼️ **Frame Processing**
----------------------

.. doxygenfile:: ModbusFrame.hpp
   :project: libmodbus

🎮 **Command Processing**
------------------------

.. doxygenfile:: ModbusCommand.hpp
   :project: libmodbus

🖥️ **Server Implementation**
---------------------------

.. doxygenfile:: ModbusBaseServer.hpp
   :project: libmodbus

🧮 **Checksum Utilities**
-------------------------

.. doxygenfile:: Checksum.hpp
   :project: libmodbus

🔗 **Detailed API Sections**
----------------------------

.. toctree::
   :maxdepth: 2

   core_classes
   enumerations
   functions 