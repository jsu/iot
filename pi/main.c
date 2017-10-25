#include <stdio.h>
#include <mosquitto.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <regex.h>
#include <wiringSerial.h>
#include <bcm2835.h>
#include <signal.h>
#include "lcd.h"
#include "menu.h"

#define SN "03"
#define COMPIN 22
#define LEDPIN 5
#define BUTTON1 23
#define BUTTON2 24
#define RELAYIT 27
#define RELAY1 5
#define RELAY2 6
#define RELAY3 13
#define RELAY4 26
#define RELAY5 12
#define BUZZER 16
#define PWMPIN RPI_BPLUS_GPIO_J8_12
#define PWM_DIVIDER BCM2835_PWM_CLOCK_DIVIDER_2
#define PWM_CHANNEL 0
#define MARK_SPACE 1
#define ENABLE 1
#define RANGE 1024

struct status{
    int LED;
    int machine;
    int EP;
    int UV;
    int ozone;
    int ion;
    int fan;
    int buzzer;
    int pwm_duty;
    int op_mode;
}status;

int strtoint(char *str)
{
    char c, *ptr = str;
    int n = 0;
    /* char c should be 0-9 */
    while((c = *ptr++) >= 0x30 && c <= 0x39 && c != 0x2f)
        n = n * 10 + c - '0';

    return n;
}

void LED_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.LED = flag;
    sprintf(buffer2, "%d", status.LED);
    sprintf(buffer, "XYCS/%s/status/LED", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    bcm2835_gpio_write(LEDPIN, status.LED);
}

void EP_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.EP = flag;
    sprintf(buffer2, "%d", status.EP);
    sprintf(buffer, "XYCS/%s/status/EP", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void UV_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.UV = flag;
    sprintf(buffer2, "%d", status.UV);
    sprintf(buffer, "XYCS/%s/status/UV", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void ozone_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.ozone = flag;
    sprintf(buffer2, "%d", status.ozone);
    sprintf(buffer, "XYCS/%s/status/ozone", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void ion_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.ion = flag;
    sprintf(buffer2, "%d", status.ion);
    sprintf(buffer, "XYCS/%s/status/ion", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void fan_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.fan = flag;
    sprintf(buffer2, "%d", status.fan);
    sprintf(buffer, "XYCS/%s/status/fan", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void buzzer_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.buzzer = flag;
    sprintf(buffer2, "%d", status.buzzer);
    sprintf(buffer, "XYCS/%s/status/buzzer", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void pwm_duty_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    if(flag > 850)
        flag = 850;
    else if(flag < 0)
        flag = 0;
    status.pwm_duty = flag;
    sprintf(buffer2, "%d", status.pwm_duty);
    sprintf(buffer, "XYCS/%s/status/pwm_duty", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    bcm2835_pwm_set_data(PWM_CHANNEL, flag);
}

void op_mode_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    status.op_mode = flag;
    sprintf(buffer2, "%d", status.op_mode);
    sprintf(buffer, "XYCS/%s/status/op_mode", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void machine_switch(struct mosquitto *mosq, int flag, uint8_t long_press)
{
    char buffer[80], buffer2[2], **b, c;
    uint8_t i;
    struct Node *ptr, *current_node;
    status.machine = flag;
    sprintf(buffer2, "%d", status.machine);
    sprintf(buffer, "XYCS/%s/status/machine", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    b = (char **)malloc(sizeof(b) * 4);
    *(b + 0) = (char *)malloc(sizeof(*b) * 21);
    *(b + 1) = (char *)malloc(sizeof(*b) * 21);
    *(b + 2) = (char *)malloc(sizeof(*b) * 21);
    *(b + 3) = (char *)malloc(sizeof(*b) * 21);

    if(status.machine)
    {
        if(long_press)
        {
            lcd_clr();
            mv_to_line(1);
            lcd_str("Rest machine status!");
            pwm_duty_switch(mosq, 0);
        }
        else
        {
            pwm_duty_switch(mosq, 250);
            current_node = ptr = current_menu_node();
            printf("current_menu_node: %d\n", current_node->id);
            for(i = 0; i < 4; i++)
            {
                if(ptr)
                {
                    c = ptr->id == current_node->id ? '>' : ' ';
                    sprintf(*(b + i), "%c %s", c, ptr->name);
                    if(ptr->next)
                        ptr = ptr->next;
                }
                else
                    sprintf(*(b + i), " ");
            }
            lcd_display(b);
        }
    }
    else
        if(long_press)
        {
            pwm_duty_switch(mosq, 0);
            lcd_clr();
            mv_to_line(1);
            lcd_str("Emergency");
            mv_to_line(2);
            lcd_str("     Shutdown");

        }
        else
        {
            pwm_duty_switch(mosq, 0);
            sprintf(*(b + 0), "********************");
            sprintf(*(b + 1), "*  Forest          *");
            sprintf(*(b + 2), "*         Breath   *");
            sprintf(*(b + 3), "********************");
            lcd_display(b);
        }
}

void message_callback(struct mosquitto *mosq, void *userdata,
        const struct mosquitto_message *message)
{
    int i;
    char *buffer;
    char *match;
    regex_t regex;
    int nmatch = 2;
    regmatch_t matchptr[nmatch];
    match = (char *)malloc(sizeof(match) * 80);
    buffer = (char *)malloc(sizeof(buffer) * 80);

    sprintf(buffer, "XYCS/%s/command/([A-Za-z0-9]+)", SN);
    if(regcomp(&regex, buffer, REG_EXTENDED))
    {
        pthread_exit(NULL); 
    }
    if(regexec(&regex, message->topic, nmatch, matchptr, 0))
    {
        pthread_exit(NULL); 
    }
    strcpy(match, message->topic);
    match[matchptr[1].rm_eo] = 0;
    strcpy(match, match + matchptr[1].rm_so);

    if(message->payloadlen){
        if(strcmp(match, "LED") == 0)
        {
            if((i = strtoint(message->payload)) != status.LED)
                LED_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "machine") == 0)
        {
            if((i = strtoint(message->payload)) != status.machine)
                machine_switch(mosq, strtoint(message->payload), 0);
        }
        else if(strcmp(match, "EP") == 0)
        {
            if((i = strtoint(message->payload)) != status.EP)
                EP_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "UV") == 0)
        {
            if((i = strtoint(message->payload)) != status.UV)
                UV_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "ozone") == 0)
        {
            if((i = strtoint(message->payload)) != status.ozone)
                ozone_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "ion") == 0)
        {
            if((i = strtoint(message->payload)) != status.ion)
                ion_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "fan") == 0)
        {
            if((i = strtoint(message->payload)) != status.fan)
                fan_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "buzzer") == 0)
        {
            if((i = strtoint(message->payload)) != status.buzzer)
                buzzer_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "pwm_duty") == 0)
        {
            if((i = strtoint(message->payload)) != status.pwm_duty)
                pwm_duty_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "op_mode") == 0)
        {
            if((i = strtoint(message->payload)) != status.op_mode)
                op_mode_switch(mosq, strtoint(message->payload));
        }
    }else{
        printf("%s (null)\n", message->topic);
    }
    free(buffer);
    free(match);
    regfree(&regex);
    fflush(stdout);
}

void *remote_control_thread(void *mosq)
{
    char buffer[80];
    mosquitto_message_callback_set(mosq, message_callback);
    sprintf(buffer, "XYCS/%s/command/+", SN);
    mosquitto_subscribe(mosq, NULL, buffer, 2);
    mosquitto_loop_forever(mosq, -1, 1);

    pthread_exit(NULL);
}

/**
 * Finite State Machine
 * 0 when not pressed
 * 1 when first press
 * 2 when second press
 * 3 when third press and beyond
 * 4 when first release
 * 5 when second release
 * 0 when third release and beyond (no pressed)
 */
uint8_t FSM(uint8_t flag, uint8_t status)
{
    switch(status)
    {
        case 0:
            if(flag == 1)
                status = 1;
            break;
        case 1: 
            if(flag == 1)
                status = 2;
            else
                status = 1;
            break;
        case 2:
            status = 3;
            break;
        case 3:
            if(flag == 0)
                status = 4;
            break;
        case 4:
            if(flag == 1)
                status = 3;
            else
                status = 5;
            break;
        case 5:
        default:
            status = 0;
    }
    return status;
}

void *local_control_thread(void *mosq)
{
    uint8_t button_status[] = {0, 0, 0, 0, 0};
    uint8_t v = 0x05; /* voltage could be 0x0f when 5v or 0x09 3.3v */
    uint32_t counter = 0;
    uint8_t long_press = 0, i, pos;
    struct timespec sleeper;
    struct Node *ptr, *current_node, *high_node, *low_node, *prev_node;
    char **buffer, c;
    char out_ch1[] = {0x06, 0x40, 0x00};
    char out_ch2[] = {0x06, 0x80, 0x00};
    char out_ch3[] = {0x06, 0xc0, 0x00};
    char out_ch4[] = {0x07, 0x00, 0x00};
    char out_ch5[] = {0x07, 0x40, 0x00};
    char ch_data[] = {0x00, 0x00, 0x00};

    bcm2835_gpio_fsel(COMPIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RELAYIT, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RELAY1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY1, HIGH);
    bcm2835_gpio_fsel(RELAY2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY2, HIGH);
    bcm2835_gpio_fsel(RELAY3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY3, HIGH);
    bcm2835_gpio_fsel(RELAY4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY4, HIGH);
    bcm2835_gpio_fsel(RELAY5, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY5, HIGH);
    bcm2835_gpio_fsel(BUZZER, BCM2835_GPIO_FSEL_OUTP);

    /* Init PWM */
    bcm2835_gpio_fsel(PWMPIN, BCM2835_GPIO_FSEL_ALT5);
    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 110000L;
    nanosleep(&sleeper, NULL);
    bcm2835_pwm_set_clock(PWM_DIVIDER);
    bcm2835_pwm_set_mode(PWM_CHANNEL, MARK_SPACE, ENABLE);
    bcm2835_pwm_set_range(PWM_CHANNEL, RANGE);

    /* Init Menu */
    init_menu();
    buffer = (char **)malloc(sizeof(buffer) * 4);
    *(buffer + 0) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 1) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 2) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 3) = (char *)malloc(sizeof(*buffer) * 21);

    /* Init LCD */
    if(!bcm2835_i2c_begin()) pthread_exit(NULL);
    bcm2835_i2c_setSlaveAddress(LCD_ADDRESS);
    init_lcd();
    sprintf(*(buffer + 0), "********************");
    sprintf(*(buffer + 1), "*  Forest          *");
    sprintf(*(buffer + 2), "*         Breath   *");
    sprintf(*(buffer + 3), "********************");
    lcd_display(buffer);

    if (!bcm2835_spi_begin())
    {
        printf("bcm2835_spi_begin failed. Are you running as root??\n");
        pthread_exit(NULL);
    }
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 10000000L; /* 10ms */
    high_node = low_node = prev_node = NULL;
    for(;;)
    {
        /* Must put sleeper before if-continue stmt */
        nanosleep(&sleeper, NULL);

        bcm2835_spi_transfernb(out_ch1, ch_data, 3);
        button_status[0] = FSM(ch_data[1] > v ? 1 : 0, button_status[0]);
        if(button_status[0] == 5)
        {
            if(counter > 30)
            {
                long_press = 1;
                printf("btn1 [Power Start/Stop] 2 second press\n");
            }
            else
            {
                printf("btn1 [Power Start/Stop] normal press\n");
            }
            reset_current_menu_node();
            high_node = low_node = prev_node = NULL;
            machine_switch(mosq, !status.machine, long_press);
            counter = 0;
            long_press = 0;
        }
        if(button_status[0] == 3)
            counter++;

        /* If machine is not running. Only on/off is available. */
        if(status.machine == 0)
            continue;
        
        bcm2835_spi_transfernb(out_ch2, ch_data, 3);
        button_status[1] = FSM(ch_data[1] > v ? 1 : 0, button_status[1]);
        if(button_status[1] == 5)
        { 
            prev_node = current_menu_node();
            current_node = ptr = menu_u();
            printf("btn2 [Up] pressed %d\n", ptr->id);
            if(high_node && ptr->id == high_node->id)
                high_node = NULL;
            else if(high_node)
                ptr = high_node;
            else
            {
                for(i = 0; i < 4; i++)
                    if(ptr->next)
                        ptr = ptr->next;
                    else
                        break;
                ptr = current_node;
                for(i = i + 1; i < 4; i++)
                    ptr = ptr->prev;
            }
            for(i = 0; i < 4; i++)
            {
                if(ptr)
                {
                    low_node = ptr;
                    c = ptr->id == current_node->id ? '>' : ' ';
                    sprintf(*(buffer + i), "%c %s", c, ptr->name);
                    ptr = ptr->next;
                }
                else
                    sprintf(*(buffer + i), " ");
            }
            lcd_display(buffer); 
        }

        bcm2835_spi_transfernb(out_ch3, ch_data, 3);
        button_status[2] = FSM(ch_data[1] > v ? 1 : 0, button_status[2]);
        if(button_status[2] == 5)
        {
            prev_node = current_menu_node();
            current_node = ptr = menu_d();
            printf("btn3 [Down] pressed %d\n", ptr->id);
            if(low_node && ptr->id == low_node->id)
            {
                low_node = NULL;
                for(i = 0; i < 3; i++)
                    if(ptr->prev)
                        ptr = ptr->prev; 
            }
            else if(low_node)
            {
                printf("low_node: %d\n", low_node->id);  
                ptr = low_node;
                for(i = 0; i < 3; i++)
                    if(ptr->prev)
                        ptr = ptr->prev;
            }
            else
            {
                pos = ptr->id;
                for(i = pos; i > 0 && i > (pos - 3); i--)
                    if(ptr->prev)
                        ptr = ptr->prev; 
                high_node = ptr;
            }
            printf("start from %d\n", ptr->id);
            for(i = 0 ; i < 4; i++)
            {
                if(ptr)
                {
                    c = ptr->id == current_node->id ? '>' : ' ';    
                    sprintf(*(buffer + i), "%c %s", c, ptr->name);
                    ptr = ptr->next;
                }
                else
                    sprintf(*(buffer + i), " ");
            }
            lcd_display(buffer); 
        }

        bcm2835_spi_transfernb(out_ch4, ch_data, 3);
        button_status[3] = FSM(ch_data[1] > v ? 1 : 0, button_status[3]);
        if(button_status[3] == 5)
        {
            prev_node = current_menu_node();
            current_node = ptr = menu_e();
            printf("btn4 [Enter] pressed %d\n", ptr->id);
            if(ptr->parent != NULL && ptr->parent->id == prev_node->id)
            {
                low_node = current_node;
                for(i = 0; i < 3; i++)
                    if(low_node->next)
                        low_node = low_node->next;
                printf("set lownode: %d\n", low_node->id);
                high_node = ptr;
                for(i = 0; i < 4; i++)
                {
                    if(ptr)
                    {
                        c = ptr->id == current_node->id ? '>' : ' ';
                        sprintf(*(buffer + i), "%c %s", c, ptr->name);
                        ptr = ptr->next;
                    }
                    else
                        sprintf(*(buffer + i), " ");
                }
                lcd_display(buffer);
            }
            else
            {
                switch(ptr->id)
                {
                    case 111:
                        pwm_duty_switch(mosq, 0);
                        break;
                    case 112:
                        pwm_duty_switch(mosq, 250);
                        break;
                    case 113:
                        pwm_duty_switch(mosq, 400);
                        break;
                    case 114:
                        pwm_duty_switch(mosq, 550);
                        break;
                    case 115:
                        pwm_duty_switch(mosq, 700);
                        break;
                    case 116:
                        pwm_duty_switch(mosq, 850);
                        break;
                    default:
                        printf("Undefined command %d\n", ptr->id);
                }
            }
        }

        bcm2835_spi_transfernb(out_ch5, ch_data, 3);
        button_status[4] = FSM(ch_data[1] > v ? 1 : 0, button_status[4]);
        if(button_status[4] == 5)
        {
            prev_node = current_menu_node();
            current_node = ptr = menu_b();
            printf("btn5 [Exit] pressed %d prev %d\n", ptr->id, prev_node->id);
            /* prev_node->id > 10 so the top level won't go back. */
            if(prev_node->id > 10 && ptr->id == prev_node->parent->id)
            {
                pos = ptr->id;
                for(i = pos; i > 0 && i > (pos - 3); i--)
                    if(ptr->prev)
                        ptr = ptr->prev; 
                high_node = ptr;
                low_node = NULL;
                for(i = 0; i < 4; i++)
                {
                    if(ptr)
                    {
                        c = ptr->id == current_node->id ? '>' : ' ';
                        sprintf(*(buffer + i), "%c %s", c, ptr->name);
                        ptr = ptr->next;
                    }
                    else
                        sprintf(*(buffer + i), " ");
                }
                lcd_display(buffer);
            }
        }
    }

    bcm2835_spi_end();
    pthread_exit(NULL);
}

void *status_publish_thread(void *mosq)
{
    char *buffer, *buffer2;
    struct timespec sleeper;
    sleeper.tv_sec = 3;
    sleeper.tv_nsec = 0L;
    buffer = (char *)malloc(sizeof(buffer) * 80);
    buffer2 = (char *)malloc(sizeof(buffer2) * 8);

    for(;;)
    {
        sprintf(buffer, "XYCS/%s/status/LED", SN);
        sprintf(buffer2, "%d", status.LED);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/machine", SN);
        sprintf(buffer2, "%d", status.machine);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/EP", SN);
        sprintf(buffer2, "%d", status.EP);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/UV", SN);
        sprintf(buffer2, "%d", status.UV);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/ozone", SN);
        sprintf(buffer2, "%d", status.ozone);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/ion", SN);
        sprintf(buffer2, "%d", status.ion);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/fan", SN);
        sprintf(buffer2, "%d", status.fan);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/buzzer", SN);
        sprintf(buffer2, "%d", status.buzzer);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/pwm_duty", SN);
        sprintf(buffer2, "%d", status.pwm_duty);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/op_mode", SN);
        sprintf(buffer2, "%d", status.op_mode);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        nanosleep(&sleeper, NULL);
    }
    free(buffer);
    free(buffer2);
}

void init_status()
{
    status.LED = 0;
    status.machine = 0;
    status.EP = 0;
    status.UV = 0;
    status.ozone = 0;
    status.ion = 0;
    status.fan = 0;
    status.buzzer = 0;
    status.pwm_duty = 0;
    status.op_mode = 0;
}

void *sensor_publish_thread(void *mosq)
{
    char *buffer, *ptr, *sensor, *value;
    char *portname = "/dev/ttyUSB0";	
    char c;
    const char delim[2] = ",";
    int fd;
    buffer = (char *)malloc(sizeof(buffer) * 80);
    init_status();

    if((fd = serialOpen(portname, 9600)) < 0)
    {
        fprintf(stderr, "Unable to open serial device %s\n", strerror(errno));
        pthread_exit(NULL); 
    }

    for(;;)
    {
        ptr = buffer;
        while((c = serialGetchar(fd)) != '\n')
            *ptr++ = c;
        *ptr = '\0';
        sensor = strtok(buffer, delim);
        value = strtok(NULL, delim);

        if(strcmp(sensor, "humidity") == 0)
        {
            sprintf(buffer, "XYCS/%s/sensor/humidity", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "temperature") == 0)
        {
            sprintf(buffer, "XYCS/%s/sensor/temperature", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "prd_current") == 0)
        {
            sprintf(buffer, "XYCS/%s/sensor/prd_current", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "motion"))
        {
            sprintf(buffer, "XYCS/%s/sensor/motion", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "fan_current"))
        {
            sprintf(buffer, "XYCS/%s/sensor/fan_current", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "UV"))
        {
            sprintf(buffer, "XYCS/%s/sensor/UV", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "ozone"))
        {
            sprintf(buffer, "XYCS/%s/sensor/ozone", SN);
            mosquitto_publish(mosq, NULL, buffer, strlen(value), value, 0,
                    false);
        }
    }
    free(buffer);
    pthread_exit(NULL);
}

void signal_handler(int signum)
{
    bcm2835_pwm_set_data(PWM_CHANNEL, 0);
    bcm2835_close();
    exit(0);
}

int main(int argc, char *argv[])
{
    char *host = "iot.eclipse.org";
    int port = 1883;
    int keepalive = 60;
    struct mosquitto *mosq = NULL;
    pthread_t id_sensor_pub, id_status_pub, id_local_ctrl, id_remote_ctrl;
    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGSTOP, signal_handler);

    bcm2835_init();
    bcm2835_gpio_fsel(COMPIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(COMPIN, 1);
    mosquitto_lib_init();
    mosq = mosquitto_new(NULL, 1, NULL); /* 1 means clean session. */
    if(!mosq){
        fprintf(stderr, "Error: Out of memory.\n");
        return 1;
    }

    if(mosquitto_connect(mosq, host, port, keepalive)){
        fprintf(stderr, "Unable to connect.\n");
        return 1;
    }

    pthread_create(&id_sensor_pub, NULL, sensor_publish_thread, (void *)mosq);
    pthread_create(&id_status_pub, NULL, status_publish_thread, (void *)mosq);
    pthread_create(&id_local_ctrl, NULL, local_control_thread, (void *)mosq);
    pthread_create(&id_remote_ctrl, NULL, remote_control_thread, (void *)mosq);
    pthread_join(id_sensor_pub, NULL);
    pthread_join(id_status_pub, NULL);
    pthread_join(id_local_ctrl, NULL);
    pthread_join(id_remote_ctrl, NULL);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
