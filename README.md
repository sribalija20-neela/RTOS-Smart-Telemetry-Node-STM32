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

* ## 📊 Expected Serial Terminal Output (UART Console)
When running, the microcontroller transmits real-time telemetry updates and periodic system diagnostics over UART at a 115200 baud rate. The low-priority diagnostics task runs every 5 seconds to report kernel health:

```
[SYS DIAGNOSTICS] Kernel Uptime: 5000 ms
[TASK STATUS] SensorTask: RUNNING | LoggerTask: BLOCKED (Awaiting Queue)
[TELEMETRY] Temp: 24.7°C | Pressure: 1013.25 hPa -> Logged to SPI Flash

[SYS DIAGNOSTICS] Kernel Uptime: 10000 ms
[TASK STATUS] SensorTask: RUNNING | LoggerTask: BLOCKED (Awaiting Queue)
[TELEMETRY] Temp: 25.1°C | Pressure: 1013.25 hPa -> Logged to SPI Flash

[SYS DIAGNOSTICS] Kernel Uptime: 15000 ms
[TASK STATUS] SensorTask: RUNNING | LoggerTask: BLOCKED (Awaiting Queue)
[TELEMETRY] Temp: 24.9°C | Pressure: 1013.25 hPa -> Logged to SPI Flash
```


---

## 🔧 Software Tools Used
* **IDE:** STM32CubeIDE / Keil uVision
* **RTOS Kernel:** FreeRTOS
* **Hardware Abstraction:** STM32 HAL Components
