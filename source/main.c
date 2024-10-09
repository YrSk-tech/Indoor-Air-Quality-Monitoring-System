#include "main.h"
#include <dht_task.h>
#include <print_task.h>
#include "lcd_i2c.h"

/* I2C bus frequency */
#define I2C_FREQ                (400000UL)

/******************************************************************************
* Global Variables
******************************************************************************/
cyhal_i2c_t mI2C; // This is the declaration of mI2C

cyhal_i2c_cfg_t mI2C_cfg;

// Define global variable for backlight state
uint8_t backlight_state = 1;

/* This enables RTOS aware debugging. */
volatile int uxTopUsedPriority;


int main(void)
{
    cy_rslt_t result;

//    /* This enables RTOS aware debugging in OpenOCD. */
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    /********* Variable to store the queue handle */
    QueueHandle_t print_queue;

    /* Initialize the device and board peripherals */
    result = cybsp_init() ;
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    /* If retarget-io init failed stop program execution */
    CY_ASSERT(result == CY_RSLT_SUCCESS);
//
    /*LCD I2C Configuration*/
    mI2C_cfg.is_slave = false;
    mI2C_cfg.address = 0;
    mI2C_cfg.frequencyhal_hz = I2C_FREQ;

    /* Init I2C master */
    cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);

    /* Configure I2C Master */
    cyhal_i2c_configure(&mI2C, &mI2C_cfg);


    /* Initialize the DATA pin. The pin used can be changed in main.h file */
    result = cyhal_gpio_init(DATA_PIN, CYHAL_GPIO_DIR_BIDIRECTIONAL, CYHAL_GPIO_DRIVE_PULLUP, 1);
    if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

    /* Enable global interrupts */
    __enable_irq();


//    /*Initialization LCD*/
//    char *text = "Hello LCD I2C";
//	lcd_init(&mI2C);
//	lcd_clear(&mI2C);
//	lcd_set_cursor(&mI2C, 0, 0);
//	lcd_write_string(&mI2C, text);
//
    /* Create a queue to store the sensor readings */
    print_queue = xQueueCreate(5, sizeof(struct readings));

    /* Create tasks */
    xTaskCreate(DHT_Task, "DHT Task", configMINIMAL_STACK_SIZE, (void*) print_queue, 2, NULL);
    xTaskCreate(Print_Task, "Print Task", 3*configMINIMAL_STACK_SIZE, (void*) print_queue, 1, NULL);

    /* If the task creation failed stop the program execution */
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    /* To avoid compiler warning */
    (void) result;

    /* Start the scheduler */
    vTaskStartScheduler();

    for (;;)
    {

    }
}

/* [] END OF FILE */
