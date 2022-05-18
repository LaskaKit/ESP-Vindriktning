#include "pm1006.h"
#include <Adafruit_NeoPixel.h>
#include <SensirionI2CScd4x.h>
#include <Wire.h>

#define PIN_FAN 12
#define PIN_LED 25
#define RXD2 16
#define TXD2 17

#define BRIGHTNESS 10

#define PM_LED 1
#define TEMP_LED 2
#define CO2_LED 3

static PM1006 pm1006(&Serial2);
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(3, PIN_LED, NEO_GRB + NEO_KHZ800);
SensirionI2CScd4x scd4x;

void setup() {
  pinMode(PIN_FAN, OUTPUT); // Fan
  rgbWS.begin(); // WS2718
  rgbWS.setBrightness(BRIGHTNESS);
  
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Wire.begin();
  uint16_t error;
  char errorMessage[256];
  scd4x.begin(Wire);
  
  Serial.println("Start...");
  delay(500);
  Serial.println("1. LED Green");
  setColorWS(0, 255, 0, PM_LED);
  delay(1000);
  Serial.println("2. LED Green");
  setColorWS(0, 255, 0, 2);
  delay(1000);
  Serial.println("3. LED Green");
  setColorWS(0, 255, 0, 3);
  delay(1000);
  setColorWS(0, 0, 0, 1);
  setColorWS(0, 0, 0, 2);
  setColorWS(0, 0, 0, 3);

  // stop potentially previously started measurement
  error = scd4x.stopPeriodicMeasurement();
  if (error) {
      Serial.print("SCD41 Error trying to execute stopPeriodicMeasurement(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
      alert(CO2_LED);
  }

  uint16_t serial0;
  uint16_t serial1;
  uint16_t serial2;
  error = scd4x.getSerialNumber(serial0, serial1, serial2);
  if (error) {
      Serial.print("SCD41 Error trying to execute getSerialNumber(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
      alert(CO2_LED);
  } else {
      printSerialNumber(serial0, serial1, serial2);
  }

  // Start Measurement
  error = scd4x.startPeriodicMeasurement();
  if (error) {
      Serial.print("SCD41 Error trying to execute startPeriodicMeasurement(): ");
      errorToString(error, errorMessage, 256);
      Serial.println(errorMessage);
      alert(CO2_LED);
  }

  Serial.println("Waiting for first measurement... (5 sec)");
}

void loop() {
  uint16_t error;
  char errorMessage[256];

  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(10000);

  uint16_t pm2_5;
  if (pm1006.read_pm25(&pm2_5)) {
    printf("PM2.5 = %u\n", pm2_5);
  } else {
    Serial.println("Measurement failed!");
    alert(PM_LED);
  }

  delay(1000);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");
  
  uint16_t co2;
  float temperature;
  float humidity;
  error = scd4x.readMeasurement(co2, temperature, humidity);
  if (error) {
    Serial.print("SCD41 Error trying to execute readMeasurement(): ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    alert(CO2_LED);
  } else if (co2 == 0) {
    Serial.println("Invalid sample detected, skipping.");
  } else {
    //temperature = temperature -4.0;
    
    Serial.print("Co2:");
    Serial.print(co2);
    Serial.print("\t");
    Serial.print(" Temperature:");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print(" Humidity:");
    Serial.println(humidity);

    if(co2 < 1000){
      setColorWS(0, 255, 0, CO2_LED);
    }
    
    if((co2 >= 1000) && (co2 < 1200)){
      setColorWS(128, 255, 0, CO2_LED);
    }
    
    if((co2 >= 1200) && (co2 < 1500)){
    setColorWS(255, 255, 0, CO2_LED);
    }
    
    if((co2 >= 1500) && (co2 < 2000)){
      setColorWS(255, 128, 0, CO2_LED);
    }
    
    if(co2 >= 2000){
      setColorWS(255, 0, 0, CO2_LED);
    }

    if(temperature < 23.0){
      setColorWS(0, 0, 255, TEMP_LED);
    }

    if((temperature >= 23.0) && (temperature < 28.0)){
      setColorWS(0, 255, 0, TEMP_LED);
    }

    if(temperature >= 28.0){
      setColorWS(255, 0, 0, TEMP_LED);
    }
  }

  // PM LED
  if(pm2_5 < 30){
    setColorWS(0, 255, 0, PM_LED);
  }
  
  if((pm2_5 >= 30) && (pm2_5 < 40)){
    setColorWS(128, 255, 0, PM_LED);
  }
  
  if((pm2_5 >= 40) && (pm2_5 < 80)){
  setColorWS(255, 255, 0, PM_LED);
  }
  
  if((pm2_5 >= 80) && (pm2_5 < 90)){
    setColorWS(255, 128, 0, PM_LED);
  }
  
  if(pm2_5 >= 90){
    setColorWS(255, 0, 0, PM_LED);
  }

  delay(60000);
}

void alert(int id){
  int i = 0;
  while (1){
     if (i > 10){
      Serial.println("Maybe need Reboot...");
      //ESP.restart();
      break;
     }
     rgbWS.setBrightness(255);
     setColorWS(255, 0, 0, id); 
     delay(200);
     rgbWS.setBrightness(BRIGHTNESS);
     setColorWS(0, 0, 0, id);
     delay(200);
     i++;
  }
}

void setColorWS(byte r, byte g, byte b, int id) {  // r = hodnota cervene, g = hodnota zelene, b = hodnota modre, id = cislo LED v poradi, kterou budeme nastavovat(1 = 1. LED, 2 = 2. LED atd.)
  uint32_t rgb;  
  rgb = rgbWS.Color(r, g, b); // Konverze vstupnich hodnot R, G, B do pomocne promenne  
  rgbWS.setPixelColor(id - 1, rgb); // Nastavi pozadovanou barvu pro konkretni led = pozice LED zacinaji od nuly
  rgbWS.show();  // Zaktualizuje barvu
}

void printUint16Hex(uint16_t value) {
    Serial.print(value < 4096 ? "0" : "");
    Serial.print(value < 256 ? "0" : "");
    Serial.print(value < 16 ? "0" : "");
    Serial.print(value, HEX);
}

void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2) {
    Serial.print("SCD41 Serial: 0x");
    printUint16Hex(serial0);
    printUint16Hex(serial1);
    printUint16Hex(serial2);
    Serial.println();
}
