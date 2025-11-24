# ESP32 Weather Station (Meteo_Station Project)

**Current Version: 2.8.0** (as of 2025-11-24)

Note: v2.6.0 improves reliability by switching the AQI data source to the Open‑Meteo Air Quality API (free, no API key required). The project remains on the stable network library set introduced in the v2.5.x rollback.

## 📖 About This Project

This project turns an ESP32-S3 microcontroller into a comprehensive, multi-function weather station. It gathers data from local sensors (temperature, humidity, pressure) and enhances it with forecasts and external data via the Open-Meteo API.

The user interface is displayed on a color TFT screen and can be controlled via physical buttons. The project also includes a web server for remote access, a JSON API for data integration, and a Telegram bot for notifications and commands.

## ✨ Key Features

- **Multi-Page Interface:** Clear navigation between several screens (7 pages):
  1.  **Summary:** Current weather, temperature, and a dynamic icon.
  2.  **Forecast:** 3-day weather forecast with min/max temperatures.
  3.  **Environment:** Data from local sensors (humidity, pressure, light, air quality).
  4.  **Weather Alerts:** Dedicated page for weather alerts from OpenWeatherMap.
  5.  **GPS:** Coordinates, speed, altitude, and fix status.
  6.  **Network & System:** Network information (IP, SSID, RSSI, MAC, Provider).
  7.  **System Status:** Uptime, free RAM, alert status, and WiFi details.
- **Modern Graphics:** Procedurally drawn "Flat Design" vector weather icons for a crisp display.
- **Advanced Connectivity:**
  - **Asynchronous Web Server:** A web interface to view data from a browser.
  - **JSON API (`/api/data`):** Allows for easy integration of the station's data into other systems.
  - **Telegram Bot:** Control the station and receive alerts directly on Telegram.
  - **Telegram Bot:** Control the station and receive alerts directly on Telegram. New command: `/getreport` — request a full status report on demand. The bot now sends only a short startup message at boot; the first full report is sent automatically once sensor/weather measurements are available.
- **Stability & Performance:**
  - **Network Anti-Freeze:** An HTTP request timeout prevents the device from freezing.
  - **Reliable Button Handling:** Uses interrupts and a refractory period to prevent bounce and double presses.
- **Smart Alert System:** A dedicated page automatically appears for weather alerts, complete with visual and audible indicators.

## 🛠️ Hardware

- **Microcontroller:** ESP32-S3 (Model used: `4d_systems_esp32s3_gen4_r8n16`)
- **Display:** ST7789-compatible color TFT screen.
- **Sensors:**
  - **Temperature & Humidity:** Adafruit AHT20
  - **Pressure & Altitude:** Adafruit BMP280
  - **Geolocation:** GPS Module (using TinyGPSPlus)
- **Interaction:** Push buttons connected to GPIO pins.
- **Audio:** Passive buzzer for sound notifications.
- **Visuals:** RGB LED (NeoPixel) for status indicators.

## 📚 Software Libraries (Dependencies)

This project is built with PlatformIO and relies on the following libraries:

| Library                                        | Version | Role                                                              |
| ---------------------------------------------- | ------- | ----------------------------------------------------------------- |
| `esphome/ESPAsyncWebServer-esphome`            | `^3.1.0`  | Creates a high-performance asynchronous web server.               |
| `esphome/AsyncTCP-esphome`                     | `^2.1.0`  | Asynchronous TCP layer required by the web server.                |
| `bblanchon/ArduinoJson`                        | `^7.0.4`  | Parses JSON responses from the Open-Meteo API.                    |
| `witnessmenow/UniversalTelegramBot`            | `^1.3.0`  | Manages communication with the Telegram Bot API.                  |
| `adafruit/Adafruit GFX Library`                | `^1.11.9` | Core graphics engine for drawing shapes and text.                 |
| `adafruit/Adafruit ST7735 and ST7789 Library` | `^1.10.3` | Driver for the color TFT display.                                 |
| `mikalhart/TinyGPSPlus`                        | `^1.1.0`  | Parses NMEA sentences from the GPS module.                        |
| `adafruit/Adafruit NeoPixel`                   | `^1.12.0` | Controls addressable RGB LEDs.                                    |
| `adafruit/Adafruit Unified Sensor`             | `^1.1.14` | Hardware abstraction layer for Adafruit sensors.                  |
| `adafruit/Adafruit AHTX0`                      | `^2.0.5`  | Driver for the AHT20 temperature and humidity sensor.             |
| `adafruit/Adafruit BMP280 Library`             | `^2.6.8`  | Driver for the BMP280 barometric pressure sensor.                 |