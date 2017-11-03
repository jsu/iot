#include <DHT.h>
#include <SoftwareSerial.h>

#define PIR_PIN 8
#define LED_PIN 13
#define ACS712_PRD_PIN A7
#define ACS712_FAN_PIN A1
#define PR_PIN A3
#define PR_MIN 100
#define OZONE_PIN A5
#define PM25_SRX 7
#define PM25_STX 6
#define DHTPIN 11
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial PMSerial(PM25_SRX, PM25_STX); /* (RX, TX) */
void call_acs712_fan()
{
  char buffer[32];
  int counter = 0, ivalue;
  float voltage = 0;
  uint32_t start_time = millis();
  while((millis() - start_time) < 1000)
  {
    voltage += analogRead(ACS712_FAN_PIN) * 0.0049;
    counter++;
  }
  voltage /= counter;
  Serial.print("fan_current,");
  Serial.print((voltage - 2.5) / 0.185);
  Serial.print("\n");
}

void call_acs712_prd()
{
  int read_value;
  int max_value = 0;
  int min_value = 1024;
  int mVperAmp = 185;
  float voltage = 0;
  float VRMS = 0;
  float AmpsRMS = 0;
  uint32_t start_time = millis();
  while((millis() - start_time) < 1000)
  {
    read_value = analogRead(ACS712_PRD_PIN);
    if(read_value > max_value)
      max_value = read_value;
    if(read_value < min_value)
      min_value = read_value;
  }

  voltage = ((max_value - min_value) * 5.0) / 1024;
  VRMS = (voltage / 2.0) * 0.707;
  AmpsRMS = (VRMS * 1000) / mVperAmp;
  Serial.print("prd_current,");
  Serial.print(AmpsRMS - 0.3);
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
  int counter;
  float ozone, voltage, ppb, ozone_min = 100.0;
  uint32_t start_time = millis();
  
  while((millis() - start_time) < 1000)
  {
    ozone += analogRead(OZONE_PIN);
    counter++;
  }

  ozone /= counter;
  voltage = ozone / 1024.0 * 5;

  if(voltage <= 2.5 && voltage >= 1.7)
    ppb = (2.5 - voltage) * 1000 + 200;
  else if(voltage < 4)
    ppb = (4 - voltage) * 133;  
  else
    ppb = 0;

  Serial.print("Voltage,");
  Serial.print(voltage);
  Serial.print("\n");
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
  /* First 1 seconds. */
  call_ozone();
  call_PIR();
  call_PM25();
  call_PR();
  call_dht22();
  /* Second 1.5 seconds. */
  call_acs712_fan();
  call_PIR();
  /* Third 1 seconds. */
  call_acs712_prd();
  call_PIR();
}
