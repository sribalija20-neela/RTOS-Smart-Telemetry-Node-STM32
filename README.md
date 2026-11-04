# RTOS-Based Smart Industrial Telemetry Node (STM32)

## 📌 Project Overview
This project is an advanced, independent technical implementation of a deterministic, multi-tasking telemetry firmware application designed for the **STM32F401RE** microcontroller (ARM Cortex-M4 core). Utilizing **FreeRTOS**, the system manages parallel tasks for sensor data acquisition, secure local logging, and system diagnostics with strict thread safety.

---

## 🛠️ Firmware Architecture & FreeRTOS Implementation
Instead of using a traditional bare-metal super-loop layout, this application leverages a real-time kernel scheduler to manage resource-constrained execution across three parallel threads:

1. **Task 1: Sensor Interfacing (`vSensorTask`)**
   - **Priority:** High (3)
   - **Protocol:** I2C (Simulated BMP280 register reads)
   - **Execution:** Samples environmental metrics every 1,000ms using `HAL_I2C_Mem_Read` and pushes packaged data into an inter-task queue.

2. **Task 2: Data Logging (`vLoggerTask`)**
   - **Priority:** Medium (2)
   - **Protocol:** SPI (Flash memory/SD Card interfacing)
   - **Execution:** Blocks efficiently until data lands in the queue. It acquires a shared-resource **Mutex** to prevent data corruption before initiating SPI transmissions via `HAL_SPI_Transmit`.

3. **Task 3: Diagnostic CLI (`vDiagnosticsTask`)**
   - **Priority:** Low (1)
   - **Protocol:** UART (Serial communication)
   - **Execution:** Wakes up every 5,000ms to output real-time system performance metrics, task execution flags, and kernel uptime ticks via `HAL_UART_Transmit`.

---

## 🎛️ Key Engineering Concepts Demonstrated
* **Thread-Safe Data Communication:** Implemented **FreeRTOS Queues** to pass complex sensor structures safely between processing and storage tasks.
* **Mutual Exclusion (Mutex):** Utilized a `SemaphoreHandle_t` mutex to protect the shared SPI bus resource, eliminating potential race conditions.
* **Non-Blocking Time Management:** Replaced processor-wasting `HAL_Delay()` with kernel-managed `vTaskDelay(pdMS_TO_TICKS())` to maximize CPU efficiency.

---

## 🔧 Software Tools Used
* **IDE:** STM32CubeIDE / Keil uVision
* **RTOS Kernel:** FreeRTOS
* **Hardware Abstraction:** STM32 HAL Components
