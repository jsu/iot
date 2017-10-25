#include <stdint.h>
#include <bcm2835.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define LCD_ADDRESS 0x27

void lcd_cmd(uint8_t cmd);
void lcd_put(char data);
void lcd_str(char *str);
void lcd_clr();
void lcd_display(char **lines);
void mv_to(int x, int y);
void mv_to_line(int line);
void init_lcd();
