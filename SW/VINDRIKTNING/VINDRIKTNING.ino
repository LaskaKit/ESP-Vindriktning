// LaskaKit ESP-VINDRIKTNING v2.0 demo software

#include "pm1006.h"
#include <Adafruit_NeoPixel.h>
#include <Wire.h>

// Pin assignment
#define PIN_FAN 12
#define PIN_LED 25
#define PIN_AMBIENT_LIGHT 4
#define RXD2 16
#define TXD2 17

// Brightness control
#define BRIGHTNESS_DAY 50   // Day brightness
#define BRIGHTNESS_NIGHT 5  // Night brightness
#define DAY_MIN_AL 3700     // Minimum ambient light for day

// PM 2.5 sensor
static PM1006 pm1006(&Serial2);

// RGB LEDs
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(3, PIN_LED, NEO_GRB + NEO_KHZ800);

void setup() {
  // Enable serial communication
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(500);
  Serial.println("Start...");

  // Setup pins
  pinMode(PIN_FAN, OUTPUT);           // Fan
  pinMode(PIN_AMBIENT_LIGHT, INPUT);  // Ambient light

  // Setup WS2718 LEDs
  rgbWS.begin();
  rgbWS.setBrightness(BRIGHTNESS_DAY);

  Serial.println("1. LED Red");
  setColorWS(255, 0, 0, 1);
  Serial.println("2. LED Green");
  setColorWS(0, 255, 0, 2);
  Serial.println("3. LED Blue");
  setColorWS(0, 0, 255, 3);
  delay(5000);

  setColorWS(0, 0, 0, 1);
  setColorWS(0, 0, 0, 2);
  setColorWS(0, 0, 0, 3);
}

void loop() {
  // Read ambient light and set LED brightness
  int al = analogRead(PIN_AMBIENT_LIGHT);
  Serial.print("Ambient light: ");
  Serial.println(al);
  if (al >= DAY_MIN_AL) {
    rgbWS.setBrightness(BRIGHTNESS_NIGHT);
  } else {
    rgbWS.setBrightness(BRIGHTNESS_DAY);
  }

  // Turn fan ON
  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(10000);

  // Read dust particles
  uint16_t pm2_5;
  if (pm1006.read_pm25(&pm2_5)) {
    printf("PM2.5 = %u\n", pm2_5);
  } else {
    Serial.println("Measurement failed!");
    alert(2);
  }

  // Turn fan off
  delay(1000);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");

  // Set LEDs to indicate status
  if (pm2_5 < 30) {
    setColorWS(0, 255, 0, 1);
    setColorWS(0, 255, 0, 2);
    setColorWS(0, 255, 0, 3);
  } else if (pm2_5 < 40) {
    setColorWS(128, 255, 0, 1);
    setColorWS(128, 255, 0, 2);
    setColorWS(128, 255, 0, 3);
  } else if (pm2_5 < 80) {
    setColorWS(255, 255, 0, 1);
    setColorWS(255, 255, 0, 2);
    setColorWS(255, 255, 0, 3);
  } else if (pm2_5 < 90) {
    setColorWS(255, 128, 0, 1);
    setColorWS(255, 128, 0, 2);
    setColorWS(255, 128, 0, 3);
  } else {
    setColorWS(255, 0, 0, 1);
    setColorWS(255, 0, 0, 2);
    setColorWS(255, 0, 0, 3);
  }

  // Wait for 30 seconds
  delay(30000);
}

void alert(int id) {
  int i = 0;
  while (1) {
    if (i > 10) {
      Serial.println("Maybe need Reboot...");
      //ESP.restart();
      break;
    }
    rgbWS.setBrightness(255);
    setColorWS(255, 0, 0, id);
    delay(200);
    setColorWS(0, 0, 0, id);
    delay(200);
    i++;
  }
}

void setColorWS(byte r, byte g, byte b, int id) {
  uint32_t rgb;
  rgb = rgbWS.Color(r, g, b);        // Convert R, G, B components to color structure
  rgbWS.setPixelColor(id - 1, rgb);  // Set color for a given LED
  rgbWS.show();                      // Update color
}
