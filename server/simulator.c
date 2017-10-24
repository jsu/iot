#include <stdio.h>
#include <mosquitto.h>
#include <pthread.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

struct ssensors{
    float temperature;
    float humidity;
    int motion;
    float PM25;
    float fan_current;
    int UV;
    float ozone;
    float prd_current;
}sensors;

struct sstatus{
    struct ssensors sensors;
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

void message_callback(struct mosquitto *mosq, void *userdata,
        const struct mosquitto_message *message)
{
	if(message->payloadlen){
        if(strcmp(message->topic, "XYCS/01/command/machine") == 0)
            status.machine = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/EP") == 0)
            status.EP = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/UV") == 0)
            status.UV = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/ozone") == 0)
            status.ozone = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/ion") == 0)
            status.ion = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/fan") == 0)
            status.fan = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/buzzer") == 0)
            status.buzzer = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/pwm_duty") == 0)
            status.pwm_duty = strtoint(message->payload);
        else if(strcmp(message->topic, "XYCS/01/command/op_mode") == 0)
            status.op_mode = strtoint(message->payload);
	}else{
		printf("%s (null)\n", message->topic);
	}
	fflush(stdout);
}

void *mosq_subscribe_thread(void *mosq)
{
	mosquitto_message_callback_set(mosq, message_callback);
    mosquitto_subscribe(mosq, NULL, "XYCS/01/command/+", 2);
	mosquitto_loop_forever(mosq, -1, 1);

    pthread_exit(NULL);
}

void init_status()
{
    status.sensors = sensors;

    /* initinal value assignments */
    status.sensors.temperature = 28.0;
    status.sensors.humidity = 80.0;
    status.sensors.motion = 0;
    status.sensors.PM25 = 100;
    status.sensors.fan_current = 2.5;
    status.sensors.UV = 0;
    status.sensors.ozone = 45.0;
    status.sensors.prd_current = 2.5;
    status.machine = 1; /* stop */
    status.EP = 0;
    status.UV = 0;
    status.ozone = 0;
    status.ion = 0;
    status.fan = 0;
    status.buzzer = 0;
    status.pwm_duty = 0;
    status.op_mode = 0;
}

void *mosq_publish_thread(void *mosq)
{
    char *buffer;
    struct timespec sleeper;
    sleeper.tv_sec = 10;
    sleeper.tv_nsec = 0L;
    buffer = (char *)malloc(sizeof(buffer) * 8);
    srand(time(NULL));
    init_status();

    for(;;)
    {
        /**
         * Many sensors here
         */
        /* float 1 - 100, make it 15 - 38 degree celsius */
        status.sensors.temperature += ((float)(rand() % 20) - 10) / 10;
        if(status.sensors.temperature < 0 || status.sensors.temperature > 100)
            status.sensors.temperature = 28;
        sprintf(buffer, "%.1f", status.sensors.temperature);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/temperature",
                strlen(buffer), buffer, 0, false);
        /* float 0 - 100, make it 70 - 90 % */
        status.sensors.humidity += ((float)(rand() % 20) - 10) / 10;
        if(status.sensors.humidity < 0 || status.sensors.humidity > 100)
            status.sensors.humidity = 80;
        sprintf(buffer, "%.1f", status.sensors.humidity);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/humidity",
                strlen(buffer), buffer, 0, false);
        /* 0/1 */
        sprintf(buffer, "%d", status.sensors.motion);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/motion", strlen(buffer),
                buffer, 0, false);
        /* float 0 - 1000, 10 - 200 µg/m3 */
        status.sensors.PM25 += ((float)(rand() % 100) - 50) / 10;
        if(status.sensors.PM25 < 0 || status.sensors.PM25 > 1000)
            status.sensors.PM25 = 100;
        sprintf(buffer, "%.1f", status.sensors.PM25);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/PM25", strlen(buffer),
                buffer, 0, false);
        /* float 0 - 5 A */
        status.sensors.fan_current += ((float)(rand() % 4) - 2) / 10;
        if(status.sensors.fan_current < 1.5 || status.sensors.fan_current > 3.5)
            status.sensors.fan_current = 2.5;
        sprintf(buffer, "%.1f", status.sensors.fan_current);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/fan_current",
                strlen(buffer), buffer, 0, false);
        /* 0/1 */
        sprintf(buffer, "%d", status.sensors.UV);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/UV", strlen(buffer),
                buffer, 0, false);
        /* float 0 - 1000, 10 - 100 ppb */
        status.sensors.ozone += ((float)(rand() % 40) - 20) / 10;
        if(status.sensors.ozone < 0 || status.sensors.ozone > 1000)
            status.sensors.ozone = 45.0;
        sprintf(buffer, "%.1f", status.sensors.ozone);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/ozone", strlen(buffer),
                buffer, 0, false);
        /* float 0 - 5 A */
        status.sensors.prd_current += ((float)(rand() % 4) - 2) / 10;
        if(status.sensors.prd_current < 1.5 || status.sensors.prd_current > 3.5)
            status.sensors.prd_current = 2.5;
        sprintf(buffer, "%.1f", status.sensors.prd_current);
        mosquitto_publish(mosq, NULL, "XYCS/01/sensor/prd_current",
                strlen(buffer), buffer, 0, false);
        
        /**
         * Lots of control statuses here
         */
        /* only simple on / off */
        sprintf(buffer, "%d", status.machine);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/machine", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.EP);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/EP", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.UV);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/s-UV", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.ozone);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/s-ozone", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.ion);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/ion", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.fan);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/fan", strlen(buffer),
                buffer, 0, false);
        sprintf(buffer, "%d", status.buzzer);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/buzzer", strlen(buffer),
                buffer, 0, false);
        /* float 0, 200 - 850 */
        sprintf(buffer, "%d", status.pwm_duty);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/pwm_duty",strlen(buffer),
                buffer, 0, false);
        /* int 0 - 2 or more... */
        sprintf(buffer, "%d", status.op_mode);
        mosquitto_publish(mosq, NULL, "XYCS/01/status/op_mode", strlen(buffer),
                buffer, 0, false);
        nanosleep(&sleeper, NULL);
    }
    free(buffer);
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	char *host = "iot.eclipse.org";
	int port = 1883;
	int keepalive = 60;
	bool clean_session = true;
	struct mosquitto *mosq = NULL;
    pthread_t id_subscribe, id_publish;

	mosquitto_lib_init();
	mosq = mosquitto_new(NULL, clean_session, NULL);
	if(!mosq){
		fprintf(stderr, "Error: Out of memory.\n");
		return 1;
	}

	if(mosquitto_connect(mosq, host, port, keepalive)){
		fprintf(stderr, "Unable to connect.\n");
		return 1;
	}

    pthread_create(&id_subscribe, NULL, mosq_subscribe_thread, (void *)mosq);
    pthread_create(&id_publish, NULL, mosq_publish_thread, (void *)mosq);
    pthread_join(id_subscribe, NULL);
    pthread_join(id_publish, NULL);

	mosquitto_destroy(mosq);
	mosquitto_lib_cleanup();
	return 0;
}
