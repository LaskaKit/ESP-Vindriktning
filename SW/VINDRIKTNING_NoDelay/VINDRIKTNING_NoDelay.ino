// LaskaKit ESP-VINDRIKTNING v2.0 demo software

/*
  This is slightly more sophisticated version of the basic example. Instead of
  delay() everywhere it uses asynchronous approach. This is better for 
  extensibility, because in the meantime some other code can run, ie. HTTP
  server etc.

  The `next_pm_event` and `next_al_event` variables hold the information about
  when something is supposed to happen next time. 

  The measurement itself is multi-step (turn fan off and on before and after)
  measuring) so additional variable `pm_status` holds current step info.
*/

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

// Action intervals
#define INTERVAL_AL_MEASURE 10000  // 10 s
#define INTERVAL_PM_MEASURE 30000  // 30 s
#define FAN_TIME_BEFORE_PM 10000   // 10 s
#define FAN_TIME_AFTER_PM 1000     // 1 s

// PM measure status
#define PM_STATUS_WAITING 0
#define PM_STATUS_FAN_BEFORE 1
#define PM_STATUS_FAN_AFTER 2

// PM 2.5 sensor
static PM1006 pm1006(&Serial2);
int pm_status = PM_STATUS_WAITING;
uint16_t pm2_5; // Last known value

// Ambient light sensor
int al; // Last known value

// RGB LEDs
Adafruit_NeoPixel rgbWS = Adafruit_NeoPixel(3, PIN_LED, NEO_GRB + NEO_KHZ800);

// Time points for next events
unsigned long next_pm_event = 0;
unsigned long next_al_event = 0;

void setup() {
  // Enable serial communication
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(500);
  Serial.println("Initializing");

  // Setup pins
  pinMode(PIN_FAN, OUTPUT);           // Fan
  pinMode(PIN_AMBIENT_LIGHT, INPUT);  // Ambient light

  // Setup WS2718 LEDs
  rgbWS.begin();
  rgbWS.setBrightness(BRIGHTNESS_DAY);
  setColorWS(255, 0, 0, 1);
  setColorWS(0, 255, 0, 2);
  setColorWS(0, 0, 255, 3);
}

void loop() {
  // Read ambient light
  if (millis() >= next_al_event) process_al_event();

  // Read dust particles
  if (millis() >= next_pm_event) process_pm_event();
}

void process_al_event() {
  // Read ambient light level
  al = analogRead(PIN_AMBIENT_LIGHT);
  Serial.print("Ambient light: ");
  Serial.println(al);

  // Set LED brightness    
  if (al >= DAY_MIN_AL) {
    rgbWS.setBrightness(BRIGHTNESS_NIGHT);
  } else {
    rgbWS.setBrightness(BRIGHTNESS_DAY);
  }

  // Schedule next event
  next_al_event = millis() + INTERVAL_AL_MEASURE;
}

void process_pm_event() {
  if (pm_status == PM_STATUS_WAITING) {
    // Turn fan ON
    digitalWrite(PIN_FAN, HIGH);
    Serial.println("Fan ON");

    // Update PM status
    pm_status = PM_STATUS_FAN_BEFORE;
    next_pm_event = millis() + FAN_TIME_BEFORE_PM;
  } else if (pm_status == PM_STATUS_FAN_BEFORE) {
    // Perform measuring
    if (pm1006.read_pm25(&pm2_5)) {
      printf("PM2.5 = %u\n", pm2_5);
    } else {
      Serial.println("Measurement failed!");
      reboot();
    }

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

    // Update PM status
    pm_status = PM_STATUS_FAN_AFTER;
    next_pm_event = millis() + FAN_TIME_AFTER_PM;
  } else if (pm_status == PM_STATUS_FAN_AFTER) {
    // Turn fan off
    digitalWrite(PIN_FAN, LOW);
    Serial.println("Fan OFF");

    // Update PM status
    pm_status = PM_STATUS_WAITING;
    next_pm_event = millis() + INTERVAL_PM_MEASURE;
  }
}

void reboot() {
  Serial.println("Rebooting ESP...");

  // Blink red 10 times
  for (int i = 0; i < 10; i++) {
    rgbWS.setBrightness(255);
    setColorWS(255, 0, 0, 1);
    setColorWS(255, 0, 0, 2);
    setColorWS(255, 0, 0, 3);
    delay(200);
    setColorWS(0, 0, 0, 1);
    setColorWS(0, 0, 0, 2);
    setColorWS(0, 0, 0, 3);
    delay(200);
  }

  // Reboot ESP
  ESP.restart();
}

void setColorWS(byte r, byte g, byte b, int id) {
  uint32_t rgb;
  rgb = rgbWS.Color(r, g, b);        // Convert R, G, B components to color structure
  rgbWS.setPixelColor(id - 1, rgb);  // Set color for a given LED
  rgbWS.show();                      // Update color
}
