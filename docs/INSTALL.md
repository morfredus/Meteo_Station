# Latest release: **v2.6.0** — AQI provider switched to Open‑Meteo Air Quality API (free, no API key).

# Installation Guide

This document describes how to set up the project environment.

## Hardware Dependencies

- ESP32-S3 (e.g., ESP32-S3-DevKitC-1)
- ST7789 Display
- AHT20 Sensor (Temperature, Humidity)
- BMP280 Sensor (Pressure)
- GPS Module (e.g., Neo-6M)
- Buttons for user input
- Passive Buzzer

## Software Dependencies

This project is built using PlatformIO. The required libraries are automatically managed if you use the provided `platformio.ini` file.

Key libraries include:
- `bblanchon/ArduinoJson @ ^7.0.4`
- `esphome/ESPAsyncWebServer-esphome @ ^3.1.0`
- `adafruit/Adafruit GFX Library @ ^1.11.9`
- `adafruit/Adafruit ST7735 and ST7789 Library @ ^1.10.3`
- `adafruit/Adafruit AHTX0 @ ^2.0.5`
- `adafruit/Adafruit BMP280 Library @ ^2.6.8`
- `mikalhart/TinyGPSPlus @ ^1.1.0`
- `witnessmenow/UniversalTelegramBot @ ^1.3.0`