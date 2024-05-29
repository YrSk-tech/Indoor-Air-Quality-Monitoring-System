#include "print_task.h"
#include "lcd_i2c.h"

extern cyhal_i2c_t mI2C;

void Print_Task(void* pvParameters)
{
    QueueHandle_t print_queue;
    print_queue = (QueueHandle_t) pvParameters;
    bool conn_err_displayed = false;
    struct readings DHT_reading = {0, 0};

    lcd_init(&mI2C);
    char humidity_str[20];
    char temperature_str[20];
    char co2_str[20];

    for (;;)
    {
        xQueueReceive(print_queue, &DHT_reading, portMAX_DELAY);

        if (DHT_reading.result_code == SUCCESS)
        {
            printf("\r\nHumidity  =   %.2f\r\n", DHT_reading.humidity);
            printf("\r\nTemperature  =   %.2f\r\n", DHT_reading.temperature);
            printf("\r\nCO2 = 415\r\n");

            sprintf(humidity_str, "Hum: %.2f", DHT_reading.humidity);
            sprintf(temperature_str, "Temp: %.2f", DHT_reading.temperature);
            sprintf(co2_str, "CO2: %d ppm", DHT_reading.co2);  // Update with actual CO2 value

            lcd_clear(&mI2C);
            lcd_set_cursor(&mI2C, 0, 0);
            lcd_write_string(&mI2C, temperature_str);
            lcd_set_cursor(&mI2C, 1, 0);
            lcd_write_string(&mI2C, humidity_str);
            lcd_set_cursor(&mI2C, 2, 0);
            lcd_write_string(&mI2C, co2_str);

            conn_err_displayed = false;
        }
        else if (DHT_reading.result_code == DHT_CONNECTION_ERROR)
        {
            if (!conn_err_displayed)
            {
                printf("\r\nDHT Sensor Connection Failed\r\n");
                conn_err_displayed = true;
            }
        }
    }
}
