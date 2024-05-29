#include "main.h"
#include "dht_task.h"
#include "print_task.h"
#include "lcd_i2c.h"
#include "co2_task.h"

#define I2C_FREQ (400000UL)

cyhal_i2c_t mI2C;
cyhal_i2c_cfg_t mI2C_cfg;
uint8_t backlight_state = 1;
volatile int uxTopUsedPriority;

int main(void)
{
    cy_rslt_t result;
    uxTopUsedPriority = configMAX_PRIORITIES - 1;

    QueueHandle_t print_queue;
    QueueHandle_t co2_queue;

    result = cybsp_init();
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    mI2C_cfg.is_slave = false;
    mI2C_cfg.address = 0;
    mI2C_cfg.frequencyhal_hz = I2C_FREQ;
    cyhal_i2c_init(&mI2C, CYBSP_I2C_SDA, CYBSP_I2C_SCL, NULL);
    cyhal_i2c_configure(&mI2C, &mI2C_cfg);

    result = cyhal_gpio_init(DATA_PIN, CYHAL_GPIO_DIR_BIDIRECTIONAL, CYHAL_GPIO_DRIVE_PULLUP, 1);
    CY_ASSERT(result == CY_RSLT_SUCCESS);

    __enable_irq();

    print_queue = xQueueCreate(5, sizeof(struct readings));
    co2_queue = xQueueCreate(5, sizeof(int));

    xTaskCreate(DHT_Task, "DHT Task", configMINIMAL_STACK_SIZE, (void*) print_queue, 2, NULL);
    xTaskCreate(Print_Task, "Print Task", 3*configMINIMAL_STACK_SIZE, (void*) print_queue, 1, NULL);
    xTaskCreate(CO2_Task, "CO2 Task", configMINIMAL_STACK_SIZE, (void*) co2_queue, 2, NULL);

    CY_ASSERT(result == CY_RSLT_SUCCESS);
    vTaskStartScheduler();

    for (;;) {}
}
