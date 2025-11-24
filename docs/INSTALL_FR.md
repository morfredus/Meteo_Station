# Dernière version : **v2.8.5** — Intervalle météo optimisé (15 min), support langue française et endpoint scan I2C.

# Guide d'Installation

Ce document décrit comment mettre en place l'environnement du projet.

## Dépendances Matérielles

- ESP32-S3 (ex: ESP32-S3-DevKitC-1)
- Écran ST7789
- Capteur AHT20 (Température, Humidité)
- Capteur BMP280 (Pression)
- Module GPS (ex: Neo-6M)
- Boutons pour l'interaction utilisateur
- Buzzer passif

## Dépendances Logicielles

Le projet est conçu avec PlatformIO. Les bibliothèques requises sont gérées automatiquement si vous utilisez le fichier `platformio.ini` fourni.

Les bibliothèques clés incluent :
- `bblanchon/ArduinoJson @ ^7.0.4`
- `esphome/ESPAsyncWebServer-esphome @ ^3.1.0`
- `adafruit/Adafruit GFX Library @ ^1.11.9`
- `adafruit/Adafruit ST7735 and ST7789 Library @ ^1.10.3`
- `adafruit/Adafruit AHTX0 @ ^2.0.5`
- `adafruit/Adafruit BMP280 Library @ ^2.6.8`
- `mikalhart/TinyGPSPlus @ ^1.1.0`
- `witnessmenow/UniversalTelegramBot @ ^1.3.0`