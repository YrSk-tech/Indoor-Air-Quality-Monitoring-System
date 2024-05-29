#ifndef LCD_I2C_H
#define LCD_I2C_H

#include "cyhal.h"



// Macro Definitions

#if !defined(I2C_ADDR)
#define I2C_ADDR 0x27 // I2C address of the LCD
#endif

#if !defined(RS_BIT)
#define RS_BIT 0 // Register select bit
#endif

#if !defined(EN_BIT)
#define EN_BIT 2 // Enable bit
#endif

#if !defined(BL_BIT)
#define BL_BIT 3 // Backlight bit
#endif

#if !defined(D4_BIT)
#define D4_BIT 4 // Data 4 bit
#endif

#if !defined(D5_BIT)
#define D5_BIT 5 // Data 5 bit
#endif

#if !defined(D6_BIT)
#define D6_BIT 6 // Data 6 bit
#endif

#if !defined(D7_BIT)
#define D7_BIT 7 // Data 7 bit
#endif

#if !defined(LCD_ROWS)
#define LCD_ROWS 4 // Number of rows on the LCD
#endif

#if !defined(LCD_COLS)
#define LCD_COLS 16 // Number of columns on the LCD
#endif

// Function Declarations

void lcd_write_nibble(cyhal_i2c_t* i2c, uint8_t nibble, uint8_t rs);
void lcd_send_cmd(cyhal_i2c_t* i2c, uint8_t cmd);
void lcd_send_data(cyhal_i2c_t* i2c, uint8_t data);
void lcd_init(cyhal_i2c_t* i2c);
void lcd_write_string(cyhal_i2c_t* i2c, char *str);
void lcd_set_cursor(cyhal_i2c_t* i2c, uint8_t row, uint8_t column);
void lcd_clear(cyhal_i2c_t* i2c);
void lcd_backlight(uint8_t state);

#endif // LCD_I2C_H
