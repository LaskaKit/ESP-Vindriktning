#include "pm1006.h"

#define PIN_FAN 12
#define PIN_LED_G 25
#define PIN_LED_Y 26
#define PIN_LED_R 27
//#define RXD2 16
//#define TXD2 17
#define RXD2 17
#define TXD2 16

static PM1006 pm1006(&Serial2);

void setup() {
  pinMode(PIN_FAN, OUTPUT); // Fan
  pinMode(PIN_LED_G, OUTPUT); // Green LED
  pinMode(PIN_LED_R, OUTPUT); // Red LED
  pinMode(PIN_LED_Y, OUTPUT); // Yellow LED
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.println("Start...");
  delay(500);
  Serial.println("Green");
  digitalWrite(PIN_LED_G, HIGH);
  delay(1000);
  Serial.println("Yellow");
  digitalWrite(PIN_LED_G, LOW);
  digitalWrite(PIN_LED_Y, HIGH);
  delay(1000);
  Serial.println("Red");
  digitalWrite(PIN_LED_Y, LOW);
  digitalWrite(PIN_LED_R, HIGH);
  delay(1000);
  digitalWrite(PIN_LED_R, LOW);
}

void loop() {

  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(10000);

  uint16_t pm2_5;
  if (pm1006.read_pm25(&pm2_5)) {
    printf("PM2.5 = %u\n", pm2_5);
  } else {
    printf("Measurement failed!\n");
  }

  delay(1000);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");

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
