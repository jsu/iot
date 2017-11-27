#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "bcm2835.h"
#include "lcd.h"

void lcd_cmd(uint8_t cmd)
{
    char buffer[1];
    struct timespec sleeper;
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 38000L;

    buffer[0] = cmd & 0xf0;
    buffer[0] |= 0x04;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
    nanosleep(&sleeper, NULL);
    buffer[0] &= 0xfb;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);

    buffer[0] = (cmd & 0x0f) << 4;
    buffer[0] |= 0x04;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
    nanosleep(&sleeper, NULL);
    buffer[0] &= 0xfb;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
}

void lcd_put(char data)
{
    char buffer[1];
    struct timespec sleeper;
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 38000L;

    buffer[0] = data & 0xf0;
    buffer[0] |= 0x05;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
    nanosleep(&sleeper, NULL);
    buffer[0] &= 0xfb;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);

    buffer[0] = (data & 0x0f) << 4;
    buffer[0] |= 0x05;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
    nanosleep(&sleeper, NULL);
    buffer[0] &= 0xfb;
    buffer[0] |= 0x08;
    bcm2835_i2c_write(buffer, 1);
}

void lcd_str(char *str)
{
    char c;
    while((c = *str++) != '\0')
        lcd_put(c);
}

void mv_to(int x, int y)
{
    lcd_cmd(0x80 + 0x40 * y + x);
}

void mv_to_line(int line)
{
    switch(line)
    {
        case 0:
            mv_to(0, 0);
            break;
        case 1:
            mv_to(0, 1);
            break;
        case 2:
            mv_to(20, 0);
            break;
        case 3:
            mv_to(20, 1);
            break;
        default:
            mv_to(0, 0);
    }
}

void lcd_display(char **lines)
{
    mv_to_line(0);
    lcd_str("                    ");
    mv_to_line(0);
    lcd_str(*(lines + 0));
    mv_to_line(1);
    lcd_str("                    ");
    mv_to_line(1);
    lcd_str(*(lines + 1));
    mv_to_line(2);
    lcd_str("                    ");
    mv_to_line(2);
    lcd_str(*(lines + 2));
    mv_to_line(3);
    lcd_str("                    ");
    mv_to_line(3);
    lcd_str(*(lines + 3));
}

void lcd_clr()
{
    /* 1.52 ms is way slower then printing blanks */
    /*
    struct timespec sleeper;
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 1520000L;
    lcd_cmd(0x01);
    nanosleep(&sleeper, NULL);
    */
    mv_to_line(0);
    lcd_str("                    ");
    mv_to_line(1);
    lcd_str("                    ");
    mv_to_line(2);
    lcd_str("                    ");
    mv_to_line(3);
    lcd_str("                    ");
}

void lcd_sleep()
{
    struct timespec sleeper;
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 38000L;
    nanosleep(&sleeper, NULL);
    lcd_cmd(0x08);
}

void init_lcd()
{
    struct timespec sleeper;
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 38000L;
    lcd_cmd(0x33); /* Init 8-line mode, required */
    nanosleep(&sleeper, NULL);
    lcd_cmd(0x32); /* Init 4-line mode now */
    nanosleep(&sleeper, NULL);
    lcd_cmd(0x28); /* 2 lines w/ 5*7 dots */
    nanosleep(&sleeper, NULL);
    lcd_cmd(0x0c); /* Enable display without cursor */
    nanosleep(&sleeper, NULL);
    lcd_cmd(0x01); /* real clear screen */
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 1520000L;
    nanosleep(&sleeper, NULL);
}

/*
int main(int argc, char **argv)
{
    char **b;
    char buf[1];

    b = (char **)malloc(sizeof(b) * 4);
    *(b + 0) = (char *)malloc(sizeof(*b) * 21);
    *(b + 1) = (char *)malloc(sizeof(*b) * 21);
    *(b + 2) = (char *)malloc(sizeof(*b) * 21);
    *(b + 3) = (char *)malloc(sizeof(*b) * 21);

    bcm2835_init();

    sprintf(*(b + 0), "  Manual");
    sprintf(*(b + 1), "> Comfort");
    sprintf(*(b + 2), "  Purify");
    sprintf(*(b + 3), "  Remote/Local"); 
    init_lcd();
    lcd_display(b);

    buf[0] = 0x08;
    bcm2835_i2c_write(buf, 1);
    return 0;
}
*/
