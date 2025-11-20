#ifndef MAIN_H
#define MAIN_H

#include "config.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <TinyGPS++.h>
#include <Adafruit_NeoPixel.h>
#include "DHT.h"

// ===================================
// EXTERN DECLARATIONS
// ===================================
// Ces déclarations indiquent au compilateur que ces variables existent
// et seront définies dans un autre fichier (main.cpp).

// Hardware Objects
extern Adafruit_ST7789 tft;
extern Adafruit_NeoPixel pixels;
extern TinyGPSPlus gps;
extern HardwareSerial SerialGPS;
extern DHT dht;

// System State
extern bool isConnected;
extern bool isGPSAcquired;
extern int currentPage;
extern bool tftInitialized;

// Sensor Data
extern float dhtTemp;
extern float dhtHum;
extern int lightLevel;

// Function Prototypes
void displayPage(int page);
void setStatusLED(uint32_t color);
void beep(int freq, int duration);

#endif // MAIN_H