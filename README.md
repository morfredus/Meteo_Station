# ESP32-S3 Weather Station Pro

A sophisticated, autonomous weather station built with **ESP32-S3**, featuring GPS geolocation, real-time environmental sensing, Telegram control, and a modern TFT interface.

![Version](https://img.shields.io/badge/version-1.2.0-blue.svg) ![License](https://img.shields.io/badge/license-MIT-green.svg)

## 🌟 Features
* **Real-time Sensing:** Temperature, Humidity, Pressure, Light level (AHT20 + BMP280 + LDR).
* **GPS Tracking:** Automatic geolocation and time synchronization (GT-U7).
* **Smart Weather:** Fetches local weather & 3-day forecast via Open-Meteo API based on GPS position.
* **Rich UI:** 1.54" TFT Display with 6 interactive pages (Summary, Details, Alerts, GPS, Network, Forecast).
* **Connectivity:** WiFi Multi-AP support & Web Server with JSON API.
* **Remote Control:** Full control and status reports via **Telegram Bot**.
* **Smart Alerts:** Visual (NeoPixel) and Audio (Buzzer) alerts for extreme conditions.

## 🛠 Hardware
* **MCU:** ESP32-S3 DevKitC-1 (N16R8)
* **Display:** 1.54" ST7789 SPI TFT (240x240)
* **Sensors:** AHT20 (Temp/Hum), BMP280 (Pressure/Alt), LDR (Light)
* **GPS:** GT-U7 Module
* **Misc:** Buzzer, NeoPixel (Built-in), 2x Push Buttons.

## 🚀 Quick Start
1.  Wire your hardware (see [INSTALL.md](INSTALL.md)).
2.  Configure your WiFi and Keys (see [CONFIG.md](CONFIG.md)).
3.  Build and Upload using PlatformIO.

## 📂 Documentation
* [Installation Guide](INSTALL.md)
* [Configuration](CONFIG.md)
* [API Reference](API_REFERENCE.md)
* [Architecture](ARCHITECTURE.md)
* [Contributing](CONTRIBUTING.md)