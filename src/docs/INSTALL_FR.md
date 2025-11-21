# Guide d'Installation et Câblage

## Prérequis
* **Logiciel :** Visual Studio Code + Extension PlatformIO.
* **Pilotes :** Pilotes USB pour ESP32-S3 (CH340 ou CP210x selon la carte).

## Plan de Câblage (Pinout)

| Composant | Nom Pin | ESP32-S3 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **Capteurs I2C** | SDA | 21 | AHT20 & BMP280 partagés |
| | SCL | 20 | |
| **Écran TFT** | MOSI/SDA | 11 | Données SPI |
| | SCLK/SCL | 12 | Horloge SPI |
| | CS | 10 | Chip Select |
| | DC | 6 | Données/Commande |
| | RST | 7 | Reset |
| | BL | 15 | Rétroéclairage |
| **GPS (GT-U7)** | TX | 8 | Connecter GPS TX sur ESP Pin 8 |
| | RX | 5 | Connecter GPS RX sur ESP Pin 5 |
| **Boutons** | Btn Page | 1 | Pull-up Interne |
| | Btn Action | 2 | Pull-up Interne |
| **Divers** | Buzzer | 3 | |
| | LDR (Lum) | 9 | Entrée Analogique |
| | NeoPixel | 48 | LED RGB intégrée |

## Bibliothèques
Les dépendances sont gérées automatiquement via `platformio.ini`. Principales lib :
* `Adafruit GFX` & `ST7789`
* `ArduinoJson` (v7)
* `UniversalTelegramBot`
* `TinyGPSPlus`
* `ESPAsyncWebServer`