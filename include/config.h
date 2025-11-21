#ifndef CONFIG_H
#define CONFIG_H

#include "secrets.h"

#define PROJECT_NAME "Meteo Station S3"

// ================= PINS I2C (SDA/SCL) =================
// Adapter selon le cablage réel sur le DevKitC-1
#define PIN_I2C_SDA         21 
#define PIN_I2C_SCL         20 

// ================= CAPTEURS =================
// AHT20 (0x38) + BMP280 (0x77 ou 0x76)
#define BMP280_I2C_ADDRESS  0x76 
#define SEALEVELPRESSURE_HPA (1013.25)

// LDR (Lumière)
#define PIN_LIGHT_SENSOR    9    // ADC 
#define LDR_THRESHOLD_NIGHT 1000 // Seuil pour mode nuit (à calibrer : 0-4095)

// NeoPixel
#define PIN_NEOPIXEL        48
#define NEOPIXEL_NUM_PIXELS 1
#define NEOPIXEL_BRIGHTNESS 30   

// Boutons (Pull-up interne activé)
#define PIN_BUTTON_PAGE     1    // Bouton pour changer de page
#define PIN_BUTTON_ACTION   2    // Bouton action (refresh/autre)

// Buzzer
#define PIN_BUZZER          3
#define BUZZER_LEDC_CHANNEL 0     // LEDC channel for the buzzer (0-7)

// ================= ECRAN TFT ST7789 (SPI) =================
#define TFT_WIDTH           240
#define TFT_HEIGHT          240
#define TFT_ROTATION        2    // 0 ou 2 pour portrait, 1 ou 3 pour paysage
// Pins SPI
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_CS              10
#define TFT_DC              6
#define TFT_RST             7
#define TFT_BL              15   // Backlight

// Couleurs UI
#define C_BLACK   0x0000
#define C_WHITE   0xFFFF
#define C_BLUE    0x001F
#define C_GREEN   0x07E0
#define C_RED     0xF800
#define C_YELLOW  0xFFE0
#define C_GREY    0x528A
#define C_ORANGE  0xFD20

// ================= GPS GT-U7 =================
#define PIN_GPS_RXD         8  // Connecté au TX du GPS
#define PIN_GPS_TXD         5  // Connecté au RX du GPS
#define GPS_BAUD_RATE       9600

// ================= TIMERS (ms) =================
#define INTERVAL_SENSORS    5000
#define INTERVAL_WEATHER    900000 // 15 min
#define INTERVAL_TELEGRAM   3000   // Verification telegram (augmenté pour éviter lag)
#define INTERVAL_DISPLAY    250    // Rafraichissement UI fluide

// Seuils d'alerte
#define ALERT_TEMP_HIGH     30.0
#define ALERT_TEMP_LOW      0.0

#endif