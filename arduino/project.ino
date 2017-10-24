#include <DHT.h>
#include <SoftwareSerial.h>

#define PIR_PIN 8
#define LED_PIN 13
#define ACS712_PRD_PIN 7
#define ACS712_FAN_PIN 1
#define PR_PIN 3
#define PR_MIN 100
#define OZONE_PIN 5
#define PM25_SRX 7
#define PM25_STX 6
#define DHTPIN 11
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial PMSerial(PM25_SRX, PM25_STX); /* (RX, TX) */
void call_acs712_fan()
{
  char buffer[32];
  int i, ivalue;
  float voltage = 0;

  for(i = 0; i < 1500; i++)
  {
    voltage += analogRead(ACS712_FAN_PIN) * 0.0049; /* 5 / 1024 = 0.0048828125 */
    delay(1);
  }
  voltage /= 1500.0;
  ivalue = int((voltage - 2.5) / 0.185); /* avg voltage to current */
  Serial.print("fan_current,");
  Serial.print((voltage - 2.5) / 0.185);
  Serial.print("\n");
}

void call_acs712_prd()
{
  char buffer[32];
  int i, ivalue;
  float voltage = 0;

  for(i = 0; i < 1500; i++)
  {
    voltage += analogRead(ACS712_PRD_PIN) * 0.0049; /* 5 / 1024 = 0.0048828125 */
    delay(1);
  }
  voltage /= 1500.0;
  ivalue = int((voltage - 2.5) / 0.185); /* avg voltage to current */
  Serial.print("prd_current,");
  Serial.print((voltage - 2.5) / 0.185);
  Serial.print("\n");
}

void call_dht22()
{
  Serial.print("humidity,");
  Serial.print(dht.readHumidity());
  Serial.print("\n");
  Serial.print("temperature,");
  Serial.print(dht.readTemperature());
  Serial.print("\n");
}

void call_PIR()
{
  int pir_status;
  
  if(digitalRead(PIR_PIN))
    Serial.print("motion,1\n");
  else
    Serial.print("motion,0\n");
}

void call_PR()
{
  if(analogRead(PR_PIN) > PR_MIN)
    Serial.print("UV,0\n");
  else
    Serial.print("UV,1\n");
}

void call_ozone()
{
  int i;
  float ozone, voltage, ppb, ozone_min = 100.0;

  for(i = 0; i < 500; i++)
  {
    ozone += analogRead(OZONE_PIN);
    delay(3);  
  }

  ozone /= 500.0;
  voltage = ozone / 1024.0 * 5.0;

  if(voltage > 1 && voltage <= 2.7)
    ppb = (voltage - 1.7) * 140 + 20;
  else if(voltage > 2.7 && voltage <= 3.8)
    ppb = (voltage - 2.7) * 764 + 160;
  else
    return;

  Serial.print("ozone,");
  Serial.print(ppb);
  Serial.print("\n");
}

int check_value(unsigned char *buffer, int buffer_length)
{
  int i, sum = 0, flag = 0;
  for(i = 0; i < (buffer_length - 2); i++)
    sum += buffer[i];
  sum += 0x42;

  if(sum == ((buffer[buffer_length - 2] << 8) + buffer[buffer_length - 1])) 
    flag = 1;
 
  return flag;
}

void call_PM25()
{
  int buffer_length = 31;
  unsigned char buffer[buffer_length]; /* 0x42 + 31 bytes equals to 32 bytes */

  if(PMSerial.find(0x42))
  {
    PMSerial.readBytes(buffer, buffer_length);
    if(buffer[0] == 0x4d && check_value(buffer, buffer_length))
    {
      Serial.print("PM25,");
      Serial.print((buffer[5] << 8) + buffer[6]);
      Serial.print("\n");
    }
  }
}

void setup() 
{
  Serial.begin(9600);
  PMSerial.begin(9600);
  PMSerial.setTimeout(1500);
  dht.begin();
}

void loop()
{
  /* First 1.5 seconds. */
  call_ozone();
  call_PIR();
  call_PM25();
  call_PR();
  call_dht22();
  /* Second 1.5 seconds. */
  call_acs712_fan();
  call_PIR();
  /* Third 1.5 seconds. */
  call_acs712_prd();
  call_PIR();
}
