# RTOS-Based Smart Industrial Telemetry Node

**Platform:** STM32F401RE | ARM Cortex-M4  
**Programming Language:** Embedded C  
**RTOS:** FreeRTOS

## Project Overview

This project demonstrates a real-time embedded firmware architecture using the STM32F401RE microcontroller and FreeRTOS.

The application is organized into three independent tasks for sensor data acquisition, SPI communication, and UART-based system diagnostics.

It demonstrates important embedded systems concepts such as task scheduling, inter-task communication, mutual exclusion, peripheral interfacing, and periodic task execution.

## Firmware Architecture

### 1. Sensor Acquisition Task — `vSensorTask`
- **Priority:** 3 (High)
- **Interface:** I2C
- Performs periodic I2C register reads from a sensor address configured for a BMP280.
- Packages sensor-related values into a data structure.
- Sends the data to the logger task using a FreeRTOS queue.
- Uses a 1-second periodic delay.

**Implementation note:** The current code does not perform complete BMP280 compensation calculations. Temperature and pressure values are partly simulated.

### 2. Logger Task — `vLoggerTask`
- **Priority:** 2 (Medium)
- **Interface:** SPI
- Receives data through a FreeRTOS queue.
- Uses a mutex to protect SPI access.
- Transmits a 2-byte test frame over SPI.

**Implementation note:** External flash or SD card storage is not implemented in the current version.

### 3. Diagnostics Task — `vDiagnosticsTask`
- **Priority:** 1 (Low)
- **Interface:** UART
- Periodically reports kernel uptime through UART.
- Uses FreeRTOS tick information to calculate elapsed time.
- Runs at 5-second intervals.

## Key Embedded Concepts

- **FreeRTOS Task Scheduling:** Three tasks with different priority levels.
- **Inter-Task Communication:** Queue-based transfer of sensor data.
- **Mutual Exclusion:** Mutex-based protection of the SPI resource.
- **Periodic Task Execution:** `vTaskDelay()` and `pdMS_TO_TICKS()`.
- **Peripheral Interfacing:** I2C, SPI, and UART using STM32 HAL APIs.
- **ARM Cortex-M4:** Firmware development for the STM32F401RE platform.

## Hardware and Software

| Category | Component |
|---|---|
| Microcontroller | STM32F401RE |
| Architecture | ARM Cortex-M4 |
| RTOS | FreeRTOS |
| Programming Language | Embedded C |
| Peripheral Interfaces | I2C, SPI, UART |
| Hardware Library | STM32 HAL |
| Development Tools | STM32CubeIDE / Keil (as applicable) |

## Project Structure

```text
RTOS-Smart-Telemetry-Node-STM32/
│
├── main.c
└── README.md
```

## Current Implementation Status

The current source code demonstrates the FreeRTOS task structure, queue-based communication, mutex usage, peripheral initialization, and UART uptime diagnostics.

The sensor values are partly simulated, SPI transmission uses a test frame, and external storage and live sensor-data logging are not implemented.

## Future Improvements

- Implement complete BMP280 sensor initialization and measurement compensation.
- Add actual SPI flash or SD card storage.
- Implement detailed task-state diagnostics.
- Add error handling for peripheral communication.
- Validate the complete firmware on physical hardware.

## Author

**Balija Neela Sri**

Embedded Systems | Firmware Development | Embedded C
