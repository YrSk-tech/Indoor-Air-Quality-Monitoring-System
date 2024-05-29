#include <dht_task.h>

/*******************************************************************************
* Function Name: Fraction_Convert
****************************************************************************//**
*
* Converts a 8 bit binary number into a fractional decimal value
*
* \param num
* 8 bit binary value
*
* \return
* Fractional decimal value of the binary number
*
*******************************************************************************/
float Fraction_Convert(uint8_t num)
{
    float fraction = 0;
    int unit = 0;
    for( int i = 0; i<8; i++)
    {
        unit = num & 1;
        num = num>>1;
        fraction = fraction + unit * pow(2, -(1+i));
    }
    return fraction;
}

/*******************************************************************************
* Function Name: DHT_Start
****************************************************************************//**
*
* Initiates the communication with the sensor.
* Function sends 18ms low signal after 1s delay.
*
*******************************************************************************/

void DHT_Start(void)
{
    cyhal_gpio_write(DATA_PIN, 1);
    cyhal_system_delay_ms(1000);
    cyhal_gpio_write(DATA_PIN, 0);
    cyhal_system_delay_ms(18);
    cyhal_gpio_write(DATA_PIN, 1);
}

/*******************************************************************************
* Function Name: DHT_Read
****************************************************************************//**
*
* Reads the temperature and humidity values if the sensor read is successful
*
* \param humidity
* Pointer to the float variable which stores humidity value
*
* \param temperature
* Pointer to the float variable which stores temperature value
*
* \return
* Status of sensor read
*
*******************************************************************************/

uint8 DHT_Read(float* humidity, float* temperature)
{
    uint8_t delay_time = 0, ack_time = 0;
    uint8_t temp = 0, index = 0, bit_count = 7;
    uint8_t byteval[5] = {0,0,0,0,0};   /* Array to store the 5 byte values received from the sensor */


    DHT_Start();
    /****************************************************************************
    *   Response to the start condition is a 54us low signal and 80us high signal.
    ****************************************************************************/

    taskENTER_CRITICAL();
    while( cyhal_gpio_read(DATA_PIN) == 1)
    {
        cyhal_system_delay_us(1);
        ack_time++;
        if(ack_time > timeout_duration)
        {
            /* Connection timed out */
            return DHT_CONNECTION_ERROR;
        }
    }

    while( cyhal_gpio_read(DATA_PIN) == 0)
    {
    	cyhal_system_delay_us(1);
        ack_time++;
        if(ack_time > timeout_duration)
        {
            /* Connection timed out */
            return DHT_CONNECTION_ERROR;
        }
    }

    /* Value of ack_time stored so that it can be used to calculate bit value */
    /* Value of delay_time corresponds to a little more than 54 us */
    delay_time = ack_time;
    ack_time = 0;
    while( cyhal_gpio_read(DATA_PIN) == 1)
    {
        /* Spin until sensor pulls the data line high */
    	cyhal_system_delay_us(1);
        ack_time++;
        if(ack_time > timeout_duration)
        {
            /* Connection timed out */
            return DHT_CONNECTION_ERROR;
        }
    }

    for (int i = 0; i < 40; i++)
	{
        ack_time = 0;
        while( cyhal_gpio_read(DATA_PIN) == 0)
        {
            /* Spin until sensor pulls the data line high */
        	cyhal_system_delay_us(1);
            ack_time++;
            if(ack_time > timeout_duration)
            {
                /* Connection timed out */
                return DHT_CONNECTION_ERROR;
            }
        }

        ack_time = 0;

        while( cyhal_gpio_read(DATA_PIN) == 1)
        {
        	cyhal_system_delay_us(1);
            ack_time++;
            if(ack_time > timeout_duration)
            {
                /* Connection timed out */
                return DHT_CONNECTION_ERROR;
            }
        }

        /****************************************************************************
        *  The sensor MCU outputs a low signal for 54us and high signal for 24us if
        *  bit value is 0 and a low signal for 54us and high signal for 70us if bit
        *  value is 1.
        ****************************************************************************/

        /* If ack_time value is more than delay_time/2 i.e., ack_time > (approx) 27 us then bit value is 1 */
        if ((ack_time) > (delay_time/2))
            byteval[index] |= (1 << bit_count);
        if (bit_count == 0)   /* Next Byte */
   	    {
   		    bit_count = 7;    /* Reset bit_count to 7 */
   		    index++;          /* Increment index so that next byte is chosen */
   	    }
   	    else bit_count--;
    }
    taskEXIT_CRITICAL();

    /* Checksum is calculated by adding all 4 bytes */
    temp = (byteval[0] + byteval[1] + byteval[2] + byteval[3]);
    if((temp == byteval[4]) && (byteval[4] != 0))
    {
        /* Pass the temperature and humidity data only when checksum is matched and it is not equal to 0 */
        *humidity = (int)byteval[0] + Fraction_Convert(byteval[1]);
        *temperature = (int)byteval[2] + Fraction_Convert(byteval[3]);

        return SUCCESS;
    }
    else
    {
    	/* ********************************************************************
    	 * Enter critical section which is exited in the task function.
    	 * If the application is not inside critical section and if
    	 * taskEXIT_CRITICAL() is called the system goes into hard fault.
    	 * To ensure that this does not happen the function is called.
    	 * ********************************************************************/
    	taskENTER_CRITICAL();

    	/* Incorrect value */
        return DHT_INCORRECT_VALUE;
    }
}

/*******************************************************************************
* Function Name: DHT_Task
****************************************************************************//**
*
* Task function to read the sensor values and pass the values into a queue
*
* \param pvParameters
* Void pointer which points to the queue handle
*
*******************************************************************************/

void DHT_Task(void* pvParameters)
{
	/* Variable to store the queue handle */
	QueueHandle_t print_queue;
	print_queue = (QueueHandle_t) pvParameters;

	/* Variable to store temperature and humidity values */
	struct readings DHT_reading = {0, 0};

	for(;;)
	{
		DHT_reading.result_code = DHT_Read(&DHT_reading.humidity, &DHT_reading.temperature);

		if(DHT_reading.result_code == SUCCESS)
		{
			/* *************************************************************
			 * If the sensor values are valid, pass the readings to the queue.
			 * If the queue is full, enter blocked state and wait for the
			 * Print_Task to read the value from the queue.
			 * *************************************************************/
			xQueueSendToBack(print_queue, &DHT_reading, portMAX_DELAY);
		}
		else
		{
			/* Exit critical section */
			taskEXIT_CRITICAL();

			/* *************************************************************
			 * If the sensor values are not valid, pass the error code along
			 * with the previous value to the queue. Print_Task reads the
			 * error value and decides the course of action.
			 * *************************************************************/
			xQueueSendToBack(print_queue, &DHT_reading, portMAX_DELAY);
		}

		/* ******************************************************************
		 * As the sensor sampling rate is ~2 seconds. task-delay function is
		 * called for 2 seconds.
		 * ******************************************************************/
		vTaskDelay(pdMS_TO_TICKS(2000));
	}
}


/* [] END OF FILE */
