### Version 3.0
-Přidán pasivní piezo bzučák - pin GPIO15
-Přidán přijímač IR dálkového ovládání - pin GPIO26
-Přidána propojka pro přepnutí ventilátoru PM senzoru na 3,3V
-Změna konstrukce převodníku úrovní pro PM senzor
-Změna USB převodníku na CH9102F
-Změna 3,3V stabilizátoru na ME6211C33
-Změna hodnoty rezistoru R8 v převodníku logických úrovní pro LED WS2812
-Změna fototranzistoru na jiný typ - PT26-51B/TR8
#### Version 2.1
- Phototransistor pin changed from GPIO4 to GPIO33 (ADC1_CHANNEL_5) - cause ADC2 (GPIO4) is not available when using WiFi | Změna pinu fototranzistoru z GPIO4 na GPIO33 (ADC1_CHANNEL_5) - ADC2 (GPIO4) není dostupné při používání WiFi
#### Version 2.0
- Logic level converter for LEDs added | Přidán převodník logické úrovně pro LED