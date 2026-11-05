# RTOS-Based Smart Industrial Telemetry Node

An embedded systems project developed using **STM32F401RE**, **STM32CubeIDE**, and **FreeRTOS**. The project demonstrates multitasking, peripheral communication, inter-task data exchange, and synchronization for an industrial telemetry application.

## Project Overview

The Smart Industrial Telemetry Node demonstrates how an STM32 microcontroller can manage sensor data acquisition, data handling, and system diagnostics using a Real-Time Operating System (RTOS).

The application is organized into multiple FreeRTOS tasks, with communication between tasks handled through a queue and shared resource access protected using a mutex.

## Hardware & Software

| Component | Details |
|---|---|
| Microcontroller | STM32F401RE |
| Architecture | ARM Cortex-M4 |
| Development IDE | STM32CubeIDE |
| Configuration Tool | STM32CubeMX |
| RTOS | FreeRTOS |
| Programming Language | Embedded C |
| Firmware Library | STM32 HAL |

## Peripheral Configuration

The peripherals were configured using STM32CubeMX within STM32CubeIDE.

| Peripheral | Configuration | Purpose |
|---|---|---|
| I2C1 | 100 kHz, 7-bit addressing | Sensor communication |
| SPI1 | Full-Duplex Master, 8-bit | SPI data transmission |
| USART2 | 115200 baud, 8-N-1 | Serial diagnostics |
| GPIO | Configured through CubeMX | Peripheral support |

### Pin Configuration

- **SPI1:** PA5 (SCK), PA6 (MISO), PA7 (MOSI)
- **USART2:** PA2 (TX), PA3 (RX)
- **I2C1:** Configured through STM32CubeMX

## FreeRTOS Task Architecture

The application uses four tasks:

### 1. SensorTask
- Performs sensor data acquisition through I2C.
- Processes the received buffer.
- Sends sensor data to the LoggerTask using a FreeRTOS queue.
- Runs periodically with a 1-second delay.

### 2. LoggerTask
- Receives sensor data from the queue.
- Uses a mutex to protect shared SPI access.
- Transmits a test data frame through SPI.

### 3. DiagnosticsTask
- Provides periodic system diagnostic messages through UART.
- Reports system uptime using the FreeRTOS tick count.
- Runs at 5-second intervals.

### 4. DefaultTask
- Generated as part of the FreeRTOS project configuration.
- Runs with the default task loop.

## RTOS Concepts Implemented

- Multitasking and task scheduling
- Inter-task communication using FreeRTOS Queues
- Resource synchronization using Mutex
- Task priorities
- Periodic task execution using delays
- System diagnostics using UART

## Project Configuration Evidence

The following STM32CubeIDE screenshots demonstrate the microcontroller configuration and peripheral setup.

### Clock Configuration

![Clock Configuration](docs/images/clock_configuration.png)

### Pinout & Peripheral Configuration

![Pinout Configuration](docs/images/pinout_configuration.png)

### STM32CubeMX Configuration File

The project includes the `.ioc` configuration file, which can be opened in STM32CubeIDE to inspect the MCU pin assignments, clock settings, and peripheral configurations.

## Project Structure

```text
RTOS_Smart_Telemetry_Node/
│
├── Core/
│   ├── Inc/
│   └── Src/
│
├── Drivers/
├── Middlewares/
├── docs/
│   └── images/
│       ├── clock_configuration.png
│       └── pinout_configuration.png
│
├── RTOS_Smart_Telemetry_Node.ioc
├── .project
├── .cproject
└── README.md
```

## Build Status

**Build Result: Successful**

- Errors: 0
- Warnings: 0

The project was successfully compiled in STM32CubeIDE.

## Hardware Validation Note

The project has been successfully compiled. Physical hardware testing and runtime validation have not yet been performed because an STM32 development board and sensor hardware were not available.

The current sensor values and SPI transmission frame are demonstration implementations and are intended for further hardware integration and validation.

## Future Improvements

- Integrate a physical environmental sensor.
- Implement calibrated sensor data acquisition.
- Add external memory or data logging functionality.
- Implement UART-based monitoring.
- Perform hardware-level testing and validation.

---

**Developed as an Embedded Systems / RTOS learning project using STM32CubeIDE and FreeRTOS.**
