#ifndef SOURCE_MAIN_H_
#define SOURCE_MAIN_H_

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

//Includes for lcd i2c

#include <stdio.h>
#include <string.h>
#include "lcd_i2c.h"

//Includes for retarget_io ( printing data to tera term )
#include "cy_retarget_io.h"
#include <math.h>
//Includes for FreeRTOS
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"



#define DATA_PIN 	P5_7

/* Error Codes */
#define SUCCESS 				0x00
#define DHT_CONNECTION_ERROR 	0x01
#define DHT_INCORRECT_VALUE		0x02

/* Structure to store temperature and humidity readings */
struct readings
{
	float humidity;
	float temperature;
	uint8 result_code;
    int co2; // Add CO2 concentration field
};

#endif /* SOURCE_MAIN_H_ */
