#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* --- Peripherals Handles --- */
I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
UART_HandleTypeDef huart2;

/* --- FreeRTOS Handles --- */
QueueHandle_t xSensorQueue = NULL;
SemaphoreHandle_t xSPI_Mutex = NULL;

/* --- Task Prototypes --- */
void vSensorTask(void *pvParameters);
void vLoggerTask(void *pvParameters);
void vDiagnosticsTask(void *pvParameters);

/* --- Data Structure for Queue --- */
typedef struct {
    float temperature;
    float pressure;
} SensorData_t;

int main(void) {
    /* Initialize System Clocks and Hal Peripherals */
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_I2C1_Init();
    MX_SPI1_Init();
    MX_USART2_UART_Init();

    /* 1. Create Inter-Task Queue (Holds up to 5 sensor readings) */
    xSensorQueue = xQueueCreate(5, sizeof(SensorData_t));

    /* 2. Create Mutex to protect the shared SPI bus resource */
    xSPI_Mutex = xSemaphoreCreateMutex();

    if (xSensorQueue != NULL && xSPI_Mutex != NULL) {
        /* 3. Create Tasks with Deterministic Priorities */
        xTaskCreate(vSensorTask, "SensorTask", 128, NULL, 3, NULL);  // Highest priority
        xTaskCreate(vLoggerTask, "LoggerTask", 128, NULL, 2, NULL);  // Medium priority
        xTaskCreate(vDiagnosticsTask, "DiagTask", 128, NULL, 1, NULL); // Lowest priority

        /* 4. Start the Real-Time Scheduler */
        vTaskStartScheduler();
    }

    while (1) {
        // Should never reach here if scheduler starts
    }
}

/* --- Task 1: Read Sensor Data via I2C --- */
void vSensorTask(void *pvParameters) {
    SensorData_t data;
    uint8_t rawBuf[4];

    while (1) {
        // Simulate reading from an I2C Sensor (e.g., BMP280 at address 0x76)
        HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0xF7, I2C_MEMADD_SIZE_8BIT, rawBuf, 4, 100);
        
        // Mocking calculated telemetry metrics
        data.temperature = 24.5f + ((float)(rawBuf[0] % 10) / 10.0f);
        data.pressure = 1013.25f;

        // Post data to Logger Task via Queue
        xQueueSend(xSensorQueue, &data, portMAX_DELAY);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Sample every 1 second
    }
}

/* --- Task 2: Log Data via Protected SPI Bus --- */
void vLoggerTask(void *pvParameters) {
    SensorData_t receivedData;
    uint8_t spiTxData[8] = {0xAA, 0xBB}; // Frame headers

    while (1) {
        // Wait until sensor data arrives in the queue
        if (xQueueReceive(xSensorQueue, &receivedData, portMAX_DELAY) == pdPASS) {
            
            // Take Mutex before using shared SPI line
            if (xSemaphoreTake(xSPI_Mutex, portMAX_DELAY) == pdTRUE) {
                
                // Transmit data safely over SPI
                HAL_SPI_Transmit(&hspi1, spiTxData, 2, 100);
                
                // Release Mutex immediately after transmission
                xSemaphoreGive(xSPI_Mutex);
            }
        }
    }
}

/* --- Task 3: Command Line Interface & Diagnostics via UART --- */
void vDiagnosticsTask(void *pvParameters) {
    char msg[64];

    while (1) {
        // Periodically output system performance metrics over UART
        snprintf(msg, sizeof(msg), "\r\n[SYS DIAGNOSTICS] Kernel Uptime: %lu ms\r\n", xTaskGetTickCount() * portTICK_PERIOD_MS);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);

        vTaskDelay(pdMS_TO_TICKS(5000)); // Print status every 5 seconds
    }
}
