![LASKAKIT ESP-VINDRIKTNING](https://github.com/LaskaKit/ESP-Vindriktning/blob/main/img/01.jpg)
# LASKAKIT ESP-VINDRIKTNING

Ikea Vindriktning is very good and really cheap product which is able to show the quality of air in the room by three LEDs. The original Vindriktning just measure the dust. 
It is not so much for customer. 

We decided to create our own replaced board what will be compatible with the original board but add with Wi-Fi and Bluetooth connectivity, optionaly more sensors, and addressable LEDs. Thanks to this, you get powerful product what may measure the quality of air in the details. 

## Programming
The LASKAKIT ESP-VINDRIKTNING as well as Ikea Vindriktning includes USB-C connector. The orginal board is using the USB-C just for power supply, the ESP-VINDRIKTNING is using USB-C for power and also for programming, because the board includes built-in USB-UART IC as programmer. 

## Connectors
The LASKAKIT ESP-VINDRIKTNING contains six connectors. Two of them are used for original dust sensor - sensor and fan. The third connector is called uŠup and it is compatible with STEMMA QT and Qwiic JST-SH. Thanks to this connector, you can add [temperature/humidity sensor SHT40](https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/) or [SCD41 (sensor of CO2/temperature/humidity)](https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/) and improve the measurement of air. 
Fourth connector is just 4pin header and you can solder what you want. The 4pin connector includes I2C and power supply with 3.3V. 
The reset of the connectors contains power supply (3.3V) and GPIO which you can use for your purpose. Also the hardware SPI may be used from this connector. 

## LEDs
The number of LEDs is the same with original board. The LEDs are RGB and addressable, it means you can define the color on separately each LED. At the end, you can indicate dust on the first LED, CO2 on the second LED and temperature on third LED. 

## Example codes 
For the fastest development, we prepared a few examples where we show how to easily use LASKAKIT ESP-VINDRIKTNING alone or with SCD41

## Link
The product is available on our store https://www.laskakit.cz/laskakit-esp-vindriktning-esp-32-i2c/
Optional parts: 

SCD41 (CO2/temperature/humidity) https://www.laskakit.cz/laskakit-scd41-senzor-co2--teploty-a-vlhkosti-vzduchu/

SHT40 (temperature/humidity) https://www.laskakit.cz/laskakit-sht40-senzor-teploty-a-vlhkosti-vzduchu/
