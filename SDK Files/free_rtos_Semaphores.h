
#ifndef free_rtos_Semaphores_HEADER
#define free_rtos_Semaphores_HEADER

//----------------- FreeRTOS -------------------
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

#define TASK_PRIO (configMAX_PRIORITIES - 1)
#define CONSUMER_LINE_SIZE 2
SemaphoreHandle_t xSemaphore_I2C_Slave;
SemaphoreHandle_t xSemaphore_I2C_Master;
SemaphoreHandle_t xSemaphore_Canbus;


#endif // !free_rtos_Semaphores_HEADER



