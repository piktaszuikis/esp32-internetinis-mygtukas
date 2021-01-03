ESP32 internetinis mygtukas
===========================

Paprasta programėlė, ESP32 mikrokontroleryje paleidžianti web serverį, kuriame galima įjungti arba išjungti kompiuterį. Plačiau apie projektą: https://piktas-zuikis.netlify.app/2021/01/03/ESP32-internetinis-mygtukas/

Konfigūracija
=============
Konfigūravimas vykdomas per `idf.py menuconfig`, ten yra sekcija "Internetinio mygtuko konfigūracija", kur galima nurodyti WiFi tinklą ir slaptažodį, pakeisti naudojamus GPIO.

Kompiliavimas
=============

Iš pradžių reikia sukonfigūruoti kompiliavimo aplinką: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html

Tada reikia paleisti tokias komandas:

```
idf.py set-target esp32
idf.py menuconfig
#Ten nustatyti WiFi SSID ir slaptažodį
idf.py build
idf.py flash
#Jei įdomu arba kyla problemų galima paskaityti logus:
idf.py monitor
```

Sėkmės!
