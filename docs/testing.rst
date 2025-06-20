Testing
=======

The libmodbus library includes comprehensive unit tests to ensure reliability and compliance
with Modbus specifications. This section covers the testing framework, running tests, and
writing additional tests.

🧪 **Test Framework**
---------------------

The testing suite uses **Google Test** (gtest) framework, which provides:

* Rich assertion macros for detailed error reporting
* Test fixtures for setup and teardown
* Parameterized tests for testing with multiple inputs
* Death tests for exception handling validation
* Test discovery and execution

**Test Structure**

.. code-block:: text

   tests/
   └── unit/
       ├── CMakeLists.txt
       └── ModbusRtuServerTest.cpp

🚀 **Running Tests**
-------------------

**Building and Running All Tests**

.. code-block:: bash

   # From project root
   mkdir build && cd build
   cmake ..
   make
   
   # Run all tests
   make test
   
   # Or run tests with detailed output
   ctest --verbose

**Running Specific Test Executable**

.. code-block:: bash

   # Run the RTU server tests directly
   ./tests/unit/ModbusRtuServerTest

**Test Output Example**

.. code-block:: text

   [==========] Running 25 tests from 1 test suite.
   [----------] Global test environment set-up.
   [----------] 25 tests from ModbusRtuServerTest
   [ RUN      ] ModbusRtuServerTest.ReadCoils_ValidRequest
   [       OK ] ModbusRtuServerTest.ReadCoils_ValidRequest (0 ms)
   [ RUN      ] ModbusRtuServerTest.ReadCoils_InvalidCRC
   [       OK ] ModbusRtuServerTest.ReadCoils_InvalidCRC (0 ms)
   ...
   [----------] 25 tests from ModbusRtuServerTest (2 ms total)
   [==========] 25 tests from 1 test suite ran. (2 ms total)
   [  PASSED  ] 25 tests.

📋 **Test Coverage**
-------------------

Current Test Coverage
~~~~~~~~~~~~~~~~~~~~~

The test suite covers all major functionality:

**✅ Function Code Testing**

* **FC 01**: Read Coils - All scenarios covered
* **FC 02**: Read Discrete Inputs - All scenarios covered  
* **FC 03**: Read Holding Registers - All scenarios covered
* **FC 04**: Read Input Registers - All scenarios covered
* **FC 05**: Write Single Coil - All scenarios covered
* **FC 06**: Write Single Register - All scenarios covered
* **FC 08**: Diagnostics - Core sub-functions covered
* **FC 15**: Write Multiple Coils - All scenarios covered
* **FC 16**: Write Multiple Registers - All scenarios covered

**✅ Error Conditions**

* Invalid CRC handling
* Wrong slave address filtering  
* Unsupported function codes
* Address out of range errors
* Invalid quantity values
* Malformed requests

**✅ Frame Processing**

* RTU frame serialization/deserialization
* Exception response generation
* CRC calculation accuracy
* Frame validation logic

🔍 **Test Examples**
-------------------

Basic Function Code Test
~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, ReadCoils_ValidRequest) {
       // Set up test data
       server.data.writeCoil(0, true);
       server.data.writeCoil(1, false);
       server.data.writeCoil(2, true);
       
       // Create valid request
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x01,       // Function code (Read Coils)
           0x00, 0x00, // Start address
           0x00, 0x03, // Quantity (3 coils)
           0x0C, 0x0D  // Valid CRC
       };
       
       // Process request
       auto response = server.process(request);
       
       // Validate response
       ASSERT_FALSE(response.empty());
       EXPECT_EQ(response[0], 0x01);     // Slave address
       EXPECT_EQ(response[1], 0x01);     // Function code
       EXPECT_EQ(response[2], 0x01);     // Byte count
       EXPECT_EQ(response[3], 0x05);     // Data: 101b = 0x05
   }

Exception Handling Test
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, InvalidFunctionCode_ReturnsException) {
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x99,       // Invalid function code
           0x00, 0x00, // Some data
           0x00, 0x01,
           0x84, 0x4A  // Valid CRC for this frame
       };
       
       auto response = server.process(request);
       
       ASSERT_FALSE(response.empty());
       EXPECT_EQ(response[0], 0x01);     // Slave address
       EXPECT_EQ(response[1], 0x99 | 0x80); // Function code with exception bit
       EXPECT_EQ(response[2], 0x01);     // Exception code (Illegal Function)
   }

CRC Validation Test
~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, InvalidCRC_NoResponse) {
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x01,       // Function code
           0x00, 0x00, // Start address
           0x00, 0x01, // Quantity
           0xFF, 0xFF  // Invalid CRC
       };
       
       auto response = server.process(request);
       
       // Should return empty response for invalid CRC
       EXPECT_TRUE(response.empty());
   }

Address Range Test
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, AddressOutOfRange_ReturnsException) {
       std::vector<uint8_t> request = {
           0x01,       // Slave address
           0x03,       // Read Holding Registers
           0x00, 0x7D, // Start address (125 - at limit)
           0x00, 0x02, // Quantity (would exceed limit)
           0x55, 0xC6  // Valid CRC
       };
       
       auto response = server.process(request);
       
       ASSERT_FALSE(response.empty());
       EXPECT_EQ(response[1], 0x83);     // Function + exception bit
       EXPECT_EQ(response[2], 0x02);     // Illegal Data Address
   }

🔧 **Writing Additional Tests**
------------------------------

Test Fixture Setup
~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   #include <gtest/gtest.h>
   #include "ModbusBaseServer.hpp"
   
   class ModbusRtuServerTest : public ::testing::Test {
   protected:
       void SetUp() override {
           // Initialize test data if needed
           server.data.writeCoil(0, false);
           server.data.writeHoldingRegister(0, 0);
       }
       
       void TearDown() override {
           // Cleanup after each test
       }
       
       ModbusRtuServer server;
   };

Parameterized Tests
~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // Test multiple function codes with similar logic
   class FunctionCodeTest : public ::testing::TestWithParam<uint8_t> {
   protected:
       ModbusRtuServer server;
   };
   
   TEST_P(FunctionCodeTest, ValidFunctionCodes) {
       uint8_t functionCode = GetParam();
       
       std::vector<uint8_t> request = {
           0x01, functionCode, 0x00, 0x00, 0x00, 0x01
       };
       
       // Add valid CRC
       uint16_t crc = Checksum::calculateCRC16(request.data(), 4);
       request.push_back(crc & 0xFF);
       request.push_back((crc >> 8) & 0xFF);
       
       auto response = server.process(request);
       
       // Should not be empty for valid function codes
       EXPECT_FALSE(response.empty());
   }
   
   INSTANTIATE_TEST_SUITE_P(
       ValidFunctionCodes,
       FunctionCodeTest,
       ::testing::Values(0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x08, 0x0F, 0x10)
   );

Helper Functions
~~~~~~~~~~~~~~~~

.. code-block:: cpp

   class TestHelper {
   public:
       static std::vector<uint8_t> createValidRequest(
           uint8_t slaveAddr,
           uint8_t functionCode,
           uint16_t startAddr,
           uint16_t quantity
       ) {
           std::vector<uint8_t> request = {
               slaveAddr,
               functionCode,
               static_cast<uint8_t>(startAddr >> 8),
               static_cast<uint8_t>(startAddr & 0xFF),
               static_cast<uint8_t>(quantity >> 8),
               static_cast<uint8_t>(quantity & 0xFF)
           };
           
           // Calculate and append CRC
           uint16_t crc = Checksum::calculateCRC16(request.data(), request.size());
           request.push_back(crc & 0xFF);
           request.push_back((crc >> 8) & 0xFF);
           
           return request;
       }
       
       static void validateExceptionResponse(
           const std::vector<uint8_t>& response,
           uint8_t expectedSlaveAddr,
           uint8_t expectedFunctionCode,
           ModbusExceptionCode expectedExceptionCode
       ) {
           ASSERT_GE(response.size(), 3);
           EXPECT_EQ(response[0], expectedSlaveAddr);
           EXPECT_EQ(response[1], expectedFunctionCode | 0x80);
           EXPECT_EQ(response[2], static_cast<uint8_t>(expectedExceptionCode));
       }
   };

🧩 **Test Data Generators**
---------------------------

CRC Test Data Generator
~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   class CRCTestData {
   public:
       struct TestFrame {
           std::vector<uint8_t> data;
           uint16_t expectedCRC;
           std::string description;
       };
       
       static std::vector<TestFrame> getKnownGoodFrames() {
           return {
               {
                   {0x01, 0x03, 0x00, 0x00, 0x00, 0x01},
                   0x840A,
                   "Read 1 holding register from address 0"
               },
               {
                   {0x01, 0x01, 0x00, 0x00, 0x00, 0x08},
                   0x3DCC,
                   "Read 8 coils from address 0"
               }
           };
       }
   };

Boundary Value Test Generator
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   class BoundaryTestData {
   public:
       static std::vector<uint16_t> getCoilQuantityBoundaries() {
           return {0, 1, 2000, 2001}; // Invalid, min valid, max valid, invalid
       }
       
       static std::vector<uint16_t> getRegisterQuantityBoundaries() {
           return {0, 1, 125, 126}; // Invalid, min valid, max valid, invalid
       }
   };

📊 **Performance Testing**
-------------------------

Throughput Test
~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, Performance_RequestProcessingSpeed) {
       auto request = TestHelper::createValidRequest(0x01, 0x03, 0x00, 0x01);
       
       auto start = std::chrono::high_resolution_clock::now();
       
       constexpr int NUM_ITERATIONS = 10000;
       for (int i = 0; i < NUM_ITERATIONS; ++i) {
           auto response = server.process(request);
           ASSERT_FALSE(response.empty());
       }
       
       auto end = std::chrono::high_resolution_clock::now();
       auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
       
       double avgTimePerRequest = static_cast<double>(duration.count()) / NUM_ITERATIONS;
       
       // Should process requests in reasonable time (adjust threshold as needed)
       EXPECT_LT(avgTimePerRequest, 100.0); // Less than 100 microseconds per request
       
       std::cout << "Average processing time: " << avgTimePerRequest 
                 << " microseconds per request" << std::endl;
   }

Memory Usage Test
~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   TEST_F(ModbusRtuServerTest, Performance_MemoryUsage) {
       // Test that repeated operations don't leak memory
       size_t initialSize = server.data.getCoils().capacity();
       
       for (int i = 0; i < 1000; ++i) {
           auto request = TestHelper::createValidRequest(0x01, 0x01, 0x00, 0x10);
           auto response = server.process(request);
       }
       
       size_t finalSize = server.data.getCoils().capacity();
       
       // Memory usage should remain stable
       EXPECT_EQ(initialSize, finalSize);
   }

🛠️ **Debugging Failed Tests**
-----------------------------

Verbose Test Output
~~~~~~~~~~~~~~~~~~~

.. code-block:: bash

   # Run with detailed output
   cd build
   ctest --verbose --output-on-failure
   
   # Run specific test with gtest flags
   ./tests/unit/ModbusRtuServerTest --gtest_filter="*ReadCoils*" --gtest_output=verbose

Test Debugging Macros
~~~~~~~~~~~~~~~~~~~~~~

.. code-block:: cpp

   // Add this to your test for detailed frame analysis
   void printFrame(const std::vector<uint8_t>& frame, const std::string& label) {
       std::cout << label << ": ";
       for (size_t i = 0; i < frame.size(); ++i) {
           printf("%02X ", frame[i]);
           if (i == 1) std::cout << "| ";  // Separate address and function
           if (i == frame.size() - 3) std::cout << "| "; // Separate data and CRC
       }
       std::cout << std::endl;
   }
   
   TEST_F(ModbusRtuServerTest, Debug_FrameAnalysis) {
       auto request = TestHelper::createValidRequest(0x01, 0x03, 0x00, 0x01);
       printFrame(request, "Request");
       
       auto response = server.process(request);
       printFrame(response, "Response");
   }

🔄 **Continuous Integration**
----------------------------

The tests are automatically run in CI/CD pipeline on every commit and pull request.
This ensures:

* **Regression Prevention**: New changes don't break existing functionality
* **Platform Compatibility**: Tests run on multiple platforms/compilers
* **Code Quality**: Maintains high standards for code contributions

**CI Test Execution**

.. code-block:: yaml

   # Example CI step (simplified)
   - name: Run Tests
     run: |
       mkdir build && cd build
       cmake ..
       make
       ctest --output-on-failure

🎯 **Best Practices for Testing**
--------------------------------

1. **Test Driven Development**: Write tests before implementing features
2. **Comprehensive Coverage**: Test both success and failure scenarios
3. **Isolated Tests**: Each test should be independent and repeatable
4. **Clear Assertions**: Use descriptive assertion messages
5. **Performance Awareness**: Include performance regression tests
6. **Documentation**: Comment complex test scenarios
7. **Boundary Testing**: Test edge cases and boundary conditions

📝 **Adding New Tests**
----------------------

When adding new functionality:

1. **Create Test Cases**: Cover normal operation, edge cases, and error conditions
2. **Update CMakeLists.txt**: Add new test files to the build system
3. **Verify Coverage**: Ensure new code paths are tested
4. **Run Full Suite**: Verify existing tests still pass

Example of adding a new test file:

.. code-block:: cmake

   # In tests/unit/CMakeLists.txt
   add_executable(NewFeatureTest NewFeatureTest.cpp)
   target_link_libraries(NewFeatureTest libmodbus gtest gtest_main)
   add_test(NAME NewFeatureTest COMMAND NewFeatureTest) 