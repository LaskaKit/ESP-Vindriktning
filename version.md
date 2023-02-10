### Version 3.0
- Added, buzzer - GPIO2 pin | Přidán pasivní piezo bzučák - pin GPIO2
- Added, IR remote control receiver - GPIO26 pin | Přidán přijímač IR dálkového ovládání - pin GPIO26
- Added, a jumper to change voltage for fan to 3.3V | Přidána propojka pro přepnutí ventilátoru PM senzoru na 3,3V
- Changed the design of the level converter for the PM sensor | Změna konstrukce převodníku úrovní pro PM senzor
- Changed the USB converter to CH9102F | Změna USB převodníku na CH9102F
- Changed the value of resistor R8 in the logic level converter for the WS2812 LED | Změna hodnoty rezistoru R8 v převodníku logických úrovní pro LED WS2812
- Changed the phototransistor to another type - PT26-51B/TR8 | Změna fototranzistoru na jiný typ - PT26-51B/TR8
- Changed  LDO - BL8071CLBTR33 | Změna LDO stabilizátoru - BL8071CLBTR33
#### Version 2.1
- Phototransistor pin changed from GPIO4 to GPIO33 (ADC1_CHANNEL_5) - cause ADC2 (GPIO4) is not available when using WiFi | Změna pinu fototranzistoru z GPIO4 na GPIO33 (ADC1_CHANNEL_5) - ADC2 (GPIO4) není dostupné při používání WiFi
#### Version 2.0
- Logic level converter for LEDs added | Přidán převodník logické úrovně pro LED