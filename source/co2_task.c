#include "co2_task.h"

void CO2_Task(void* pvParameters)
{
    cyhal_uart_t mh_co2;
    uint32_t actualbaud;
    uint8_t tx_buf[TX_BUF_SIZE] = {0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
    uint8_t rx_buf[RX_BUF_SIZE];
    size_t tx_length = TX_BUF_SIZE;
    size_t rx_length = RX_BUF_SIZE;

    const cyhal_uart_cfg_t cnf_co2 = {
        .data_bits = DATA_BITS_8,
        .stop_bits = STOP_BITS_1,
        .parity = CYHAL_UART_PARITY_NONE,
        .rx_buffer = rx_buf,
        .rx_buffer_size = RX_BUF_SIZE
    };

    cyhal_uart_init(&mh_co2, TX_CO2, RX_CO2, NC, NC, NULL, &cnf_co2);
    cyhal_uart_set_baud(&mh_co2, BAUD_RATE, &actualbaud);

    tx_buf[2] = ZERO_POINT_CALIBRATION;
    cyhal_uart_write(&mh_co2, (void*)tx_buf, &tx_length);
    cyhal_system_delay_ms(UART_DELAY);

    tx_buf[2] = READ_CO2_CONCENTRATION;
    cyhal_uart_write(&mh_co2, (void*)tx_buf, &tx_length);
    cyhal_system_delay_ms(UART_DELAY);

    for (;;)
    {
        cyhal_uart_read(&mh_co2, rx_buf, &rx_length);
        cyhal_system_delay_ms(UART_DELAY);

        int ihigh_co2 = rx_buf[3];
        int ilow_co2 = rx_buf[4];
        int concentration = ihigh_co2 * 256 + ilow_co2;

        printf("\r\nCO2 Concentration = %d ppm\r\n", concentration);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
