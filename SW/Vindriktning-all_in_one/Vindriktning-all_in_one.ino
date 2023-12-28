/*
* Vzorovy kod od laskakit.cz pro LaskaKit ESP-VINDRIKTNING
* Kod podporuje cidla: SCD41 (CO2, teplota a vlhkost) nebo SHT40 (teplota a vlhkost)
* Kod podporuje posilani dat na: server TMEP.cz nebo seriovy port (UART)
*
* TMEP.cz momentalne neumi zobrazovat ctyri veliciny najednou - prasnost, CO2, teplota, vlhkost. 
* Proto jsou zaregistrovana dve cidla zvlast - prasnost (cidlo PM1006) a CO2/teplota/vlhkost (cidlo SCD41)
* Pokud pouzijete cidlo SHT40 bude vam stacit pouze jedno registrovane cidlo
*
* LaskaKit ESP-VINDRIKTNING (https://www.laskakit.cz/laskakit-esp-vindriktning-esp-32-i2c/)
* LaskaKit SCD41 Senzor CO2, teploty a vlhkosti vzduchu (https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/)
* LaskaKit SHT40 Senzor teploty a vlhkosti vzduchu (https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/)
*
* Vytvoreno (c) laskakit.cz 2022
* Upravil @cqeta1564
*
* Potrebne knihovny:
* https://github.com/adafruit/Adafruit_SHT4X //SHT40
* https://github.com/sparkfun/SparkFun_SCD4x_Arduino_Library //SCD41
* https://github.com/bertrik/pm1006 //PM1006
*/

/*--------------------- UPRAV NASTAVENI ---------------------*/
const char* ssid = "SSID";
const char* password = "PASSWORD";

// pokud NEpouzivas cidlo SCD41, tak vypln pouze prvni adresu - teplota, vlhkost a prasnost
String hlavni = "http://HLAVNI_DOMENA.tmep.cz/index.php?";

// pokud pouzivas cidlo SCD41, tak vypln i tuto adresu
// hodnoty budou rozdeleny nasledovne: prvni adresa - teplota, vlhkost a co2
//                                     druha adresa - prasnost
String sekundarni = "http://SEKUNDARNI-DOMENA.tmep.cz/index.php?";

// Interval mereni (zadavano v milisekundach)
#define SLEEP_SEC 300000

// zapnout seriovy port (UART)
#define uart

// zapnout server TMEP.cz
//#define tmep

// zapnout SCD41
//#define scd41 

// zapnout SHT40
//#define sht40

// zapnout OLED displej
//#define oled
/*------------------------ KONEC UPRAV -----------------------*/

#include <Wire.h>

/* TMEP.cz */
#ifdef tmep
#include <WiFi.h>
#include <HTTPClient.h>
#endif

/* SHT40 */
#ifdef sht40
#include "Adafruit_SHT4x.h"
#endif

/* OLED Dislpej */
#ifdef oled
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#endif

/* LaskaKit ESP-VINDRIKTNING s čidlem CO2/teploty/vlhkosti SCD41 */
#ifdef scd41
#include "SparkFun_SCD4x_Arduino_Library.h"
#endif

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006 */
#include "pm1006.h"

/* RGB adresovatelne LED */
#include <Adafruit_NeoPixel.h>

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006 */
#define PIN_FAN 12 // spinani ventilatoru
#define RXD2 16 // UART - RX
#define TXD2 17 // UART - TX

/* Nastaveni RGB LED */
#define BRIGHTNESS 100
#define PIN_LED 25
#define PM_LED 0
#define TEMP_LED 1
#define CO2_LED 2

/* OLED nastaveni */
#ifdef oled
#define SCREEN_WIDTH  128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const unsigned char laskakit [] PROGMEM = {
	0x01, 0xf0, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x07, 0xfc, 0x00, 0x3f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0x0f, 0xfe, 0x00, 0x7f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0x1e, 0x0f, 0x00, 0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0, 
	0x3c, 0x07, 0x80, 0xe0, 0x78, 0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 
	0x78, 0x03, 0xc0, 0x00, 0x3c, 0x00, 0x38, 0x0f, 0xc0, 0x03, 0x80, 0x00, 0x03, 0x80, 0x03, 0x00, 
	0x70, 0x01, 0xe0, 0x00, 0x1c, 0x00, 0x38, 0x00, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x00, 
	0xe0, 0x00, 0xf0, 0x00, 0x0e, 0x00, 0x38, 0x00, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x30, 
	0xe0, 0x00, 0x78, 0x00, 0x0e, 0x00, 0x38, 0x00, 0x00, 0x03, 0x80, 0x00, 0x03, 0x80, 0x00, 0x30, 
	0xe0, 0x00, 0x3c, 0x00, 0x0e, 0x00, 0x38, 0xfe, 0x0f, 0xe3, 0x8f, 0x1f, 0xc3, 0x8f, 0x1c, 0x7c, 
	0xe0, 0x00, 0x1e, 0x00, 0x0e, 0x00, 0x38, 0xff, 0x1f, 0xe3, 0x9e, 0x1f, 0xe3, 0x9e, 0x1c, 0x7c, 
	0x70, 0x00, 0x0f, 0x00, 0x1c, 0x00, 0x38, 0x03, 0x9c, 0x03, 0xbc, 0x00, 0xe3, 0xbc, 0x1c, 0x30, 
	0x78, 0x00, 0x07, 0x80, 0x3c, 0x00, 0x38, 0x03, 0x9e, 0x03, 0xf8, 0x00, 0xe3, 0xf8, 0x1c, 0x30, 
	0x3c, 0x00, 0x03, 0xc0, 0x78, 0x00, 0x38, 0xff, 0x8f, 0xe3, 0xf8, 0x1f, 0xe3, 0xf8, 0x1c, 0x30, 
	0x1e, 0x00, 0x01, 0xe0, 0xf0, 0x00, 0x39, 0xc3, 0x80, 0xf3, 0xfc, 0x38, 0xe3, 0xfc, 0x1c, 0x30, 
	0x0f, 0x00, 0x00, 0xff, 0xe0, 0x00, 0x39, 0xc3, 0x80, 0x73, 0x9e, 0x38, 0xe3, 0x9e, 0x1c, 0x30, 
	0x07, 0x80, 0x00, 0x7f, 0xc0, 0x00, 0x39, 0xff, 0x9f, 0xe3, 0x8f, 0x3f, 0xe3, 0x8f, 0x1c, 0x3e, 
	0x03, 0xc0, 0x00, 0x1f, 0x00, 0x00, 0x38, 0xff, 0x9f, 0xc3, 0x87, 0x1f, 0xe3, 0x87, 0x1c, 0x1e, 
	0x01, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x3c, 0x00, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1e, 0x00, 0x78, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0f, 0x00, 0xf0, 0x00, 0x00, 0x20, 0x00, 0x02, 0x00, 0x02, 0x00, 0x00, 0x08, 0x00, 0x00, 
	0x00, 0x07, 0x81, 0xe0, 0x00, 0x00, 0x3a, 0x9f, 0xba, 0xbb, 0x73, 0x66, 0x7e, 0xea, 0xed, 0xc0, 
	0x00, 0x03, 0xc3, 0xc0, 0x00, 0x00, 0x2a, 0x92, 0x8b, 0x2a, 0x62, 0x94, 0x4a, 0x2c, 0xa9, 0x80, 
	0x00, 0x01, 0xe7, 0x80, 0x00, 0x00, 0x29, 0x12, 0xaa, 0xa2, 0x12, 0x94, 0x4a, 0xaa, 0x88, 0x40, 
	0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x39, 0x12, 0xb2, 0xba, 0x72, 0x64, 0x4a, 0xca, 0xe9, 0xc0, 
	0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

/* LaskaKit ESP-VINDRIKTNING - cidlo prasnosti PM1006, nastaveni UART2 */
static PM1006 pm1006(&Serial2);

/* LaskaKit ESP-VINDRIKTNING s čidlem CO2/teploty/vlhkosti SCD41 */
#ifdef scd41
SCD4x SCD41;
#endif

/* LaskaKit ESP-VINDRIKTNING s čidlem teploty/vlhkosti SHT40 */
#ifdef sht40
Adafruit_SHT4x sht4 = Adafruit_SHT4x();
#endif

/* RGB adresovatelne LED */
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, PIN_LED, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(PIN_FAN, OUTPUT); // Ventilator pro cidlo prasnosti PM1006
  Serial.begin(115200);
  Wire.begin();
  
  pixels.begin(); // WS2718
  pixels.setBrightness(BRIGHTNESS);

  delay(10);
  
  /*-------------- RGB adresovatelne LED - zhasni --------------*/
  pixels.setPixelColor(PM_LED, pixels.Color(0, 0, 0)); // R, G, B
  pixels.setPixelColor(TEMP_LED, pixels.Color(0, 0, 0)); // R, G, B
  pixels.setPixelColor(CO2_LED, pixels.Color(0, 0, 0)); // R, G, B
  pixels.show();  // Zaktualizuje barvu

  /*-------------- PM1006 - cidlo prasnosti ---------------*/
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // cidlo prasnosti PM1006

  /*-------------------- OLED Displej --------------------*/
  #ifdef oled
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.drawBitmap(0, 0, laskakit, 128, 32, WHITE);
  display.display();
  #endif

  /*------------- SCD41 - CO2, teplota, vlhkost -----------*/
  #ifdef scd41
  // inicializace
  //             begin, autokalibrace
  //               |      |
  if (SCD41.begin(false, true) == false)
  {
    Serial.println("SCD41 nenalezen.");
    Serial.println("Zkontroluj propojeni.");
    while(1)
      ;
  }
 
  // prepnuti do low power modu
  if (SCD41.startLowPowerPeriodicMeasurement() == true)  
  {
    Serial.println("Low power mod povolen.");
  }
  #endif

  /*------------- SHT40 - teplota, vlhkost -----------*/
  #ifdef sht40
  if (! sht4.begin()) 
  {
    Serial.println("SHT4x not found");
    Serial.println("Check the connection");
    while (1) delay(1);
  }

  sht4.setPrecision(SHT4X_HIGH_PRECISION); // highest resolution
  sht4.setHeater(SHT4X_NO_HEATER); // no heater
  #endif

  /*------------- Wi-Fi -----------*/
  #ifdef tmep
  WiFi.begin(ssid, password);
  Serial.println("Pripojovani");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Pripojeno do site, IP adresa zarizeni: ");
  Serial.println(WiFi.localIP());
  #endif
}
 
void loop() {
  /*------------- SCD41 - CO2, teplota, vlhkost -----------*/
  int co2 = -1;
  float teplota = 9999.0;
  int vlhkost = -1;
  #ifdef scd41 
  while (!SCD41.readMeasurement()) // cekani na nova data (zhruba 30s)
  {
    delay(1);
  } 

  co2 = SCD41.getCO2();
  teplota = SCD41.getTemperature();
  vlhkost = SCD41.getHumidity();
  #endif

  // odeslani hodnot pres UART
  #ifdef uart
  Serial.print("Teplota: "); Serial.print(teplota); Serial.println(" degC");
  Serial.print("Vlhkost: "); Serial.print(vlhkost); Serial.println("% rH");
  Serial.print("CO2: "); Serial.print(co2); Serial.println(" ppm");
  #endif


  /*------------- SHT40 - teplota, vlhkost -----------*/
  #ifdef sht40
  sensors_event_t humidity, temp; // temperature and humidity variables
  sht4.getEvent(&humidity, &temp);

  teplota = temp.temperature;
  vlhkost = humidity.relative_humidity;

  // odeslani hodnot pres UART
  #ifdef uart
  Serial.print("Teplota: ");
  Serial.print(teplota);
  Serial.println(" degC");
  Serial.print("Vlhkost: ");
  Serial.print(vlhkost);
  #endif
  #endif


  /*-------------- PM1006 - cidlo prasnosti ---------------*/
  uint16_t pm2_5;
  digitalWrite(PIN_FAN, HIGH);
  Serial.println("Fan ON");
  delay(30000);

  if (pm1006.read_pm25(&pm2_5)){
    Serial.println("Mam data z cidla");
  } else {
    Serial.println("Data z cisla nesla vycist");
  }


  delay(100);
  digitalWrite(PIN_FAN, LOW);
  Serial.println("Fan OFF");

  // odeslani hodnot pres UART
  #ifdef uart
  Serial.print("PM2.5: "); Serial.print(pm2_5); Serial.println(" ppm");
  #endif
  
  
  /*-------------- RGB adresovatelne LED ---------------*/
  // CO2 LED
  if(co2 < 0){
    pixels.setPixelColor(CO2_LED, pixels.Color(0, 0, 0)); // R, G, B
  }

  if((co2 >= 0) && (co2 < 1000)){
    pixels.setPixelColor(CO2_LED, pixels.Color(0, 255, 0)); // R, G, B
  }
  
  if((co2 >= 1000) && (co2 < 1200)){
    pixels.setPixelColor(CO2_LED, pixels.Color(128, 255, 0)); // R, G, B
  }
  
  if((co2 >= 1200) && (co2 < 1500)){
  pixels.setPixelColor(CO2_LED, pixels.Color(255, 255, 0)); // R, G, B
  }
  
  if((co2 >= 1500) && (co2 < 2000)){
    pixels.setPixelColor(CO2_LED, pixels.Color(255, 128, 0)); // R, G, B
  }
  
  if(co2 >= 2000){
    pixels.setPixelColor(CO2_LED, pixels.Color(255, 0, 0)); // R, G, B
  }

  // teplota LED
  if(teplota < 20.0){
    pixels.setPixelColor(TEMP_LED, pixels.Color(0, 0, 255)); // R, G, B
  }

  if((teplota >= 20.0) && (teplota < 23.0)){
    pixels.setPixelColor(TEMP_LED, pixels.Color(0, 255, 0)); // R, G, B
  }

  if((teplota >= 23.0) && (teplota < 9999)){
    pixels.setPixelColor(TEMP_LED, pixels.Color(255, 0, 0)); // R, G, B
  }

  if(teplota >= 9999){
    pixels.setPixelColor(TEMP_LED, pixels.Color(0, 0, 0)); // R, G, B
  }

  // PM LED
  if(!pm1006.read_pm25(&pm2_5)){
    pixels.setPixelColor(PM_LED, pixels.Color(0, 0, 0)); // R, G, B
  }

  if((pm2_5 >= 0) && (pm2_5 < 30)){
    pixels.setPixelColor(PM_LED, pixels.Color(0, 255, 0)); // R, G, B
  }
  
  if((pm2_5 >= 30) && (pm2_5 < 40)){
    pixels.setPixelColor(PM_LED, pixels.Color(128, 255, 0)); // R, G, B
  }
  
  if((pm2_5 >= 40) && (pm2_5 < 80)){
  pixels.setPixelColor(PM_LED, pixels.Color(255, 255, 0)); // R, G, B
  }
  
  if((pm2_5 >= 80) && (pm2_5 < 90)){
    pixels.setPixelColor(PM_LED, pixels.Color(255, 128, 0)); // R, G, B
  }
  
  if(pm2_5 >= 90){
    pixels.setPixelColor(PM_LED, pixels.Color(255, 0, 0)); // R, G, B
  }
  pixels.show();  // Zaktualizuje barvu


  /*------------- OLED Display -------------*/
  #ifdef oled
  display.clearDisplay();
  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  display.setCursor(5,8);             // Start at top-left corner
  display.println(String(pm2_5) + "mg/m3");

  #ifdef sht40
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  display.setCursor(69,5);             // Start at top-left corner
  display.println(String(teplota) + "degC");  
  display.setCursor(85,21);             // Start at top-left corner
  display.println(String(vlhkost) + "%");
  display.setCursor(5,21);             // Start at top-left corner
  display.println(String(pm2_5) + "mg/m3");
  #endif

  #ifdef scd41
  display.clearDisplay();
  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text

  display.setCursor(5,5);             // Start at top-left corner
  display.println(String(co2) + "ppm");
  display.setCursor(69,5);             // Start at top-left corner
  display.println(String(teplota) + "degC");  
  display.setCursor(85,21);             // Start at top-left corner
  display.println(String(vlhkost) + "%");
  display.setCursor(5,21);             // Start at top-left corner
  display.println(String(pm2_5) + "mg/m3");
  #endif
  
  

  display.display();  
  delay(1);
  #endif
  

  /*------------ Odeslani hodnot na TMEP.cz ------------------*/
  #ifdef tmep
  #ifdef sht40
  if(WiFi.status()== WL_CONNECTED)
  {
    //GUID, nasleduje hodnota teploty, pro vlhkost "humV", pro CO2 "CO2" cidla SCD41
    String serverPathCO2 = hlavni + "" + "temp" + "=" + teplota + "&humV=" + vlhkost + "&pm=" + pm2_5; 
    sendhttpGet(serverPathCO2);

  }
  else 
  {
    Serial.println("Wi-Fi odpojeno");
  }
  #endif
  #ifdef scd41
  if(WiFi.status()== WL_CONNECTED)
  {
    //GUID, nasleduje hodnota teploty, pro vlhkost "humV", pro CO2 "CO2" cidla SCD41
    String serverPathCO2 = sekundarni + "" + "temp=" + teplota + "&humV=" + vlhkost + "&CO2=" + co2; 
    sendhttpGet(serverPathCO2);

    delay(100);
    int pmOdeslat = pm2_5;
    //GUID, nasleduje hodnota cidla prasnosti PM1006 a odeslani na druhou domenu
    String serverPathPM = hlavni + "" + "pm=" + pmOdeslat; 
    sendhttpGet(serverPathPM);

  }
  else 
  {
    Serial.println("Wi-Fi odpojeno");
  }
  #endif
  #endif

  delay(100);
  delay(SLEEP_SEC);
}

// funcke pro odeslani dat na TMEP.cz
#ifdef tmep
void sendhttpGet(String httpGet)
{
  HTTPClient http;
      
  // odeslani dat 
  String serverPath = httpGet; 
  
  // zacatek http spojeni
  http.begin(serverPath.c_str());
  
  // http get request
  int httpResponseCode = http.GET();
  
  if (httpResponseCode>0) 
  {
    Serial.print("HTTP odpoved: ");
    Serial.println(httpResponseCode);
    String payload = http.getString();
    Serial.println(payload);
  }
  else 
  {
    Serial.print("Error kod: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();
}
#endif
