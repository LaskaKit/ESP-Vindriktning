![LASKAKIT ESP-VINDRIKTNING](https://github.com/LaskaKit/ESP-Vindriktning/blob/main/img/01.jpg)
# LASKAKIT ESP-VINDRIKTNING

Ikea Vindriktning je zajímavý a levný produkt firmy Ikea, který dokáže měřit kvalitu vzduchu - prašnost ve vzduchu. A to je všechno.

Rozhodli jsme se vytvořit vlastní desku kterou jenom vyměníš - stačí odšroubovat a přišroubovat 7 šroubků. 
Naše deska LASKAKIT ESP-VINDRIKTNING má konektory na stejném místě jako originální deska, ale navíc obsahuje výkonný a populární čip ESP32, který kromě komunikace s čidlem prachu 
disponuje i možností připojení přes Wi-Fi a Bluetooth. Volitelně můžeš dokoupit i další čidla - SHT40 pro měření teploty a vlhkost nebo SCD41 pro měření CO2/teploty/vlhkosti. 

## Programování
LASKAKIT ESP-VINDRIKTNING stejně jako originální Ikea Vindriktning obsahuje USB-C konektor. U originální desky slouží pouze k napájení, u naší desky je kromě napájení možné ho použít i pro programování.
Stačí jí tedy připojit do USB konektoru a otevřít třeba Arduino IDE. 

## Konektory
LASKAKIT ESP-VINDRIKTNING obsahuje celkem šest konektorů. První dva slouží k připojení originální čidla - čidla prašnosti (čidlo a ventilátor). Třetí [konektor je μŠup](https://blog.laskarduino.cz/predstavujeme-univerzalni-konektor-pro-propojeni-modulu-a-cidel-%ce%bcsup/),
který je kompatibilní s STEMMA QT a Qwiic JST-SH
Díky tomuto konektoru můžeš přidat čidlo [tepoty/vlhkosti SHT40](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/) nebo [SCD41 (čidlo CO2/teploty/vlhkosti)](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/) 
a vytvořit tak komplexní měření kvality vzduchu.
Čtvrtým konektorem je prostý 4pinový hřebínek s I2C a napájením (3.3V)
Zbylé dva konektory obsahují GPIO a 3.3V napájení. GPIO je možné využít s hardwarovým SPI k připojení například displeje. 

## LEDky
Počet LEDek je stejný jako u původní desky. LEDky jsou ale RGB a adresovatelné, to znamená, že každá LEDka dokáže měnit barvu dle dat poslaných přes jednovodičovou sběrnici,
takže první LED může indikovat koncentraci CO2, druhá teplotu, třetí prašnost. 

## Vzorové kódy 
Pro rychlejší start s LASKAKIT ESP-VINDRIKTNING jsme napsali několik vzorových kódů, které můžeš jednoduše upravit pro vlastní potřeby.
Zároveň jsme rozsáhlý článek i na náš blog - https://blog.laskarduino.cz/senzor-prachovych-castic-ikea-vindriktning-vylepseny-o-cidlo-co2-teploty-vlhkosti-bluetooth-wi-fi-komunikaci-a-s-vizualizaci-dat-na-tmep-cz/
Použij desku "nodemcu-32s" v ESPHome a "ESP32 Dev Module" v Arduino IDE.

## Odkaz
Deska je dostupná na našem e-shopu https://www.laskakit.cz/laskakit-esp-vindriktning-esp-32-i2c/
Další volitelné části: 

SCD41 (CO2/teplota/vlhkost) https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/

SHT40 (teplota/vlhkost) https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/
