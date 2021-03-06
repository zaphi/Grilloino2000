# Grilloino2000

![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/im4.jpg "Optionaler Titel")

ESP8266 Grillthermometer

Als leidenschaftlicher Griller war es dies Jahr zeit für einen Smoker! Da kaufen nicht in frage kommt wurde der Bau eines UDS geplant. Und da ich in der Nacht gerne Schlafe und nicht alle halbe Stunde zum Smoker laufen möchte, musste man hier automatisieren.

Bauteile:
- ESP8266 NodeMCU
- 4051N Analogmultiplexer
- 16x2 LCD
- kleinkram... siehe BOM.txt

![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/im33.jpg "Optionaler Titel")
![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/im7.jpg "Optionaler Titel")

Funktionen:
- 1x Garraum
- 1-3x Kerntemperatur
- WIFI
- Handy App mittels Blynk
- Thingspeak Logging
- Pitmaster (Lüfter/Klappe)
- Push Nachrichten
- HTTP Server (wird vermutlich entfernt)

![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/im22_small.jpg "BLYNK")

Aktueller Projektstand:
- PCB V3.1 incl. Buttons 
- 3D Druck Gehäuse

![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/im5.jpg "BLYNK")


ToDo:
- korrektur Polung DC Stecker in PCB V3.2


HowTo:
- Es können theoretisch  alle Fühler verwendet werden die auch beim [WLANThermo V2](https://github.com/WLANThermo/WLANThermo_v2/tree/master/software/usr/share/doc/WLANThermo/probedata) funktionieren. Dort wird auch mittels der Steinhart-Hart-Gleichung gerechnet somit können die Parameter A,B,C und Rn übernommen werden. Danke für das vermessen der  Sensoren :-)

Aktuell eingepflegt:
- Maverick ET-7/71/72/73/77/901b
- Maverick ET-735
- Maverick ET-732/733
- Ikkea Fantast
- Ikkea Fantast Neu

Einstellungen für Blynk:
- char auth[] = "PUT YOUR CODE HEAR";

Einstellungen Thingspeak
- const int channelID = Your ID;
- String writeAPIKey = "Your Key";

![Grilloino](https://github.com/zaphi/Grilloino2000/blob/master/img/ts.JPG "BLYNK")

Messen und Korrektur V3.3:
- Spannung an JP1 messen und im Code eintragen z.B. 3.293V
- float vin = 3.293;  

Button S1:
- Startet WiFi Konfigurations Mode. Grilloino wird als AP im Handy sichtbar und kann mit jedem beliebigen
Hotspot verubnden werden.

Button S2:
- Doppelklick -> Menü Seite -
- Einfach Klick -> Wert +1
- Langer Klick -> Wert ++

Button S3:
- Doppelklick -> Menü Seite +
- Einfach Klick -> Wert -1
- Langer Klick -> Wert --
- Langer Klick nicht im Menü -> anzeige Spannung, IP und RSSI


Bauteile:
- R1-R4 47kOhm Messwiderstand
- R5 560 Ohm
- R9,R10,R11,R13,R14 10kOhm
- R12 47 kOhm

- C1 0,33uf
- C2 0,1uf
- C3,C4,C5,C6 1000 uf
- IC2 LM7805
- Q1 STP36NF06L
- IC1 4051N


Benötigte libraries:

- [Wifi Manager](https://github.com/tzapu/WiFiManager)
- [OneButton](https://github.com/mathertel/OneButton)
- [Timer](https://github.com/JChristensen/Timer)
- [LiquidCrystal-I2C-library](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library)
- [Blynk](https://github.com/blynkkk/blynk-library/releases/tag/v0.5.4)
- [PID-Library](https://github.com/br3ttb/Arduino-PID-Library)



