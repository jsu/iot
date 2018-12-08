#ifndef __LCD_H__
#define __LCD_H__
#include <stdint.h>

#define LCD_ADDRESS 0x27

void lcd_cmd(uint8_t cmd);
void lcd_put(char data);
void lcd_str(char *str);
void lcd_clr();
void lcd_display(char **lines);
void lcd_sleep();
void mv_to(int x, int y);
void mv_to_line(int line);
void init_lcd();

#endif
