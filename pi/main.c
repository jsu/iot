#include <stdio.h>
#include <mosquitto.h>
#include <termios.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <regex.h>
#include <signal.h>

#include "bcm2835.h"
#include "lcd.h"
#include "menu.h"

#define SN "1031"
#define COMPIN 22
#define LEDPIN 5
#define BUTTON1 23

#define RELAY1 5
#define RELAY2 6
#define RELAY3 13
#define RELAY4 26
#define RELAY_ON LOW
#define RELAY_OFF HIGH
#define RELAYIT 27
#define RELAYIT_ON HIGH
#define RELAYIT_OFF LOW
#define BUZZER 16
#define PWMPIN RPI_BPLUS_GPIO_J8_12
#define PWM_DIVIDER BCM2835_PWM_CLOCK_DIVIDER_2
#define PWM_CHANNEL 0
#define PWM_DUTY_250 250
#define PWM_DUTY_400 400
#define PWM_DUTY_550 550
#define PWM_DUTY_700 700
#define PWM_DUTY_850 850
#define MARK_SPACE 1
#define ENABLE 1
#define RANGE 1024
#define ON 1
#define OFF 0
#define MODE_MANUAL 0
#define MODE_COMFORT 1
#define MODE_UV 2
#define MODE_O3 3
#define MODE_HYBRID 4

struct _status{
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
    int remote;
}sstatus;

struct _sensor{
    float humidity;
    float temperature; 
    float prd_current;
    int motion;
    float PM25;
    float fan_current;
    int UV;
    float ozone;
}ssensor;

uint8_t uv_first_run, o3_first_run; 
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int strtoint(char *str)
{
    char c, *ptr = str;
    int n = 0;
    /* char c should be 0-9 */
    while((c = *ptr++) >= '0' && c <= '9')
        n = n * 10 + c - '0';

    return n;
}

void remote_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.remote = flag;
    sprintf(buffer2, "%d", sstatus.remote);
    sprintf(buffer, "XYCS/%s/status/remote", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void EP_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.EP = flag;
    sprintf(buffer2, "%d", sstatus.EP);
    sprintf(buffer, "XYCS/%s/status/EP", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    if(flag)
        bcm2835_gpio_write(RELAY1, RELAY_ON);
    else
        bcm2835_gpio_write(RELAY1, RELAY_OFF);
}

void UV_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.UV = flag;
    sprintf(buffer2, "%d", sstatus.UV);
    sprintf(buffer, "XYCS/%s/status/UV", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    if(flag)
        bcm2835_gpio_write(RELAY2, RELAY_ON);
    else
        bcm2835_gpio_write(RELAY2, RELAY_OFF);
}

void ozone_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.ozone = flag;
    sprintf(buffer2, "%d", sstatus.ozone);
    sprintf(buffer, "XYCS/%s/status/ozone", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    if(flag)
        bcm2835_gpio_write(RELAY3, RELAY_ON);
    else
        bcm2835_gpio_write(RELAY3, RELAY_OFF);
}

void ion_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.ion = flag;
    sprintf(buffer2, "%d", sstatus.ion);
    sprintf(buffer, "XYCS/%s/status/ion", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    if(flag)
        bcm2835_gpio_write(RELAY4, RELAY_ON);
    else
        bcm2835_gpio_write(RELAY4, RELAY_OFF);
}

void fan_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.fan = flag;
    sprintf(buffer2, "%d", sstatus.fan);
    sprintf(buffer, "XYCS/%s/status/fan", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    if(flag)
        bcm2835_gpio_write(RELAYIT, RELAYIT_ON);
    else
        bcm2835_gpio_write(RELAYIT, RELAYIT_OFF);
}

void buzzer_switch(int flag)
{
    sstatus.buzzer = flag;
    bcm2835_gpio_write(BUZZER, flag);
}

void pwm_duty_switch(struct mosquitto *mosq, int flag)
{
    printf("pwm duty %d\n", flag);
    char buffer[80], buffer2[2];
    if(flag > 850)
        flag = 850;
    else if(flag < 0)
        flag = 0;
    sstatus.pwm_duty = flag;
    sprintf(buffer2, "%d", sstatus.pwm_duty);
    sprintf(buffer, "XYCS/%s/status/pwm_duty", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    bcm2835_pwm_set_data(PWM_CHANNEL, flag);
}

void op_mode_switch(struct mosquitto *mosq, int flag)
{
    char buffer[80], buffer2[2];
    sstatus.op_mode = flag;
    sprintf(buffer2, "%d", sstatus.op_mode);
    sprintf(buffer, "XYCS/%s/status/opmode", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
}

void machine_switch(struct mosquitto *mosq, int flag, uint8_t long_press)
{
    char buffer[80], buffer2[2], **b, c;
    uint8_t i;
    struct Node *ptr, *current_node;
    sstatus.machine = flag;
    sprintf(buffer2, "%d", sstatus.machine);
    sprintf(buffer, "XYCS/%s/status/machine", SN);
    mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
            buffer2, 0, false);
    b = (char **)malloc(sizeof(b) * 4);
    *(b + 0) = (char *)malloc(sizeof(*b) * 21);
    *(b + 1) = (char *)malloc(sizeof(*b) * 21);
    *(b + 2) = (char *)malloc(sizeof(*b) * 21);
    *(b + 3) = (char *)malloc(sizeof(*b) * 21);

    if(sstatus.machine)
    {
        if(long_press)
        {
            lcd_clr();
            mv_to_line(1);
            lcd_str("Rest machine status!");
            fan_switch(mosq, OFF);
            pwm_duty_switch(mosq, OFF);
            EP_switch(mosq, OFF);
            ion_switch(mosq, OFF);
            UV_switch(mosq, OFF);
            ozone_switch(mosq, OFF);
            sprintf(*(b + 0), "  Manual");
            sprintf(*(b + 1), "> Comfort");
            sprintf(*(b + 2), "  Purify");
            sprintf(*(b + 3), "  Remote/Local"); 
            pthread_mutex_lock(&mutex);
            lcd_display(b);
            pthread_mutex_unlock(&mutex);
        }
        else
        {
            fan_switch(mosq, ON);
            pwm_duty_switch(mosq, PWM_DUTY_250);
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
            pthread_mutex_lock(&mutex);
            lcd_display(b);
            pthread_mutex_unlock(&mutex);
        }
    }
    else
    {
        fan_switch(mosq, OFF);
        pwm_duty_switch(mosq, OFF);
        EP_switch(mosq, OFF);
        ion_switch(mosq, OFF);
        UV_switch(mosq, OFF);
        ozone_switch(mosq, OFF);
        if(long_press)
        {
            lcd_clr();
            mv_to_line(1);
            lcd_str("Emergency");
            mv_to_line(2);
            lcd_str("     Shutdown");

        }
        else
        {
            sprintf(*(b + 0), "********************");
            sprintf(*(b + 1), "*  Forest          *");
            sprintf(*(b + 2), "*         Breath   *");
            sprintf(*(b + 3), "********************");
            pthread_mutex_lock(&mutex);
            lcd_display(b);
            pthread_mutex_unlock(&mutex);
        }
    }
}

void message_callback(struct mosquitto *mosq, void *userdata,
        const struct mosquitto_message *message)
{
    int i;
    char *buffer, **b;
    char *match;
    regex_t regex;
    int nmatch = 2;
    regmatch_t matchptr[nmatch];
    match = (char *)malloc(sizeof(match) * 80);
    buffer = (char *)malloc(sizeof(buffer) * 80);

    b = (char **)malloc(sizeof(b) * 4);
    *(b + 0) = (char *)malloc(sizeof(*b) * 21);
    *(b + 1) = (char *)malloc(sizeof(*b) * 21);
    *(b + 2) = (char *)malloc(sizeof(*b) * 21);
    *(b + 3) = (char *)malloc(sizeof(*b) * 21);

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

    if(message->payloadlen && sstatus.remote)
    {
        printf("Remote command: %s => %d\n", match, strtoint(message->payload));
        if(strcmp(match, "machine") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.machine)
                machine_switch(mosq, strtoint(message->payload), 0);
        }
        else if(strcmp(match, "EP") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.EP)
                EP_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "UV") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.UV)
                UV_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "ozone") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.ozone)
                ozone_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "ion") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.ion)
                ion_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "fan") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.fan)
                fan_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "buzzer") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.buzzer)
                buzzer_switch(strtoint(message->payload));
        }
        else if(strcmp(match, "pwm") == 0)
        {
            if((i = strtoint(message->payload)) != sstatus.pwm_duty)
                pwm_duty_switch(mosq, strtoint(message->payload));
        }
        else if(strcmp(match, "opmode") == 0)
        {
            if((i = strtoint(message->payload)) == MODE_MANUAL)
            {
                printf("Remote Switch to Manual Mode.\n");
                op_mode_switch(mosq, MODE_MANUAL);
                fan_switch(mosq, ON);
                pwm_duty_switch(mosq, PWM_DUTY_250);
                EP_switch(mosq, OFF);
                ion_switch(mosq, OFF);
                UV_switch(mosq, OFF);
                ozone_switch(mosq, OFF);
                sstatus.op_mode = MODE_MANUAL;
            }
            else
            {
                fan_switch(mosq, ON);
                pwm_duty_switch(mosq, PWM_DUTY_250);
                EP_switch(mosq, OFF);
                ion_switch(mosq, OFF);
                UV_switch(mosq, OFF);
                ozone_switch(mosq, OFF);
                if((i = strtoint(message->payload)) == MODE_COMFORT)
                {
                    op_mode_switch(mosq, MODE_COMFORT);
                    printf("Remote Comfort Mode\n");
                    fan_switch(mosq, ON);
                    pwm_duty_switch(mosq, PWM_DUTY_250);
                    EP_switch(mosq, ON);
                    ion_switch(mosq, ON);
                    sstatus.op_mode = MODE_COMFORT;
                }
                else if((i = strtoint(message->payload)) == MODE_UV)
                {
                    op_mode_switch(mosq, 2);
                    fan_switch(mosq, ON);
                    pwm_duty_switch(mosq, PWM_DUTY_400);
                    UV_switch(mosq, ON);
                    printf("Remote UV Mode\n");
                    sstatus.op_mode = MODE_UV;
                    uv_first_run = 1;
                }
                else if((i = strtoint(message->payload)) == 3)
                {
                    op_mode_switch(mosq, 3);
                    fan_switch(mosq, ON);
                    pwm_duty_switch(mosq, PWM_DUTY_850);
                    ozone_switch(mosq, ON);
                    printf("O3 Mode\n");
                    sstatus.op_mode = MODE_O3;
                    o3_first_run = 1;
                }
            }
        }
    }
    else
        printf("%s (null)\n", message->topic);
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
    uint8_t v = 0x09; /* voltage could be 0x0f when 5v or 0x09 3.3v */
    uint32_t counter = 0, uv_timer, o3_timer;
    uint8_t long_press = 0, i, pos, pressed = 0;
    int32_t time_left;
    time_t uv_time, o3_time, o3m_time;
    struct timespec sleeper, btn_sleeper, bzr_sleeper;
    struct Node *ptr, *current_node, *high_node, *low_node, *prev_node;
    char **buffer, c;
    char out_ch1[] = {0x06, 0x40, 0x00};
    char out_ch2[] = {0x06, 0x80, 0x00};
    char out_ch3[] = {0x06, 0xc0, 0x00};
    char out_ch4[] = {0x07, 0x00, 0x00};
    char out_ch5[] = {0x07, 0x40, 0x00};
    char ch_data[] = {0x00, 0x00, 0x00};

    buffer = (char **)malloc(sizeof(buffer) * 4);
    *(buffer + 0) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 1) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 2) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 3) = (char *)malloc(sizeof(*buffer) * 21);

    /* Machine should not start by default */
    machine_switch(mosq, OFF, 0);

    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 10000000L; /* 10ms */

    high_node = low_node = prev_node = NULL;
    bzr_sleeper.tv_sec = 0;
    bzr_sleeper.tv_nsec = 150000000L;
    buzzer_switch(ON);
    nanosleep(&bzr_sleeper, NULL);
    buzzer_switch(OFF);
    btn_sleeper.tv_sec = 0;
    btn_sleeper.tv_nsec = 600000000L;
    for(;;)
    {
        /* Must put sleeper before if-continue stmt */
        nanosleep(&sleeper, NULL);

        if(pressed)
        {
            nanosleep(&btn_sleeper, NULL);
            pressed = 0; 
        }

        /* Button 1, Start/Stop */
        bcm2835_spi_transfernb(out_ch1, ch_data, 3);
        button_status[0] = FSM(ch_data[1] > v ? 1 : 0, button_status[0]);
        if(button_status[0] == 5)
        {
            pressed = 1;
            if(counter > 30)
            {
                long_press = 1;
                printf("btn1 [Power Start/Stop] 2 second press\n");
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }
            else
            {
                printf("btn1 [Power Start/Stop] normal press\n");
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }
            reset_current_menu_node();
            high_node = low_node = prev_node = NULL;
            machine_switch(mosq, !sstatus.machine, long_press);
            counter = 0;
            long_press = 0;
        }
        if(button_status[0] == 3)
            counter++;

        /* If machine is not running. Only on/off is available. */
        if(sstatus.machine == 0)
            continue;

        bcm2835_spi_transfernb(out_ch2, ch_data, 3);
        button_status[1] = FSM(ch_data[1] > v ? 1 : 0, button_status[1]);
        if(button_status[1] == 5)
        { 
            pressed = 1;
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
            pthread_mutex_lock(&mutex);
            lcd_display(buffer);
            pthread_mutex_unlock(&mutex);
        }

        bcm2835_spi_transfernb(out_ch3, ch_data, 3);
        button_status[2] = FSM(ch_data[1] > v ? 1 : 0, button_status[2]);
        if(button_status[2] == 5)
        {
            pressed = 1;
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
            pthread_mutex_lock(&mutex);
            lcd_display(buffer);
            pthread_mutex_unlock(&mutex);
        }

        bcm2835_spi_transfernb(out_ch4, ch_data, 3);
        button_status[3] = FSM(ch_data[1] > v ? 1 : 0, button_status[3]);
        if(button_status[3] == 5)
        {
            pressed = 1;
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
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
            }
            else
            {
                switch(ptr->id)
                {
                    case 111:
                        if(!uv_timer && !o3_timer)
                        {
                            fan_switch(mosq, OFF);
                            pwm_duty_switch(mosq, OFF);
                        }
                        printf("Fan: Off\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 112:
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_250);
                        printf("PWM DUTY: 250\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 113:
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_400);
                        printf("PWM DUTY: 400\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 114:
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_550);
                        printf("PWM DUTY: 550\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 115:
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_700);
                        printf("PWM DUTY: 700\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 116:
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_850);
                        printf("PWM DUTY: 850\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 121:
                        EP_switch(mosq, ON);
                        printf("EP: On\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 122:
                        EP_switch(mosq, OFF);
                        printf("EP: Off\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 131:
                        ion_switch(mosq, ON);
                        printf("ION: On\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 132:
                        ion_switch(mosq, OFF);
                        printf("ION: Off\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 141:
                        UV_switch(mosq, OFF);
                        uv_time = 0;
                        uv_timer = 0;
                        printf("Manual UV: Off.\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 142:
                        /* 10, 15, 30, 60 minutes */
                        if(ssensor.temperature < 60)
                        {
                            fan_switch(mosq, ON);
                            pwm_duty_switch(mosq, PWM_DUTY_250);
                            UV_switch(mosq, ON);
                            uv_time = time(NULL);
                            uv_timer = 10 * 60;
                            printf("Manual UV: 10 Minutes.\n");
                        }
                        else
                            printf("Manual UV: temperature > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 143:
                        if(ssensor.temperature < 60)
                        {
                            fan_switch(mosq, ON);
                            pwm_duty_switch(mosq, PWM_DUTY_250);
                            UV_switch(mosq, ON);
                            uv_time = time(NULL);
                            uv_timer = 15 * 60;
                            printf("Manual UV: 15 Minutes.\n");
                        }
                        else
                            printf("Manual UV: temperature > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 144:
                        if(ssensor.temperature < 60)
                        {
                            fan_switch(mosq, ON);
                            pwm_duty_switch(mosq, PWM_DUTY_250);
                            UV_switch(mosq, ON);
                            uv_time = time(NULL);
                            uv_timer = 30 * 60;
                            printf("Manual UV: 30 Minutes.\n");
                        }
                        else
                            printf("Manual UV: temperature > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 145:
                        if(ssensor.temperature < 60)
                        {
                            fan_switch(mosq, ON);
                            pwm_duty_switch(mosq, PWM_DUTY_250);
                            UV_switch(mosq, ON);
                            uv_time = time(NULL);
                            uv_timer = 60 * 60;
                            printf("Manual UV: 60 Minutes.\n");
                        }
                        else
                            printf("Manual UV: temperature > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 151:
                        ozone_switch(mosq, OFF);
                        o3_time = 0;
                        o3_timer = 0;
                        printf("Manual O3: Off.\n");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 152:
                        /* 5, 10, 15, 30 minutes */
                        /*if(ssensor.ozone <= 800 && ssensor.temperature < 60)*/
                        if(ssensor.temperature < 60)
                        {
                            ozone_switch(mosq, ON);
                            o3_time = time(NULL);
                            o3_timer = 5 * 60;
                            printf("Manual O3: 5 Minutes.\n");
                        }
                        else
                            printf("Manual O3: O3 > 800 or temp > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 153:
                        /*if(ssensor.ozone <= 800 && ssensor.temperature < 60)*/
                        if(ssensor.temperature < 60)
                        {
                            ozone_switch(mosq, ON);
                            o3_time = time(NULL);
                            o3_timer = 10 * 60;
                            printf("Manual O3: 10 Minutes.\n");
                        }
                        else
                            printf("Manual O3: O3 > 800 or temp > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 154:
                        /*if(ssensor.ozone <= 800 && ssensor.temperature < 60)*/
                        if(ssensor.temperature < 60)
                        {
                            ozone_switch(mosq, ON);
                            o3_time = time(NULL);
                            o3_timer = 15 * 60;
                            printf("Manual O3: 15 Minutes.\n");
                        }
                        else
                            printf("Manual O3: O3 > 800 or temp > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 155:
                        if(ssensor.temperature < 60)
                        {
                            ozone_switch(mosq, ON);
                            o3_time = time(NULL);
                            o3_timer = 30 * 60;
                            printf("Manual O3: 30 Minutes.\n");
                        }
                        else
                            printf("Manual O3: O3 > 800 or temp > 60!");
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 2:
                        /* comfort mode */
                        printf("Comfort Mode: Start.\n");
                        sstatus.op_mode = MODE_COMFORT;
                        op_mode_switch(mosq, 1);
                        fan_switch(mosq, ON);
                        pwm_duty_switch(mosq, PWM_DUTY_250);
                        UV_switch(mosq, OFF);
                        ozone_switch(mosq, OFF);
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 31:
                        if(ssensor.temperature < 60)
                        {
                            op_mode_switch(mosq, 2);
                            printf("UV Mode: Start.\n");
                            sstatus.op_mode = MODE_UV;
                            UV_switch(mosq, ON);
                            fan_switch(mosq, ON);
                            pwm_duty_switch(mosq, PWM_DUTY_250);
                            EP_switch(mosq, OFF);
                            ion_switch(mosq, OFF);
                            uv_first_run = 1;
                        }
                        else
                            printf("UV Mode: Temperature over 60 degree. Can't Start.\n");
                        /* UV, O3, UV & O3 Hybrid */
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 32:
                        /*if(ssensor.ozone <= 800 && ssensor.temperature < 60)*/
                        if(ssensor.temperature < 60)
                        {
                            printf("O3 Mode: Start.\n");
                            op_mode_switch(mosq, 3);
                            sstatus.op_mode = MODE_O3;
                            o3_first_run = 1;
                        }
                        else
                        {
                            sprintf(*(buffer + 0), "==[Purify:O3]=======");
                            sprintf(*(buffer + 1), "Temp: %3.2f C", ssensor.temperature);
                            sprintf(*(buffer + 2), "Temperature > 60 C");
                            sprintf(*(buffer + 3), "O3 Mode Stopped!");
                            pthread_mutex_lock(&mutex);
                            lcd_display(buffer);
                            pthread_mutex_unlock(&mutex);
                        }
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);

                        break;
                    case 41:
                        /* Local, Remote */
                        lcd_sleep();
                        sstatus.remote = OFF;
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    case 42:
                        sstatus.remote = ON;
                        buzzer_switch(ON);
                        nanosleep(&bzr_sleeper, NULL);
                        buzzer_switch(OFF);
                        break;
                    default:
                        printf("Undefined command %d\n", ptr->id);
                }
            }
        }


        if(uv_timer)
        {
            if(time(NULL) - uv_time > uv_timer || ssensor.temperature > 60)
            {
                uv_time = 0;
                uv_timer = 0;
                UV_switch(mosq, OFF);
                op_mode_switch(mosq, 0);
                printf("Manual UV time is up.\n");
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }
        }

        if(o3_timer)
        {
            if(time(NULL) - o3_time > o3_timer || ssensor.temperature > 60)
            {
                o3_time = 0;
                o3_timer = 0;
                ozone_switch(mosq, OFF);
                op_mode_switch(mosq, 0);
                printf("Manual O3 time is up.\n");
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }
        }


        bcm2835_spi_transfernb(out_ch5, ch_data, 3);
        button_status[4] = FSM(ch_data[1] > v ? 1 : 0, button_status[4]);
        if(button_status[4] == 5)
        {
            pressed = 1;
            if(sstatus.op_mode)
            {
                if(sstatus.op_mode == MODE_COMFORT)
                {
                    printf("Comfort_Mode: Shutdown caused by exit.\n");
                    EP_switch(mosq, OFF);
                    ion_switch(mosq, OFF);
                    sprintf(*(buffer + 0), "  Manual");
                    sprintf(*(buffer + 1), "> Comfort");
                    sprintf(*(buffer + 2), "  Purify");
                    sprintf(*(buffer + 3), "  Remote/Local"); 
                    pthread_mutex_lock(&mutex);
                    lcd_display(buffer);
                    pthread_mutex_unlock(&mutex);
                }
                if(sstatus.op_mode == MODE_UV)
                {
                    printf("UV_Mode: Shutdown caused by exit.\n");
                    UV_switch(mosq, OFF);
                }

                if(sstatus.op_mode == MODE_O3)
                {
                    printf("O3_Mode: Shutdown caused by exit.\n");
                    ozone_switch(mosq, OFF);
                }
                pwm_duty_switch(mosq, PWM_DUTY_250);
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
                sstatus.op_mode = MODE_MANUAL;
                op_mode_switch(mosq, 0);
            }
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
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
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
        sprintf(buffer2, "%d", sstatus.LED);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/machine", SN);
        sprintf(buffer2, "%d", sstatus.machine);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/EP", SN);
        sprintf(buffer2, "%d", sstatus.EP);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/UV", SN);
        sprintf(buffer2, "%d", sstatus.UV);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/ozone", SN);
        sprintf(buffer2, "%d", sstatus.ozone);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/ion", SN);
        sprintf(buffer2, "%d", sstatus.ion);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/fan", SN);
        sprintf(buffer2, "%d", sstatus.fan);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/buzzer", SN);
        sprintf(buffer2, "%d", sstatus.buzzer);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/pwm_duty", SN);
        sprintf(buffer2, "%d", sstatus.pwm_duty);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/opmode", SN);
        sprintf(buffer2, "%d", sstatus.op_mode);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        sprintf(buffer, "XYCS/%s/status/remote", SN);
        sprintf(buffer2, "%d", sstatus.remote);
        mosquitto_publish(mosq, NULL, buffer, strlen(buffer2),
                buffer2, 0 , false);
        nanosleep(&sleeper, NULL);
    }
    free(buffer);
    free(buffer2);
}


void *sensor_publish_thread(void *mosq)
{
    char *buffer, *buffer2, *ptr, *sensor, *value;
    char *portname = "/dev/ttyUSB0";	
    speed_t baud = 9600;
    struct termios options;
    char c;
    const char delim[2] = ",";
    int fd;
    buffer = (char *)malloc(sizeof(buffer) * 80);
    buffer2 = (char *)malloc(sizeof(buffer2) * 80);

    if((fd = open(portname, O_RDONLY)) < 0)
    {
        fprintf(stderr, "Unable to open serial device %s\n", strerror(errno));
        pthread_exit(NULL); 
    }
    tcgetattr(fd, &options);
    cfmakeraw(&options);
    cfsetispeed(&options, baud);
    cfsetospeed(&options, baud);

    for(;;)
    {
        ptr = buffer;
        for(;;)
        {
            read(fd, &c, 1); 
            if(c != '\n')
                *ptr++ = c;
            else
                break;
        }
        *ptr = '\0';

        sensor = strtok(buffer, delim);
        value = strtok(NULL, delim);

        if(strcmp(sensor, "humidity") == 0)
        {
            ssensor.humidity = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "temperature") == 0)
        {
            ssensor.temperature = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "prd_current") == 0)
        {
            ssensor.prd_current = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "motion") == 0)
        {
            ssensor.motion = strtoint(value);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "fan_current") == 0)
        {
            ssensor.fan_current = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "PM25") == 0)
        {
            ssensor.PM25 = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "UV") == 0)
        {
            ssensor.UV = strtoint(value);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
        else if(strcmp(sensor, "ozone") == 0)
        {
            ssensor.ozone = strtof(value, NULL);
            sprintf(buffer2, "XYCS/%s/sensor/%s", SN, sensor);
            mosquitto_publish(mosq, NULL, buffer2, strlen(value), value, 0,
                    false);
        }
    }
    free(buffer);
    pthread_exit(NULL);
}

void init_status()
{
    sstatus.LED = 0;
    sstatus.machine = 0;
    sstatus.EP = 0;
    sstatus.UV = 0;
    sstatus.ozone = 0;
    sstatus.ion = 0;
    sstatus.fan = 0;
    sstatus.buzzer = 0;
    sstatus.pwm_duty = 0;
    sstatus.op_mode = 0;
    sstatus.remote = 1;
}

void init_sensor()
{
    ssensor.humidity = 0;
    ssensor.temperature = 0;
    ssensor.prd_current = 0;
    ssensor.motion = 0;
    ssensor.PM25 = 0;
    ssensor.fan_current = 0;
    ssensor.UV = 0;
    ssensor.ozone = 0;
}

void init()
{
    struct timespec sleeper;
    init_status();
    init_sensor();
    bcm2835_init();
    bcm2835_gpio_fsel(COMPIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(COMPIN, 1);
    bcm2835_gpio_fsel(COMPIN, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RELAYIT, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_fsel(RELAY1, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY1, RELAY_OFF);
    bcm2835_gpio_fsel(RELAY2, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY2, RELAY_OFF);
    bcm2835_gpio_fsel(RELAY3, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY3, RELAY_OFF);
    bcm2835_gpio_fsel(RELAY4, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAY4, RELAY_OFF);
    bcm2835_gpio_fsel(RELAYIT, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(RELAYIT, RELAYIT_OFF);
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

    /* Init LCD */
    bcm2835_i2c_begin();
    bcm2835_i2c_setSlaveAddress(LCD_ADDRESS);
    init_lcd();

    /* Init SPI */
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_65536);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    mosquitto_lib_init();
}

void signal_handler(int signum)
{
    bcm2835_pwm_set_data(PWM_CHANNEL, 0);
    bcm2835_gpio_write(RELAY1, RELAY_OFF);
    bcm2835_gpio_write(RELAY2, RELAY_OFF);
    bcm2835_gpio_write(RELAY3, RELAY_OFF);
    bcm2835_gpio_write(RELAY4, RELAY_OFF);
    bcm2835_gpio_write(RELAYIT, RELAYIT_OFF);

    bcm2835_close();
    exit(0);
}

void *op_mode_comfort_thread(void *mosq)
{
    uint32_t comfort_timer;
    time_t comfort_time;
    char **buffer;
    uint16_t pwm_duty = 0;
    struct timespec sleeper;

    buffer = (char **)malloc(sizeof(buffer) * 4);
    *(buffer + 0) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 1) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 2) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 3) = (char *)malloc(sizeof(*buffer) * 21);

    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 100L * 1000 * 1000; /* 10ms */
    for(;;)
    {
        nanosleep(&sleeper, NULL);
        if(sstatus.op_mode == MODE_COMFORT)
        {
            sprintf(*(buffer + 0), "==[Comfort]=========");
            sprintf(*(buffer + 1), "PM25: %3.0f ug/m3", ssensor.PM25);
            sprintf(*(buffer + 2), "Temp: %3.1f C", ssensor.temperature);
            sprintf(*(buffer + 3), "Humi: %3.1f %%", ssensor.humidity);
            pthread_mutex_lock(&mutex);
            lcd_display(buffer);
            pthread_mutex_unlock(&mutex);

            if(comfort_timer)
            {
                if(ssensor.PM25 < 10 &&
                        time(NULL) - comfort_time > comfort_timer)
                {
                    EP_switch(mosq, OFF);
                    ion_switch(mosq, OFF);
                    comfort_time = 0;
                    comfort_timer = 0;
                }
                else
                    comfort_time = time(NULL);
            }
            else if(ssensor.PM25 < 10)
            {
                comfort_time = time(NULL);
                comfort_timer = 10 * 60;
            }
            else if(ssensor.PM25 >= 15)
            {
                EP_switch(mosq, ON);
                ion_switch(mosq, ON);
            }

            pwm_duty = (((ssensor.PM25 - 15) / 15) + 1) * 250;
            if(pwm_duty < PWM_DUTY_250)
                pwm_duty = PWM_DUTY_250;
            else if(pwm_duty > PWM_DUTY_850)
                pwm_duty = PWM_DUTY_850;
            if(sstatus.op_mode == MODE_COMFORT && sstatus.pwm_duty != pwm_duty)
                pwm_duty_switch(mosq, pwm_duty);
        }
    }
} 


void *op_mode_uv_thread(void *mosq)
{
    uint32_t uvm_timer, time_left;
    int8_t uv_direction;
    uint32_t uv_counter;
    time_t uvm_time;
    char **buffer;
    uint16_t pwm_duty = 0;
    struct timespec sleeper, bzr_sleeper;

    buffer = (char **)malloc(sizeof(buffer) * 4);
    *(buffer + 0) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 1) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 2) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 3) = (char *)malloc(sizeof(*buffer) * 21);

    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 100L * 1000 * 1000; /* 10ms */

    bzr_sleeper.tv_sec = 0;
    bzr_sleeper.tv_nsec = 150000000L;
    for(;;)
    {
        nanosleep(&sleeper, NULL);
        if(sstatus.op_mode == MODE_UV)
        {
            if(uv_first_run)
            {
                uvm_time = time(NULL);
                uvm_timer = 60;
                uv_counter = 0;
                uv_first_run = 0;
            }

            uv_counter++;
            if(uv_counter >= 10)
            {
                if(pwm_duty <= 250)
                { 
                    pwm_duty = PWM_DUTY_250;
                    uv_direction = 10;    
                }
                else if(pwm_duty >= 850)
                {
                    pwm_duty = PWM_DUTY_850;
                    uv_direction = -10;
                }
                pwm_duty += uv_direction;
                pwm_duty_switch(mosq, pwm_duty);
                uv_counter = 0;
            }

            if(ssensor.temperature > 60)
            {
                sprintf(*(buffer + 0), "==[Purify:UV]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Temperature > 60 C");
                sprintf(*(buffer + 3), "UV Mode Stopped!");
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
                printf("UV_Mode: Temperature over 60 degree celsius.\n");
                UV_switch(mosq, OFF);
                sstatus.op_mode = MODE_MANUAL;
                pwm_duty_switch(mosq, 250);
                uv_counter = 0;
                op_mode_switch(mosq, 0);
                sstatus.op_mode = MODE_MANUAL;
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }

            if(uvm_timer == 60)
            {
                time_left = 600 - (time(NULL) - uvm_time);
                sprintf(*(buffer + 0), "==[Purify:UV]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                sprintf(*(buffer + 3), "Time left: %d sec.", time_left);
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
                if(time(NULL) - uvm_time > uvm_timer)
                {
                    printf("UV_Mode: First minute ends.\n");
                    uvm_time = time(NULL);
                    uvm_timer = 540;
                }
            }

            if(uvm_timer == 540)
            {
                time_left = 540 - 1 - (time(NULL) - uvm_time);
                sprintf(*(buffer + 0), "==[Purify:UV]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                sprintf(*(buffer + 3), "Time left: %d sec.", time_left);
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
                if(time(NULL) - uvm_time > uvm_timer)
                {
                    sprintf(*(buffer + 0), "==[Purify:UV]=======");
                    sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                    sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                    sprintf(*(buffer + 3), "Time's Up.");
                    pthread_mutex_lock(&mutex);
                    lcd_display(buffer);
                    pthread_mutex_unlock(&mutex);

                    printf("UV_Mode: 10 minutes timed up.\n");
                    UV_switch(mosq, OFF);
                    pwm_duty_switch(mosq, 250);
                    uv_counter = 0;
                    op_mode_switch(mosq, 0);
                    sstatus.op_mode = MODE_MANUAL;
                    buzzer_switch(ON);
                    nanosleep(&bzr_sleeper, NULL);
                    buzzer_switch(OFF);
                }

                if(ssensor.motion)
                {
                    sprintf(*(buffer + 0), "==[Purify:UV]=======");
                    sprintf(*(buffer + 1), "Object movement");
                    sprintf(*(buffer + 2), "           detected.");
                    sprintf(*(buffer + 3), "UV Mode Stopped!");
                    pthread_mutex_lock(&mutex);
                    lcd_display(buffer);
                    pthread_mutex_unlock(&mutex);
                    printf("UV_Mode: You have moved\n");
                    UV_switch(mosq, OFF);
                    sstatus.op_mode = MODE_MANUAL;
                    pwm_duty_switch(mosq, 250);
                    uv_counter = 0;
                    op_mode_switch(mosq, 0);
                    buzzer_switch(ON);
                    nanosleep(&bzr_sleeper, NULL);
                    buzzer_switch(OFF);
                }
            }
        }
    }
} 

void *op_mode_o3_thread(void *mosq)
{
    uint32_t o3m_timer, time_left;
    time_t o3m_time;
    char **buffer;
    struct timespec sleeper, bzr_sleeper;

    buffer = (char **)malloc(sizeof(buffer) * 4);
    *(buffer + 0) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 1) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 2) = (char *)malloc(sizeof(*buffer) * 21);
    *(buffer + 3) = (char *)malloc(sizeof(*buffer) * 21);

    sleeper.tv_sec = 0;
    sleeper.tv_nsec = 100L * 1000 * 1000; /* 10ms */

    bzr_sleeper.tv_sec = 0;
    bzr_sleeper.tv_nsec = 150000000L;
    for(;;)
    {
        nanosleep(&sleeper, NULL);
        if(sstatus.op_mode == MODE_O3)
        {
            if(o3_first_run)
            {
                o3m_time = time(NULL);
                o3m_timer = 60;
                o3_first_run = 0;
                pwm_duty_switch(mosq, PWM_DUTY_850);
                ozone_switch(mosq, ON);
                EP_switch(mosq, OFF);
                ion_switch(mosq, OFF);
            }

            /*
            if(ssensor.ozone > 800)
            {
                sprintf(*(buffer + 0), "==[Purify:O3]=======");
                sprintf(*(buffer + 1), "O3: %3.2f C", ssensor.ozone);
                sprintf(*(buffer + 2), "O3 over 800 ppb.");
                sprintf(*(buffer + 3), "O3 Mode Stopped!");
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);
                printf("O3_Mode: O3 over 800, STOP.\n");
                pwm_duty_switch(mosq, 250); 
                ozone_switch(mosq, OFF);
                sstatus.op_mode = MODE_MANUAL;
            }
            */

            if(ssensor.temperature > 60)
            {
                sprintf(*(buffer + 0), "==[Purify:O3]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Temperature > 60 C");
                sprintf(*(buffer + 3), "O3 Mode Stopped!");
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);

                printf("O3_Mode: Temperature over 60. !!!!\n");
                ozone_switch(mosq, OFF);
                pwm_duty_switch(mosq, PWM_DUTY_250);
                sstatus.op_mode = MODE_MANUAL;
                op_mode_switch(mosq, 0);
                buzzer_switch(ON);
                nanosleep(&bzr_sleeper, NULL);
                buzzer_switch(OFF);
            }

            if(o3m_timer == 60)
            {
                time_left = 600 - (time(NULL) - o3m_time);
                sprintf(*(buffer + 0), "==[Purify:O3]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                sprintf(*(buffer + 3), "Time left: %d sec.", time_left);
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);

                if(time(NULL) - o3m_time > o3m_timer)
                {
                    printf("O3_Mode: First minute up.\n");
                    o3m_time = time(NULL);
                    o3m_timer = 540;
                }
            }

            if(o3m_timer == 540)
            {
                time_left = 540 - 1 - (time(NULL) - o3m_time);
                sprintf(*(buffer + 0), "==[Purify:O3]=======");
                sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                sprintf(*(buffer + 3), "Time left: %d sec.", time_left);
                pthread_mutex_lock(&mutex);
                lcd_display(buffer);
                pthread_mutex_unlock(&mutex);

                if(time(NULL) - o3m_time > o3m_timer)
                {
                    sprintf(*(buffer + 0), "==[Purify:O3]=======");
                    sprintf(*(buffer + 1), "Temp: %3.1f C", ssensor.temperature);
                    sprintf(*(buffer + 2), "Humi: %3.1f %%", ssensor.humidity);
                    sprintf(*(buffer + 3), "Time's Up.");
                    pthread_mutex_lock(&mutex);
                    lcd_display(buffer);
                    pthread_mutex_unlock(&mutex);
                    printf("O3_Mode: Time is up.\n");
                    ozone_switch(mosq, OFF);
                    pwm_duty_switch(mosq, PWM_DUTY_250);
                    sstatus.op_mode = MODE_MANUAL;
                    op_mode_switch(mosq, 0);
                    buzzer_switch(ON);
                    nanosleep(&bzr_sleeper, NULL);
                    buzzer_switch(OFF);
                }

                if(ssensor.motion)
                {
                    sprintf(*(buffer + 0), "==[Purify:O3]=======");
                    sprintf(*(buffer + 1), "Object movement");
                    sprintf(*(buffer + 2), "           detected.");
                    sprintf(*(buffer + 3), "O3 Mode Stopped!");
                    pthread_mutex_lock(&mutex);
                    lcd_display(buffer);
                    pthread_mutex_unlock(&mutex);
                    printf("O3_Mode: You have moved !!!!\n");
                    ozone_switch(mosq, OFF);
                    pwm_duty_switch(mosq, PWM_DUTY_250);
                    sstatus.op_mode = MODE_MANUAL;
                    op_mode_switch(mosq, 0);
                    buzzer_switch(ON);
                    nanosleep(&bzr_sleeper, NULL);
                    buzzer_switch(OFF);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    char *host = "iot.eclipse.org";
    int port = 1883;
    int keepalive = 60;
    struct mosquitto *mosq = NULL;
    pthread_t id_sensor_pub, id_status_pub, id_local_ctrl, id_remote_ctrl;
    pthread_t id_op_mode_comfort, id_op_mode_uv, id_op_mode_o3;
    signal(SIGINT, signal_handler);
    signal(SIGKILL, signal_handler);
    signal(SIGSTOP, signal_handler);

    init();

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
    pthread_create(&id_op_mode_comfort, NULL, op_mode_comfort_thread, (void *)mosq);
    pthread_create(&id_op_mode_uv, NULL, op_mode_uv_thread, (void *)mosq);
    pthread_create(&id_op_mode_o3, NULL, op_mode_o3_thread, (void *)mosq);
    pthread_join(id_sensor_pub, NULL);
    pthread_join(id_status_pub, NULL);
    pthread_join(id_local_ctrl, NULL);
    pthread_join(id_remote_ctrl, NULL);
    pthread_join(id_op_mode_comfort, NULL);
    pthread_join(id_op_mode_uv, NULL);
    pthread_join(id_op_mode_o3, NULL);

    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
