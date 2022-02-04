/*
*
* LaskaKit board with ESP32 for IKEA Vindriktning
*
* The board includes the connector for FAN, PM1006 sensor 
* and I2C connector - in this case used for SCD41 (CO2, temp, hum)
*
* The board may be directly replaced one by one. 
* 
* Libraries: 
* PM1006 - https://github.com/bertrik/pm1006
* SCD41 - https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library
*
* made by www.laskakit.cz - 2021
*/

#include "pm1006.h"
#include <Wire.h>
#include "SparkFun_SCD4x_Arduino_Library.h"

#define PIN_FAN 12
#define PIN_LED_G 25
#define PIN_LED_Y 26
#define PIN_LED_R 27
#define RXD2 16
#define TXD2 17

static PM1006 pm1006(&Serial2);
SCD4x SCD41;

bool SCDconnected = false;

void setup() {
  /*-----------------------UART---------------------------------*/
  Serial.begin(9600); // output
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // PM1006
  
  /*-----------------------SCD41---------------------------------*/
  Wire.begin(); //SCD41 I2C initialization
  if (SCD41.begin(false, true) == false)
  {
    SCDconnected = false;
  }
  else
  {
    SCDconnected = true;  
  }

  if (SCDconnected == true) // if SCD41 is connected
  {
    if (SCD41.startLowPowerPeriodicMeasurement() == true)
    {
      Serial.println("Low power mode enabled.");
    }
  }
  /*-----------------------GPIO---------------------------------*/
  pinMode(PIN_FAN, OUTPUT); // Fan
  pinMode(PIN_LED_G, OUTPUT); // Green LED
  pinMode(PIN_LED_R, OUTPUT); // Red LED
  pinMode(PIN_LED_Y, OUTPUT); // Yellow LED
}

void loop() {
  /*-----------------------PM1006---------------------------------*/
  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(10000);

  uint16_t pm2_5;
  if (pm1006.read_pm25(&pm2_5)) 
  {
    Serial.printf("PM2.5 = %u\n", pm2_5);
  } 
  else 
  {
    Serial.println("Measurement failed!\n");
  }

  delay(1000);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");

  /*-----------------------SCD41---------------------------------*/
  if (SCDconnected == true)
  {
    if (SCD41.readMeasurement()) // wait for a new data (approx 30s)
    {
      Serial.print("CO2(ppm):");
      Serial.print(SCD41.getCO2());
  
      Serial.print("\tTemperature(C):");
      Serial.print(SCD41.getTemperature(), 1);
  
      Serial.print("\tHumidity(%RH):");
      Serial.print(SCD41.getHumidity(), 1);
  
      Serial.println();
    }
  }

  /*-----------------------LED---------------------------------*/
  if(pm2_5 < 30){
  digitalWrite(PIN_LED_G, HIGH);
  digitalWrite(PIN_LED_Y, LOW);
  digitalWrite(PIN_LED_R, LOW);
  }
  
  if((pm2_5 > 30) && (pm2_5 < 40)){
  digitalWrite(PIN_LED_G, HIGH);
  digitalWrite(PIN_LED_Y, HIGH);
  digitalWrite(PIN_LED_R, LOW);
  }
  
  if((pm2_5 > 40) && (pm2_5 < 80)){
  digitalWrite(PIN_LED_Y, HIGH);
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_LED_R, LOW);
  }
  
  if((pm2_5 > 80) && (pm2_5 < 90)){
  digitalWrite(PIN_LED_Y, HIGH);
  digitalWrite(PIN_LED_R, HIGH);
  digitalWrite(PIN_LED_G, LOW);
  }
  
  if(pm2_5 > 90){
  digitalWrite(PIN_LED_R, HIGH);
  digitalWrite(PIN_LED_Y, LOW);
  digitalWrite(PIN_LED_G, LOW);
  }

  delay(60000);
}
