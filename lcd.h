#ifndef LCD_H_
#define LCD_H_

#include "myLib.h"

void lcd_portconfig(void);
void lcd_init(void);
void lcd_command(char cmd);
void lcd_char(char data);
void lcd_string(char *g);
void lcd_cursor(char row, char column);
void lcd_print(char row, char column, uint32_t value, int digits);

#endif
