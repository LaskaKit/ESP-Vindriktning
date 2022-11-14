/*
* Vzorovy kod od TMEP.cz zalozeny na laskakit.cz ESP-VINDRIKTNING
* Misto SCD40 je LaskaKit SHT40, takze teplota a vlhkost bez CO2.
*
* Kod posle pres seriovy port (UART) a zaroven na server TMEP.cz
*
* Vytvoreno (c) laskakit.cz 2022, mirna modifikace na "jen" teplotu, 
* vlhkost (a pranost) - MultiTricker TMEP.cz
*
* V Arduino IDE jsem po doinstalovani ESP32 desek pouzil board: ESP32 Dev Module
*
* Potrebne knihovny - na pranost:
* https://github.com/bertrik/pm1006 //PM1006
* 
* Na LaskaKit SHT40 staci v Arduino IDE Library manageru vyhledat a 
* nainstalovat vc. zavislosti "Adafruit SHT4x"
* https://github.com/adafruit/Adafruit_SHT4X
* 
* Na TMEPu zvol typ cidla "Teplota, vlhkost a CO2", treti promennou (CO2) 
* na zalozce "Zasilane hodnoty" pojmenuj jako "Prasnost" (vc. diakritiky ;))
* 
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "Adafruit_SHT4x.h"

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006 */
#include "pm1006.h"

/* RGB adresovatelne LED */
#include <Adafruit_NeoPixel.h>

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006 */
#define PIN_FAN 12  // spinani ventilatoru
#define RXD2 16     // UART - RX
#define TXD2 17     // UART - TX

/* Nastaveni RGB LED */
#define BRIGHTNESS 5
#define PIN_LED 25
#define PM_LED 0
#define TEMP_LED 1
#define HUM_LED 2

/*--------------------- UPRAV NASTAVENI ---------------------*/
const char* ssid = "SSID";
const char* password = "PASSWORD";

// Vypln domenu pro zapis hodnot, kterou najdes u vytvoreneho cidla na tmep.cz
// Nemas? registrace je za minutu a cidlo pridas okamzite ;)
String serverName = "http://TVOJE_DOMENA_PRO_ZAPIS.tmep.cz/index.php?";

// Muzes vyplnit vlastni pojmenovani GUID, pokud si je na TMEPu nastavis na jine nez vychozi
String GUID_TEPLOTA = "temp";
String GUID_VLHKOST = "humi";
String GUID_PRASNOST = "CO2";
/*------------------------ KONEC UPRAV -----------------------*/

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006, nastaveni UART2 */
static PM1006 pm1006(&Serial2);

/* LaskaKit ESP-VINDRIKTNING s čidlem LaskaKit SHT40 */
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

/* RGB adresovatelne LED */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, PIN_LED, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(PIN_FAN, OUTPUT);  // Ventilator pro cidlo prasnosti PM1006
  Serial.begin(115200);
  Serial.println("BOOTujeme");
  
  if (! sht4.begin()) 
  {
    Serial.println("SHT4x not found");
    Serial.println("Check the connection");
    while (1) delay(1);
  }

  sht4.setPrecision(SHT4X_HIGH_PRECISION); // highest resolution
  sht4.setHeater(SHT4X_NO_HEATER); // no heater
  
  Wire.begin();

  pixels.begin();  // WS2718
  pixels.setBrightness(BRIGHTNESS);

  delay(10);

  /*-------------- RGB adresovatelne LED - zhasni --------------*/
  pixels.setPixelColor(PM_LED, pixels.Color(0, 0, 0));    // R, G, B
  pixels.setPixelColor(TEMP_LED, pixels.Color(0, 0, 0));  // R, G, B
  pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 0));   // R, G, B
  pixels.show();                                          // Zaktualizuje barvu

  /*-------------- PM1006 - cidlo prasnosti ---------------*/
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // cidlo prasnosti PM1006

  /*------------- Wi-Fi -----------*/
  WiFi.begin(ssid, password);
  Serial.println("Pripojovani");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Pripojeno do site, IP adresa zarizeni: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  /*------------- LaskaKit SHT40 - teplota, vlhkost -----------*/
  float teplota = 0.0;
  int vlhkost = 0;

  sensors_event_t humidity, temp; // temperature and humidity variables
  sht4.getEvent(&humidity, &temp);

  teplota = temp.temperature;
  vlhkost = humidity.relative_humidity;

  // odeslani hodnot pres UART
  Serial.print("Teplota: ");
  Serial.print(teplota);
  Serial.println(" degC");
  Serial.print("Vlhkost: ");
  Serial.print(vlhkost);

  /*-------------- PM1006 - cidlo prasnosti ---------------*/
  uint16_t pm2_5;
  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(30000);

  while (!pm1006.read_pm25(&pm2_5)) {
    delay(1);
  }

  delay(100);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");

  // odeslani hodnot pres UART
  Serial.print("PM2.5: ");
  Serial.print(pm2_5);
  Serial.println(" ppm");

  /*-------------- RGB adresovatelne LED ---------------*/

  // teplota LED
  if(teplota < 20.0){
    pixels.setPixelColor(TEMP_LED, pixels.Color(0, 0, 255)); // R, G, B
  } else if (teplota < 23.0){
    pixels.setPixelColor(TEMP_LED, pixels.Color(0, 255, 0)); // R, G, B
  } else {
    pixels.setPixelColor(TEMP_LED, pixels.Color(255, 0, 0)); // R, G, B
  }

  // vlhkost LED
  if (vlhkost < 20.0) {
    pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 50));  // R, G, B
  } else if (vlhkost < 40.0) {
    pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 100));  // R, G, B
  } else if (vlhkost < 60.0) {
    pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 150));  // R, G, B
  } else if (vlhkost < 80.0) {
    pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 200));  // R, G, B
  } else {
    pixels.setPixelColor(HUM_LED, pixels.Color(0, 0, 255));  // R, G, B
  }

  // PM LED
  if (pm2_5 < 30) {
    pixels.setPixelColor(PM_LED, pixels.Color(0, 255, 0));  // R, G, B
  } else if (pm2_5 < 40) {
    pixels.setPixelColor(PM_LED, pixels.Color(128, 255, 0));  // R, G, B
  } else if (pm2_5 < 80) {
    pixels.setPixelColor(PM_LED, pixels.Color(255, 255, 0));  // R, G, B
  } else if (pm2_5 < 90) {
    pixels.setPixelColor(PM_LED, pixels.Color(255, 128, 0));  // R, G, B
  } else {
    pixels.setPixelColor(PM_LED, pixels.Color(255, 0, 0));  // R, G, B
  }
  pixels.show();  // Zaktualizuje barvu

  /*------------ Odeslani hodnot na TMEP.cz ------------------*/
  if (WiFi.status() == WL_CONNECTED) {
    // GUID, nasleduje hodnota teploty, vlhkosti a pranosti
    String serverPath = serverName + "" + GUID_TEPLOTA + "=" + teplota + "&" + GUID_VLHKOST + "=" + vlhkost + "&" + GUID_PRASNOST + "=" + pm2_5;
    sendhttpGet(serverPath);

  } else {
    Serial.println("Wi-Fi odpojeno");
  }

  esp_sleep_enable_timer_wakeup(900 * 1000000);  // uspani na 15 minut
  Serial2.flush();
  Serial.flush();
  delay(100);
  esp_deep_sleep_start();
}

// funcke pro odeslani dat na TMEP.cz
void sendhttpGet(String httpGet) {
  HTTPClient http;

  // odeslani dat
  String serverPath = httpGet;

  // zacatek http spojeni
  http.begin(serverPath.c_str());

  // http get request
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.print("HTTP odpoved: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.print("Error kod: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}
