#ifndef SOURCE_DHT_TASK_H_
#define SOURCE_DHT_TASK_H_

#include "main.h"

/* Sensor timeout duration can be changed here */
#define timeout_duration 100



float Fraction_Convert(uint8_t num);
void DHT_Start(void);
uint8 DHT_Read(float* humidity, float* temperature);
void DHT_Task(void* pvParameters);


#endif /* SOURCE_DHT_TASK_H_ */
