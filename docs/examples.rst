Examples
========

This section provides practical examples of using the libmodbus library for various Modbus
operations. All examples assume you have the library built and linked to your project.

🚀 **Basic Server Setup**
-------------------------

Creating a Simple RTU Server
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   int main() {
       // Create an RTU server (responds to slave address 1)
       ModbusRtuServer server;
       
       // Initialize some example data
       server.data.writeCoil(0, true);           // Coil 0 = ON
       server.data.writeCoil(1, false);          // Coil 1 = OFF
       server.data.writeHoldingRegister(0, 1234); // Register 0 = 1234
       server.data.writeHoldingRegister(1, 5678); // Register 1 = 5678
       
       // In a real application, you would read from serial port
       // Here we simulate a request to read 8 coils starting at address 0
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x01,       // Function code (Read Coils)
           0x00, 0x00, // Starting address (0)
           0x00, 0x08, // Quantity (8 coils)
           0x3D, 0xCC  // CRC
       };
       
       // Process the request
       auto response = server.process(request);
       
       // Response will contain the coil values
       if (!response.empty()) {
           printf("Response: ");
           for (uint8_t byte : response) {
               printf("%02X ", byte);
           }
           printf("\n");
       }
       
       return 0;
   }

📖 **Reading Data**
------------------

Reading Coils (Function Code 01)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void readCoilsExample() {
       ModbusRtuServer server;
       
       // Set up some coil data
       server.data.writeCoil(0, true);    // Address 0 = ON
       server.data.writeCoil(1, false);   // Address 1 = OFF
       server.data.writeCoil(2, true);    // Address 2 = ON
       server.data.writeCoil(3, false);   // Address 3 = OFF
       
       // Read Coils request: FC=01, Start=0, Quantity=4
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x01,       // Function code
           0x00, 0x00, // Start address
           0x00, 0x04, // Quantity (4 coils)
           0xFD, 0xCA  // CRC (calculated for this specific request)
       };
       
       auto response = server.process(request);
       
       // Response format: [Address][FC][ByteCount][Data][CRC]
       // Expected: 01 01 01 05 xx xx (coils packed in 1 byte: 0101b = 0x05)
   }

Reading Holding Registers (Function Code 03)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void readHoldingRegistersExample() {
       ModbusRtuServer server;
       
       // Set up register data
       server.data.writeHoldingRegister(0, 0x1234); // 16-bit value
       server.data.writeHoldingRegister(1, 0x5678);
       
       // Read Holding Registers request: FC=03, Start=0, Quantity=2
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x03,       // Function code
           0x00, 0x00, // Start address
           0x00, 0x02, // Quantity (2 registers)
           0xC4, 0x0B  // CRC
       };
       
       auto response = server.process(request);
       
       // Response: [01][03][04][12][34][56][78][CRC][CRC]
       // 4 bytes of data (2 registers × 2 bytes each)
   }

Reading Discrete Inputs (Function Code 02)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void readDiscreteInputsExample() {
       ModbusRtuServer server;
       
       // Set up discrete input data (normally from hardware)
       server.data.setDiscreteInput(0, true);
       server.data.setDiscreteInput(1, false);
       server.data.setDiscreteInput(2, true);
       
       // Read Discrete Inputs request: FC=02
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x02,       // Function code
           0x00, 0x00, // Start address
           0x00, 0x03, // Quantity (3 inputs)
           0x24, 0x0A  // CRC
       };
       
       auto response = server.process(request);
   }

Reading Input Registers (Function Code 04)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void readInputRegistersExample() {
       ModbusRtuServer server;
       
       // Set up input register data (normally from sensors)
       server.data.setInputRegister(0, 2500);  // Temperature × 100
       server.data.setInputRegister(1, 1013);  // Pressure in mbar
       
       // Read Input Registers request: FC=04
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x04,       // Function code
           0x00, 0x00, // Start address
           0x00, 0x02, // Quantity (2 registers)
           0x71, 0xCB  // CRC
       };
       
       auto response = server.process(request);
   }

✏️ **Writing Data**
------------------

Writing Single Coil (Function Code 05)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void writeSingleCoilExample() {
       ModbusRtuServer server;
       
       // Write Single Coil request: FC=05, Address=0, Value=ON
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x05,       // Function code
           0x00, 0x00, // Coil address
           0xFF, 0x00, // Value (0xFF00 = ON, 0x0000 = OFF)
           0x8C, 0x3A  // CRC
       };
       
       auto response = server.process(request);
       
       // Response echoes the request for successful write
       // Check that coil was actually set
       bool coilValue = server.data.readCoil(0);
       assert(coilValue == true);
   }

Writing Single Register (Function Code 06)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void writeSingleRegisterExample() {
       ModbusRtuServer server;
       
       // Write Single Register request: FC=06, Address=0, Value=0x1234
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x06,       // Function code
           0x00, 0x00, // Register address
           0x12, 0x34, // Value to write
           0x80, 0x06  // CRC
       };
       
       auto response = server.process(request);
       
       // Verify the write
       uint16_t regValue = server.data.readHoldingRegister(0);
       assert(regValue == 0x1234);
   }

Writing Multiple Coils (Function Code 15)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void writeMultipleCoilsExample() {
       ModbusRtuServer server;
       
       // Write Multiple Coils: Set 10 coils starting at address 0
       // Pattern: ON, OFF, ON, OFF, ON, OFF, ON, OFF, ON, OFF
       // Packed: 01010101 01000000 = 0x55, 0x02
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x0F,       // Function code (15)
           0x00, 0x00, // Start address
           0x00, 0x0A, // Quantity (10 coils)
           0x02,       // Byte count (2 bytes)
           0x55,       // First 8 coils: 01010101
           0x02,       // Last 2 coils: 00000010 (only bits 0-1 used)
           0x26, 0x99  // CRC
       };
       
       auto response = server.process(request);
       
       // Verify some coils were set correctly
       assert(server.data.readCoil(0) == true);
       assert(server.data.readCoil(1) == false);
       assert(server.data.readCoil(8) == true);
       assert(server.data.readCoil(9) == false);
   }

Writing Multiple Registers (Function Code 16)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void writeMultipleRegistersExample() {
       ModbusRtuServer server;
       
       // Write Multiple Registers: Set 3 registers starting at address 0
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x10,       // Function code (16)
           0x00, 0x00, // Start address
           0x00, 0x03, // Quantity (3 registers)
           0x06,       // Byte count (6 bytes = 3 registers × 2)
           0x12, 0x34, // Register 0 = 0x1234
           0x56, 0x78, // Register 1 = 0x5678
           0x9A, 0xBC, // Register 2 = 0x9ABC
           0x57, 0xC1  // CRC
       };
       
       auto response = server.process(request);
       
       // Verify the writes
       assert(server.data.readHoldingRegister(0) == 0x1234);
       assert(server.data.readHoldingRegister(1) == 0x5678);
       assert(server.data.readHoldingRegister(2) == 0x9ABC);
   }

🔧 **Diagnostics**
-----------------

Diagnostics Function (Function Code 08)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void diagnosticsExample() {
       ModbusRtuServer server;
       
       // Return Query Data (Sub-function 00): Echo back test data
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x08,       // Function code (Diagnostics)
           0x00, 0x00, // Sub-function (Return Query Data)
           0x12, 0x34, // Test data
           0x81, 0x80  // CRC
       };
       
       auto response = server.process(request);
       
       // Response should echo back the same data
       // Format: [01][08][00][00][12][34][CRC][CRC]
   }

🚨 **Error Handling**
--------------------

Exception Response Example
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void exceptionHandlingExample() {
       ModbusRtuServer server;
       
       // Invalid function code request
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x99,       // Invalid function code
           0x00, 0x00, // Some data
           0x00, 0x01,
           0x84, 0x0A  // CRC (calculated for this frame)
       };
       
       auto response = server.process(request);
       
       // Response will be an exception:
       // [01][99|0x80][01][CRC][CRC]
       // Function code OR'ed with 0x80, exception code 0x01 (Illegal Function)
       
       if (!response.empty() && response.size() >= 3) {
           bool isException = (response[1] & 0x80) != 0;
           assert(isException);
           
           ModbusExceptionCode exceptionCode = 
               static_cast<ModbusExceptionCode>(response[2]);
           assert(exceptionCode == ModbusExceptionCode::ILLEGAL_FUNCTION);
       }
   }

Address Out of Range Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void addressRangeExample() {
       ModbusRtuServer server;
       
       // Try to read beyond valid address range
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x03,       // Function code (Read Holding Registers)
           0x00, 0x7D, // Start address (125 - at the limit)
           0x00, 0x02, // Quantity (2 registers - goes beyond limit)
           0x55, 0xC6  // CRC
       };
       
       auto response = server.process(request);
       
       // Should return exception 0x02 (Illegal Data Address)
       if (!response.empty() && response.size() >= 3) {
           ModbusExceptionCode exceptionCode = 
               static_cast<ModbusExceptionCode>(response[2]);
           assert(exceptionCode == ModbusExceptionCode::ILLEGAL_DATA_ADDRESS);
       }
   }

🔄 **Real-World Integration**
----------------------------

Serial Port Integration Template
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   #include <vector>
   
   class SerialModbusServer {
   private:
       ModbusRtuServer modbusServer;
       // Your serial port implementation here
       
   public:
       void processSerialData() {
           // Pseudocode for serial integration
           /*
           while (serialPort.isOpen()) {
               // Read from serial port with appropriate timing
               auto frameData = serialPort.readFrame();
               
               if (!frameData.empty()) {
                   // Process with Modbus server
                   auto response = modbusServer.process(frameData);
                   
                   if (!response.empty()) {
                       // Send response back
                       serialPort.write(response);
                   }
               }
           }
           */
       }
       
       // Accessors for setting data from your application
       ModbusDataModel& getDataModel() {
           return modbusServer.data;
       }
   };

Custom Data Model Limits
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include "libmodbus.hpp"
   
   void customLimitsExample() {
       // Create data model with custom limits for specialized application
       ModbusDataModel customData(
           100,   // 100 discrete inputs
           50,    // 50 coils  
           200,   // 200 input registers
           150    // 150 holding registers
       );
       
       // Use with server
       ModbusRtuServer server;
       // Note: You would need to modify the server to accept custom data model
       // This requires extending the current implementation
   }

💡 **Best Practices**
--------------------

1. **Frame Validation**: Always validate CRC before processing frames
2. **Address Bounds**: Use the built-in address validation
3. **Exception Handling**: Properly handle and respond to exception conditions
4. **Data Consistency**: Ensure thread safety when accessing data from multiple contexts
5. **Timing**: Implement proper inter-frame timing for RTU protocol (3.5 character times)

🔍 **Debugging Tips**
--------------------

.. code-block:: cpp

   #include "libmodbus.hpp"
   #include <iostream>
   #include <iomanip>
   
   void debugFrames(const std::vector<uint8_t>& frame, const std::string& label) {
       std::cout << label << ": ";
       for (uint8_t byte : frame) {
           std::cout << std::hex << std::setw(2) << std::setfill('0') 
                     << static_cast<int>(byte) << " ";
       }
       std::cout << std::dec << std::endl;
   }
   
   void debugExample() {
       ModbusRtuServer server;
       
       std::vector<uint8_t> request = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
       debugFrames(request, "Request");
       
       auto response = server.process(request);
       debugFrames(response, "Response");
   } 