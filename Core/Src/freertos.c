/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "queue.h"
#include "semphr.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
typedef struct {
    float temperature;
    float pressure;
} SensorData_t;

QueueHandle_t xSensorQueue = NULL;
SemaphoreHandle_t xSPI_Mutex = NULL;
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for SensorTask */
osThreadId_t SensorTaskHandle;
const osThreadAttr_t SensorTask_attributes = {
  .name = "SensorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LoggerTask */
osThreadId_t LoggerTaskHandle;
const osThreadAttr_t LoggerTask_attributes = {
  .name = "LoggerTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for DiagnosticsTask */
osThreadId_t DiagnosticsTaskHandle;
const osThreadAttr_t DiagnosticsTask_attributes = {
  .name = "DiagnosticsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartSensorTask(void *argument);
void StartLoggerTask(void *argument);
void StartDiagnosticsTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */

  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	xSensorQueue = xQueueCreate(5, sizeof(SensorData_t));

	xSPI_Mutex = xSemaphoreCreateMutex();
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of SensorTask */
  SensorTaskHandle = osThreadNew(StartSensorTask, NULL, &SensorTask_attributes);

  /* creation of LoggerTask */
  LoggerTaskHandle = osThreadNew(StartLoggerTask, NULL, &LoggerTask_attributes);

  /* creation of DiagnosticsTask */
  DiagnosticsTaskHandle = osThreadNew(StartDiagnosticsTask, NULL, &DiagnosticsTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartSensorTask */
/**
* @brief Function implementing the SensorTask thread.
* @param argument: Not used
* @retval None
*/
void StartSensorTask(void *argument)
{
    /* USER CODE BEGIN StartSensorTask */

    SensorData_t data;
    uint8_t rawBuf[4];

    for (;;)
    {
        if (HAL_I2C_Mem_Read(&hi2c1, (0x76 << 1), 0xF7,
                             I2C_MEMADD_SIZE_8BIT, rawBuf, 4, 100) == HAL_OK)
        {
            data.temperature = 24.5f + ((float)(rawBuf[0] % 10) / 10.0f);
            data.pressure = 1013.25f;

            if (xSensorQueue != NULL)
            {
                xQueueSend(xSensorQueue, &data, portMAX_DELAY);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    /* USER CODE END StartSensorTask */
}

/* USER CODE BEGIN Header_StartLoggerTask */
/**
* @brief Function implementing the LoggerTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLoggerTask */
void StartLoggerTask(void *argument)
{
    /* USER CODE BEGIN StartLoggerTask */

    SensorData_t receivedData;
    uint8_t spiTxData[2] = {0xAA, 0xBB};

    for (;;)
    {
        if (xQueueReceive(xSensorQueue, &receivedData, portMAX_DELAY) == pdPASS)
        {
            if (xSemaphoreTake(xSPI_Mutex, portMAX_DELAY) == pdTRUE)
            {
                HAL_SPI_Transmit(&hspi1, spiTxData, 2, 100);

                xSemaphoreGive(xSPI_Mutex);
            }
        }
    }

    /* USER CODE END StartLoggerTask */
}

void StartDiagnosticsTask(void *argument)
{
    /* USER CODE BEGIN StartDiagnosticsTask */

    char msg[64];

    for (;;)
    {
        snprintf(msg, sizeof(msg),
                 "\r\n[SYS DIAGNOSTICS] Kernel Uptime: %lu ms\r\n",
                 (unsigned long)(xTaskGetTickCount() * portTICK_PERIOD_MS));

        HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), 100);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }

    /* USER CODE END StartDiagnosticsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

