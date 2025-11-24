#ifndef CONFIG_H
#define CONFIG_H

#include "secrets.h"

#define PROJECT_NAME "Meteo Station ESP32-S3"

// ================= PINS I2C =================
#define PIN_I2C_SDA         21 
#define PIN_I2C_SCL         20 

// ================= CAPTEURS =================
#define BMP280_I2C_ADDRESS  0x76 
#define SEALEVELPRESSURE_HPA (1013.25)

// LDR (Lumière) 
#define PIN_LIGHT_SENSOR    4    
#define LDR_THRESHOLD_NIGHT 500 

// NeoPixel
#define PIN_NEOPIXEL        48
#define NEOPIXEL_NUM_PIXELS 1
#define NEOPIXEL_BRIGHTNESS 30   

// Boutons
#define PIN_BUTTON_PAGE     1    
#define PIN_BUTTON_ACTION   2    

// Buzzer
#define PIN_BUZZER          3
#define BUZZER_LEDC_CHANNEL 0    

// ================= ECRAN TFT ST7789 =================
#define TFT_WIDTH           240
#define TFT_HEIGHT          240
#define TFT_ROTATION        2    
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_CS              10
#define TFT_DC              6
#define TFT_RST             7
#define TFT_BL              15   

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
#define PIN_GPS_RXD         8  
#define PIN_GPS_TXD         5  
#define GPS_BAUD_RATE       9600

// ================= TIMERS (ms) =================
#define INTERVAL_SENSORS    5000
#define INTERVAL_WEATHER    120000  // 2 minutes (changé de 900000 / 15 min)
#define INTERVAL_TELEGRAM   3000   
#define INTERVAL_DISPLAY    250    

// Seuils d'alerte
#define ALERT_TEMP_HIGH     30.0
#define ALERT_TEMP_LOW      0.0

#endif