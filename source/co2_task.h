#ifndef CO2_TASK_H
#define CO2_TASK_H

#include "cyhal.h"
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Macros for CO2 sensor UART communication
#define TX_CO2 P12_1
#define RX_CO2 P12_0
#define DATA_BITS_8 8
#define STOP_BITS_1 1
#define BAUD_RATE 9600
#define UART_DELAY 10u
#define RX_BUF_SIZE 9
#define TX_BUF_SIZE 9

#define READ_CO2_CONCENTRATION 0x86
#define SPAN_POINT_CALIBRATION 0x88
#define ZERO_POINT_CALIBRATION 0x87

void CO2_Task(void* pvParameters);

#endif /* CO2_TASK_H */
