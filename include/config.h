// Version: Defined by PROJECT_VERSION in platformio.ini
// ESP32-S3 DevKitC-1 N16R8 - Meteo Station Configuration File

#ifndef CONFIG_H
#define CONFIG_H

#include "secrets.h" // Inclusion des données confidentielles

// ============================================================================
// PROJECT & BOARD INFORMATION
// ============================================================================
#define PROJECT_NAME "Meteo Station"

// ============================================================================
// SENSORS CONFIGURATION
// ============================================================================
#define DHTPIN              4     // Pin pour le capteur DHT11/DHT22
#define DHTTYPE             DHT11 // Type de capteur : DHT11, DHT22


// ============================================================================
// HARDWARE PIN DEFINITIONS - ESP32-S3 DevKitC-1 N16R8
// ============================================================================

// Light Sensor (ADC)
#define PIN_LIGHT_SENSOR    9     // S3 safe ADC pin

// NeoPixel RGB LED
#define PIN_NEOPIXEL        48    // Broche de la LED RGB intégrée sur l'ESP32-S3 DevKitC-1
#define NEOPIXEL_NUM_PIXELS 1
#define NEOPIXEL_BRIGHTNESS 50    // 0-255, a lower value is usually enough

// Buttons (Input with internal pull-up)
#define PIN_BUTTON_1        1     // Page switch button
#define PIN_BUTTON_2        2     // Reserved for future use

// Buzzer
#define PIN_BUZZER          3
#define BUZZER_LEDC_CHANNEL 0     // LEDC channel for the buzzer (0-7)

// I2C Sensors (BME280/OLED)
#define PIN_I2C_SDA         21    // I2C Data (pour BME280, etc.)
#define PIN_I2C_SCL         20    // I2C Clock (pour BME280, etc.)

// ============================================================================
// TFT DISPLAY SETTINGS
// ============================================================================
// --- Paramètres pour l'écran TFT ST7789 (SPI) --- (carré 1.54" 240x240)
// Le programme est conçu pour fonctionner même si l'écran n'est pas connecté.
#define TFT_WIDTH           240
#define TFT_HEIGHT          240
#define TFT_ROTATION        2     // 0=Portrait, 1=Landscape, 2=Portrait inverted, 3=Landscape inverted
#define TFT_BACKLIGHT_PWM   255   // Luminosité du rétroéclairage (0-255)
// --- Matériel Requis (SPI) ---
// TFT_MOSI (11) -> DIN/SDA sur l'écran
// TFT_SCLK (12) -> CLK/SCL sur l'écran
// TFT_CS   (10) -> CS sur l'écran
// TFT_DC   (6)  -> DC/RS sur l'écran
// TFT_RST  (7)  -> RST sur l'écran
// TFT_BL   (15) -> BLK/LED sur l'écran
#define TFT_MOSI            11
#define TFT_SCLK            12
#define TFT_CS              10
#define TFT_DC              6
#define TFT_RST             7
#define TFT_BL              15

// TFT Colors (RGB565 format)
#define TFT_COLOR_BG        0x0000  // Black
#define TFT_COLOR_HEADER    0x001F  // Blue
#define TFT_COLOR_TEXT      0xFFFF  // White
#define TFT_COLOR_VALUE     0x07E0  // Green
#define TFT_COLOR_WARNING   0xFFE0  // Yellow
#define TFT_COLOR_ERROR     0xF800  // Red
#define TFT_COLOR_SEPARATOR 0x4208  // Dark Gray

// --- Tailles de police (pour Adafruit GFX) ---
#define FONT_SIZE_DEFAULT   2     // Taille par défaut pour la plupart des textes
#define FONT_SIZE_HEADER    2     // Taille pour les textes du bandeau
#define FONT_SIZE_VALUE     3     // Taille pour les valeurs principales (température, etc.)
#define FONT_SIZE_SMALL     1     // Petite taille pour les détails

// Display pages
#define NUM_PAGES           3
#define PAGE_GPS_DATA       0
#define PAGE_DIAGNOSTICS    1
#define PAGE_SATELLITES     2

// ============================================================================
// GPS SETTINGS
// ============================================================================
#define GPS_MODEL "GT-U7"
#define GPS_BAUD_RATE 9600

// --- Broches de connexion GPS (UART 2) ---
#define PIN_GPS_RXD         8     // Connects to GPS TX
#define PIN_GPS_TXD         5     // Connects to GPS RX
#define PIN_GPS_PPS         38    // Pulse Per Second
#define GPS_UPDATE_RATE     1000  // Update display every 1000ms (1 Hz)
#define GPS_TIMEOUT         5000  // GPS data timeout in ms
#define GPS_FIX_TIMEOUT     60000 // Time to wait for fix before warning (60s)
#define HDOP_GOOD_THRESHOLD 2.0   // HDOP value below which the fix is considered "good"

// ============================================================================
// WIFI SETTINGS
// ============================================================================
#define WIFI_CONNECT_TIMEOUT 30000 // 30 secondes de timeout par réseau


// ============================================================================
// WEB SERVER SETTINGS
// ============================================================================
#define WEB_SERVER_PORT     80
#define WEB_UPDATE_INTERVAL 1000   // WebSocket update interval in ms

// ============================================================================
// BUZZER SETTINGS
// ============================================================================
#define BUZZER_BIP_FREQ     2000   // Fréquence pour un bip standard (Hz)
#define BUZZER_BIP_DURATION 100    // Durée d'un bip standard (ms)
#define BUZZER_ALERT_FREQ   1000   // Fréquence pour une alerte (Hz)
#define BUZZER_ALERT_DURATION 500  // Durée d'une alerte (ms)

// ============================================================================
// LED SETTINGS
// ============================================================================
// NeoPixel status colors (used in main.cpp)
#define NEOPIXEL_COLOR_OFF      0
#define NEOPIXEL_COLOR_BLUE     Adafruit_NeoPixel::Color(0, 0, 255)
#define NEOPIXEL_COLOR_GREEN    Adafruit_NeoPixel::Color(0, 255, 0)
#define NEOPIXEL_COLOR_RED      Adafruit_NeoPixel::Color(255, 0, 0)
#define NEOPIXEL_BLINK_INTERVAL 500 // ms for blinking

// ============================================================================
// NTP (TIME) SETTINGS
// ============================================================================
#define NTP_SERVER          "pool.ntp.org"
#define GMT_OFFSET_SEC      3600      // Décalage UTC+1 (Paris)
#define DAYLIGHT_OFFSET_SEC 3600      // Heure d'été

// ============================================================================
// JSON & API SETTINGS
// ============================================================================
#define API_UPDATE_INTERVAL 900000 // Intervalle de mise à jour des données météo (15 min)

// JSON buffer size for web data
#define JSON_BUFFER_SIZE    2048

// ============================================================================
// BUTTON DEBOUNCE
// ============================================================================
#define BUTTON_DEBOUNCE_MS  50     // Debounce delay in milliseconds

// ============================================================================
// DEBUG SETTINGS
// ============================================================================
#define SERIAL_BAUD_RATE    115200
#define DEBUG_ENABLED       true   // Set to false to disable debug output

#if DEBUG_ENABLED
  #define LOG_PRINT(x)    Serial.print(x)
  #define LOG_PRINTLN(x)  Serial.println(x)
  #define LOG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
  #define LOG_PRINT(x)
  #define LOG_PRINTLN(x)
  #define LOG_PRINTF(...)
#endif

// ============================================================================
// SAFE BOOT PINS - DO NOT USE
// ============================================================================
// GPIO 0: Boot button (strapping pin)
// GPIO 35, 36, 37: Reserved for OPI PSRAM
// GPIO 45, 46: Strapping pins
// These pins are intentionally avoided in the design

#endif // CONFIG_H