#include "lcd_i2c.h"

// Correct initialization of the static variable
static uint8_t backlight_state = 1; // Turn on backlight

void lcd_write_nibble(cyhal_i2c_t* i2c, uint8_t nibble, uint8_t rs) {
    uint8_t data = (nibble << D4_BIT) | (rs << RS_BIT) | (backlight_state << BL_BIT) | (1 << EN_BIT);
    cyhal_i2c_master_write(i2c, I2C_ADDR, &data, 1, 100, true);
    cyhal_system_delay_ms(1);
    data &= ~(1 << EN_BIT);
    cyhal_i2c_master_write(i2c, I2C_ADDR, &data, 1, 100, true);
}

void lcd_send_cmd(cyhal_i2c_t* i2c, uint8_t cmd) {
    uint8_t upper_nibble = cmd >> 4;
    uint8_t lower_nibble = cmd & 0x0F;
    lcd_write_nibble(i2c, upper_nibble, 0);
    lcd_write_nibble(i2c, lower_nibble, 0);
    if (cmd == 0x01 || cmd == 0x02) {
        cyhal_system_delay_ms(2); // longer delay for clear and home commands
    }
}

void lcd_send_data(cyhal_i2c_t* i2c, uint8_t data) {
    uint8_t upper_nibble = data >> 4;
    uint8_t lower_nibble = data & 0x0F;
    lcd_write_nibble(i2c, upper_nibble, 1);
    lcd_write_nibble(i2c, lower_nibble, 1);
}

void lcd_init(cyhal_i2c_t* i2c) {
    cyhal_system_delay_ms(50);
    lcd_write_nibble(i2c, 0x03, 0);
    cyhal_system_delay_ms(5);
    lcd_write_nibble(i2c, 0x03, 0);
    cyhal_system_delay_ms(1);
    lcd_write_nibble(i2c, 0x03, 0);
    cyhal_system_delay_ms(1);
    lcd_write_nibble(i2c, 0x02, 0);
    lcd_send_cmd(i2c, 0x28); // Function set: 4-bit, 2 line, 5x7 dots
    lcd_send_cmd(i2c, 0x0C); // Display on, cursor off, blink off
    lcd_send_cmd(i2c, 0x06); // Entry mode set: increment automatically, no display shift
    lcd_send_cmd(i2c, 0x01); // Clear display
    cyhal_system_delay_ms(2);
}

void lcd_write_string(cyhal_i2c_t* i2c, char *str) {
    while (*str) {
        lcd_send_data(i2c,*str++);
    }
}

void lcd_set_cursor(cyhal_i2c_t* i2c, uint8_t row, uint8_t column) {
    uint8_t address = (row == 0) ? 0x00 : 0x40;
    address += column;
    lcd_send_cmd(i2c, 0x80 | address);
}

void lcd_clear(cyhal_i2c_t* i2c) {
    lcd_send_cmd(i2c,0x01);
    cyhal_system_delay_ms(2); // Wait for the clear command to execute
}

void lcd_backlight(uint8_t state) {
    backlight_state = state ? 1 : 0;
}
