# Installation & Wiring Guide

## Prerequisites
* **Software:** Visual Studio Code + PlatformIO Extension.
* **Drivers:** USB Drivers for ESP32-S3 (CH340 or CP210x depending on board).

## Wiring Diagram (Pinout)

| Component | Pin Name | ESP32-S3 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **I2C Sensors** | SDA | 21 | AHT20 & BMP280 shared |
| | SCL | 20 | |
| **TFT Display** | MOSI/SDA | 11 | SPI Data |
| | SCLK/SCL | 12 | SPI Clock |
| | CS | 10 | Chip Select |
| | DC | 6 | Data/Command |
| | RST | 7 | Reset |
| | BL | 15 | Backlight |
| **GPS (GT-U7)** | TX | 8 | Connect GPS TX to ESP RX pin |
| | RX | 5 | Connect GPS RX to ESP TX pin |
| **Buttons** | Page Btn | 1 | Pull-up Internal |
| | Action Btn | 2 | Pull-up Internal |
| **Misc** | Buzzer | 3 | |
| | LDR (Light) | 9 | Analog Input |
| | NeoPixel | 48 | On-board RGB LED |

## Libraries
Dependencies are automatically managed via `platformio.ini`. Major libs used:
* `Adafruit GFX` & `ST7789`
* `ArduinoJson` (v7)
* `UniversalTelegramBot`
* `TinyGPSPlus`
* `ESPAsyncWebServer`