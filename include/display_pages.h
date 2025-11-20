// display_pages.h - Gestion centralisee de l'affichage TFT ST7789 - VERSION FINALE
#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <time.h>
#include <WiFi.h>
#include "config.h"
#include "icons.h"

// Declarations externes necessaires
extern Adafruit_ST7789 display;
extern bool isConnected;
extern bool isWeatherBandActive;
extern String weatherDescription;
extern String weatherAlert;
extern String alertColor;
extern int temperature;
extern float dhtTemp;
extern float dhtHum;
extern int iconCode;
extern int lightLevel;
extern TinyGPSPlus gps;
extern String WB_LAT;
extern String WB_LON;
extern String dynamicInsee;
extern bool isGPSAcquired;
extern bool tftInitialized;
extern unsigned long lastTFTRefresh;
extern const unsigned long tftRefreshInterval;

// Structures externes
struct Forecast;
extern Forecast forecasts[5];
extern bool forecastsAvailable;

struct AirQuality;
extern AirQuality airQuality;
extern bool airQualityAvailable;

// Fonctions externes
extern void updateForecasts();
extern void updateAirQuality();
extern void initTFT();
extern bool checkTFT();
extern void recoverTFT();
extern void drawDegreeSymbol(int16_t x, int16_t y, uint16_t color);
extern void displayTemperature(int temp, uint16_t color);

// Variables pour la stabilite de l'affichage
unsigned long lastDisplayUpdate = 0;
const unsigned long displayUpdateInterval = 1000; // Limite la frequence d'update
bool displayBusy = false;

// ===================================
// FONCTIONS DE PROTECTION TFT
// ===================================

bool isDisplayReady() {
  if (displayBusy) {
    Serial.println("[TFT] Display occupe, attente...");
    return false;
  }
  
  if (!tftInitialized) {
    Serial.println("[TFT] Display non initialise");
    initTFT();
    return tftInitialized;
  }
  
  // Verification periodique
  if (millis() - lastTFTRefresh > tftRefreshInterval) {
    if (!checkTFT()) {
      Serial.println("[TFT] Test echoue, recuperation...");
      recoverTFT();
      lastTFTRefresh = millis();
      return tftInitialized;
    }
    lastTFTRefresh = millis();
  }
  
  return true;
}

void safeDisplayOperation(void (*operation)()) {
  if (!isDisplayReady()) return;
  
  displayBusy = true;
  operation();
  yield(); // Permet aux autres taches de s'executer
  displayBusy = false;
}

// ===================================
// FONCTIONS UTILITAIRES TFT SECURISEES
// ===================================

void drawCircularGauge(int centerX, int centerY, int radius, float value, float maxValue, uint16_t color) {
  if (!isDisplayReady()) return;
  
  float angle = constrain(map(value, 0, maxValue, 0, 270), 0, 270);
  
  // Contour de la jauge
  display.drawCircle(centerX, centerY, radius, COLOR_WHITE);
  yield();
  
  // Arc de valeur - optimise pour eviter les blocages
  for (int i = 0; i <= angle; i += 2) { // Pas de 2 pour reduire les calculs
    float rad = (i - 135) * PI / 180;
    int x1 = centerX + (radius - 3) * cos(rad);
    int y1 = centerY + (radius - 3) * sin(rad);
    int x2 = centerX + (radius - 1) * cos(rad);
    int y2 = centerY + (radius - 1) * sin(rad);
    display.drawLine(x1, y1, x2, y2, color);
    
    if (i % 20 == 0) yield(); // Yield periodique
  }
}

void drawProgressBar(int x, int y, int width, int height, float value, float maxValue, uint16_t color) {
  if (!isDisplayReady()) return;
  
  int fillWidth = constrain(map(value, 0, maxValue, 0, width), 0, width);
  
  display.drawRect(x, y, width, height, COLOR_WHITE);
  yield();
  if (fillWidth > 2) {
    display.fillRect(x + 1, y + 1, fillWidth - 2, height - 2, color);
  }
  yield();
}

// ===================================
// FONCTION HEADER COMMUNE OPTIMISEE
// ===================================

void drawCommonHeader() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    // Fond du header
    display.fillRect(0, 0, SCREEN_WIDTH, 20, COLOR_BLACK);
    yield();
    
    // Logo Wi-Fi avec couleurs
    if (isConnected) {
      long rssi = WiFi.RSSI();
      uint16_t wifiColor = COLOR_GREEN;
      if (rssi <= -80) wifiColor = COLOR_RED;
      else if (rssi <= -65) wifiColor = COLOR_YELLOW;
      
      if (rssi > -50) {
        drawIconWifiBar4(0, 2, wifiColor);
      } else if (rssi > -65) {
        drawIconWifiBar3(0, 2, wifiColor);
      } else if (rssi > -80) {
        drawIconWifiBar2(0, 2, wifiColor);
      } else {
        drawIconWifiBar1(0, 2, wifiColor);
      }
    } else {
      drawIconWifiNoSignal(0, 2, COLOR_RED);
    }
    yield();

    // Temperatures avec couleurs conditionnelles
    display.setTextSize(1);
    uint16_t tempColor = COLOR_WHITE;
    if (temperature > 30) tempColor = COLOR_RED;
    else if (temperature > 25) tempColor = COLOR_ORANGE;
    else if (temperature < 5) tempColor = COLOR_BLUE;
    else if (temperature < 15) tempColor = COLOR_CYAN;
    
    display.setTextColor(tempColor);
    display.setCursor(25, 5);
    display.print(temperature);
    drawDegreeSymbol(display.getCursorX() + 2, 7, tempColor); // Symbole degre
    display.setCursor(display.getCursorX() + 6, 5);
    display.print("C");
    yield();
    
    // Separateur
    display.setTextColor(COLOR_WHITE);
    display.print(" / ");
    
    // Temperature capteur
    uint16_t dhtColor = COLOR_WHITE;
    if (dhtTemp > 30) dhtColor = COLOR_RED;
    else if (dhtTemp < 15) dhtColor = COLOR_CYAN;
    
    display.setTextColor(dhtColor);
    display.print((int)dhtTemp);
    drawDegreeSymbol(display.getCursorX() + 2, 7, dhtColor); // Symbole degre
    display.setCursor(display.getCursorX() + 6, 5);
    display.print("C");
    yield();
    
    // Icone meteo en couleur
    const unsigned char* iconBitmap = epd_bitmap_brouillard;
    uint16_t iconColor = COLOR_WHITE;
    
    switch (iconCode) {
      case 0:
      case 24:
        iconBitmap = epd_bitmap_soleil;
        iconColor = COLOR_YELLOW;
        break;
      case 1:
      case 2:
      case 3:
        iconBitmap = epd_bitmap_nuageux;
        iconColor = COLOR_WHITE;
        break;
      case 4:
      case 11:
        iconBitmap = epd_bitmap_tres_nuageux;
        iconColor = COLOR_GRAY;
        break;
      case 5:
        iconBitmap = epd_bitmap_couvert;
        iconColor = COLOR_GRAY;
        break;
      case 7:
      case 8:
      case 9:
      case 10:
        iconBitmap = epd_bitmap_pluie;
        iconColor = COLOR_BLUE;
        break;
      case 12:
      case 13:
      case 14:
      case 20:
        iconBitmap = epd_bitmap_neige;
        iconColor = COLOR_WHITE;
        break;
      case 15:
      case 22:
        iconBitmap = epd_bitmap_orage;
        iconColor = COLOR_YELLOW;
        break;
      case 16:
        iconBitmap = epd_bitmap_pluie_neige;
        iconColor = COLOR_CYAN;
        break;
      case 17:
        iconBitmap = epd_bitmap_verglas;
        iconColor = COLOR_CYAN;
        break;
      case 18:
      case 21:
        iconBitmap = epd_bitmap_grele;
        iconColor = COLOR_WHITE;
        break;
      case 6:
      case 19:
        iconBitmap = epd_bitmap_brouillard;
        iconColor = COLOR_GRAY;
        break;
      case 23:
        iconBitmap = epd_bitmap_vent_fort;
        iconColor = COLOR_WHITE;
        break;
    }
    
    display.drawBitmap(SCREEN_WIDTH - 18, 2, iconBitmap, 16, 16, iconColor);
    display.drawFastHLine(0, 20, SCREEN_WIDTH, COLOR_WHITE);
  });
}

// ===================================
// PAGES OPTIMISEES POUR LA STABILITE
// ===================================

void drawPage1() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
      Serial.println("Echec de la recuperation de l'heure");
      return;
    }

    // Affichage de l'heure en grand
    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    display.setTextSize(3);
    display.setTextColor(COLOR_GREEN);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(timeStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 40);
    display.println(timeStr);
    yield();
    
    // Affichage de la date
    char dateStr[11];
    strftime(dateStr, sizeof(dateStr), "%d/%m/%Y", &timeinfo);
    display.setTextSize(2);
    display.setTextColor(COLOR_CYAN);
    display.getTextBounds(dateStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 85);
    display.println(dateStr);
    yield();
    
    // Jour de la semaine
    char dayStr[10];
    strftime(dayStr, sizeof(dayStr), "%A", &timeinfo);
    display.setTextSize(1);
    display.setTextColor(COLOR_WHITE);
    display.getTextBounds(dayStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 115);
    display.println(dayStr);
    yield();
    
    // Conditions meteo avec couleur d'alerte
    String desc = weatherDescription;
    display.setTextSize(1);
    
    if (isWeatherBandActive) {
      uint16_t alertBgColor = COLOR_YELLOW;
      uint16_t alertTextColor = COLOR_BLACK;
      
      if (alertColor == "ROUGE") {
        alertBgColor = COLOR_RED;
        alertTextColor = COLOR_WHITE;
      } else if (alertColor == "ORANGE") {
        alertBgColor = COLOR_ORANGE;
        alertTextColor = COLOR_BLACK;
      }
      
      display.fillRect(0, 140, SCREEN_WIDTH, 30, alertBgColor);
      display.setTextColor(alertTextColor);
    } else {
      display.setTextColor(COLOR_WHITE);
    }
    yield();
    
    // Decoupage du texte si necessaire
    int wrap_length = SCREEN_WIDTH / 6;
    String line1 = "";
    String line2 = "";
    
    int space_idx = desc.lastIndexOf(' ', wrap_length);
    if (space_idx != -1 && desc.length() > wrap_length) {
      line1 = desc.substring(0, space_idx);
      line2 = desc.substring(space_idx + 1);
    } else {
      line1 = desc;
    }
    
    display.getTextBounds(line1, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 150);
    display.println(line1);
    yield();
    
    if (!line2.isEmpty()) {
      display.getTextBounds(line2, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 160);
      display.println(line2);
    }

    // Indicateur de page
    display.setTextColor(COLOR_WHITE);
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("1/6");
  });
}

void drawPage2() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    display.setTextSize(3);
    
    uint16_t alertBgColor = COLOR_GREEN;
    uint16_t alertTextColor = COLOR_WHITE;
    String statusText = "VERT";
    
    if (isWeatherBandActive) {
      statusText = alertColor;
      if (alertColor == "ROUGE") {
        alertBgColor = COLOR_RED;
        alertTextColor = COLOR_WHITE;
      } else if (alertColor == "ORANGE") {
        alertBgColor = COLOR_ORANGE;
        alertTextColor = COLOR_BLACK;
      } else if (alertColor == "JAUNE") {
        alertBgColor = COLOR_YELLOW;
        alertTextColor = COLOR_BLACK;
      }
    }
    
    // Bande d'alerte coloree
    display.fillRect(0, 40, SCREEN_WIDTH, 40, alertBgColor);
    display.setTextColor(alertTextColor);
    yield();
    
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(statusText.c_str(), 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 55);
    display.println(statusText);
    yield();
    
    // Texte de l'alerte
    display.setTextColor(COLOR_WHITE);
    display.setTextSize(1);
    display.setCursor(5, 90);
    
    if (isWeatherBandActive) {
      display.println(weatherAlert);
    } else {
      display.println("Aucune alerte active");
    }
    yield();
    
    // Informations environnementales avec jauges
    display.setCursor(5, 120);
    display.print("Humidite: ");
    display.print((int)dhtHum);
    display.println("%");
    
    // Jauge d'humidite
    uint16_t humColor = COLOR_BLUE;
    if (dhtHum < 30) humColor = COLOR_RED;
    else if (dhtHum > 70) humColor = COLOR_CYAN;
    
    drawProgressBar(5, 135, SCREEN_WIDTH - 10, 8, dhtHum, 100, humColor);
    yield();
    
    // Luminosite
    display.setCursor(5, 155);
    display.print("Luminosite: ");
    display.print(lightLevel);
    
    // Jauge de luminosite
    uint16_t lightColor = COLOR_YELLOW;
    if (lightLevel < 100) lightColor = COLOR_BLUE;
    else if (lightLevel > 3000) lightColor = COLOR_WHITE;
    
    drawProgressBar(5, 170, SCREEN_WIDTH - 10, 8, lightLevel, 4095, lightColor);
    
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("2/6");
  });
}

void drawPage3() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    display.setTextSize(2);
    display.setTextColor(COLOR_CYAN);
    
    // Titre avec statut GPS
    int16_t x1, y1;
    uint16_t w, h;
    String title = "POSITION GPS";
    display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 30);
    display.println(title);
    yield();
    
    display.setTextSize(1);
    
    // Statut de la source
    if (gps.location.isValid()) {
      display.setTextColor(COLOR_GREEN);
      display.setCursor(5, 60);
      display.println("Source: GPS DIRECT");
    } else if (isGPSAcquired && !WB_LAT.isEmpty()) {
      display.setTextColor(COLOR_YELLOW);
      display.setCursor(5, 60);
      display.println("Source: GEOLOCALISATION IP");
    } else {
      display.setTextColor(COLOR_RED);
      display.setCursor(5, 60);
      display.println("Source: POSITION PAR DEFAUT");
    }
    yield();
    
    // Coordonnees
    display.setTextColor(COLOR_WHITE);
    display.setCursor(5, 80);
    display.print("Latitude: ");
    display.setTextColor(COLOR_CYAN);
    if (gps.location.isValid()) {
      display.println(gps.location.lat(), 6);
    } else {
      display.println(WB_LAT);
    }

    display.setTextColor(COLOR_WHITE);
    display.setCursor(5, 100);
    display.print("Longitude: ");
    display.setTextColor(COLOR_CYAN);
    if (gps.location.isValid()) {
      display.println(gps.location.lng(), 6);
    } else {
      display.println(WB_LON);
    }
    yield();

    // Informations supplementaires
    if (gps.location.isValid()) {
      display.setTextColor(COLOR_WHITE);
      display.setCursor(5, 130);
      display.print("Satellites: ");
      display.setTextColor(COLOR_GREEN);
      display.println(gps.satellites.value());
      
      display.setTextColor(COLOR_WHITE);
      display.setCursor(5, 150);
      display.print("Precision (HDOP): ");
      display.setTextColor(COLOR_YELLOW);
      display.println(gps.hdop.hdop(), 1);
      
      if (gps.altitude.isValid()) {
        display.setTextColor(COLOR_WHITE);
        display.setCursor(5, 170);
        display.print("Altitude: ");
        display.setTextColor(COLOR_CYAN);
        display.print(gps.altitude.meters(), 1);
        display.println(" m");
      }
    } else {
      display.setTextColor(COLOR_WHITE);
      display.setCursor(5, 130);
      display.print("Code INSEE: ");
      display.setTextColor(COLOR_YELLOW);
      display.println(dynamicInsee);
      
      display.setTextColor(COLOR_GRAY);
      display.setCursor(5, 150);
      display.println("Donnees approximatives");
    }

    display.setTextColor(COLOR_WHITE);
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("3/6");
  });
}

void drawPage4() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    display.setTextSize(2);
    display.setTextColor(COLOR_YELLOW);
    
    int16_t x1, y1;
    uint16_t w, h;
    String title = "LUMINOSITE";
    display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 30);
    display.println(title);
    yield();
    
    // Valeur numerique grande
    display.setTextSize(3);
    display.setTextColor(COLOR_WHITE);
    String lightStr = String(lightLevel);
    display.getTextBounds(lightStr, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 70);
    display.print(lightLevel);
    yield();
    
    // Unite
    display.setTextSize(1);
    display.setCursor(5, 110);
    display.println("Valeur brute du capteur LDR");
    
    // Jauge circulaire
    uint16_t gaugeColor = COLOR_BLUE;
    if (lightLevel > 3000) gaugeColor = COLOR_WHITE;
    else if (lightLevel > 1000) gaugeColor = COLOR_YELLOW;
    else if (lightLevel > 300) gaugeColor = COLOR_ORANGE;
    
    drawCircularGauge(SCREEN_WIDTH/2, 150, 30, lightLevel, 4095, gaugeColor);
    yield();
    
    // Interpretation
    display.setTextSize(1);
    display.setTextColor(COLOR_WHITE);
    display.setCursor(5, 190);
    if (lightLevel < 100) {
      display.setTextColor(COLOR_BLUE);
      display.println("TRES SOMBRE");
    } else if (lightLevel < 500) {
      display.setTextColor(COLOR_CYAN);
      display.println("SOMBRE");
    } else if (lightLevel < 1500) {
      display.setTextColor(COLOR_YELLOW);
      display.println("LUMINEUX");
    } else {
      display.setTextColor(COLOR_WHITE);
      display.println("TRES LUMINEUX");
    }
    
    display.setTextColor(COLOR_WHITE);
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("4/6");
  });
}

void drawPage5() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    if (!forecastsAvailable) {
      updateForecasts();
    }
    
    display.setTextSize(2);
    display.setTextColor(COLOR_CYAN);
    
    int16_t x1, y1;
    uint16_t w, h;
    String title = "PREVISIONS";
    display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 30);
    display.println(title);
    yield();
    
    display.setTextSize(1);
    
    if (forecastsAvailable) {
      const char* days[] = {"Auj", "Dem", "J+2", "J+3", "J+4"};
      
      for (int i = 1; i <= 3 && i < 5; i++) {
        int yPos = 60 + (i-1) * 25;
        
        // Jour
        display.setTextColor(COLOR_WHITE);
        display.setCursor(5, yPos);
        display.print(days[i]);
        display.print(": ");
        
        // Icone meteo simplifiee avec couleur
        uint16_t iconColor = COLOR_WHITE;
        char iconChar = '?';
        
        if (forecasts[i].weatherCode <= 2) {
          iconChar = 'S'; // Soleil
          iconColor = COLOR_YELLOW;
        } else if (forecasts[i].weatherCode <= 5) {
          iconChar = 'N'; // Nuageux
          iconColor = COLOR_GRAY;
        } else if (forecasts[i].weatherCode <= 11) {
          iconChar = 'P'; // Pluie
          iconColor = COLOR_BLUE;
        } else {
          iconChar = '?';
          iconColor = COLOR_RED;
        }
        
        display.setTextColor(iconColor);
        display.print(iconChar);
        display.print(" ");
        
        // Temperatures avec symbole degre
        display.setTextColor(COLOR_CYAN);
        display.print(forecasts[i].tempMin);
        display.print("/");
        display.setTextColor(COLOR_RED);
        display.print(forecasts[i].tempMax);
        drawDegreeSymbol(display.getCursorX() + 2, yPos + 2, COLOR_RED);
        display.setCursor(display.getCursorX() + 6, yPos);
        display.setTextColor(COLOR_WHITE);
        display.print("C");
        
        // Probabilite de pluie
        if (forecasts[i].precipProb > 30) {
          display.setTextColor(COLOR_BLUE);
          display.print(" ");
          display.print(forecasts[i].precipProb);
          display.print("%");
        }
        
        // Description sur la ligne suivante
        display.setTextColor(COLOR_GRAY);
        display.setCursor(15, yPos + 10);
        String desc = forecasts[i].description;
        if (desc.length() > 18) {
          desc = desc.substring(0, 15) + "...";
        }
        display.println(desc);
        
        yield();
      }
    } else {
      display.setTextColor(COLOR_YELLOW);
      display.setCursor(5, 80);
      display.println("Chargement des previsions...");
      
      display.setTextColor(COLOR_GRAY);
      display.setCursor(5, 100);
      display.println("Verifiez la connexion WiFi");
    }
    
    display.setTextColor(COLOR_WHITE);
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("5/6");
  });
}

void drawPage6() {
  if (!isDisplayReady()) return;
  
  safeDisplayOperation([]() {
    if (!airQualityAvailable) {
      updateAirQuality();
    }
    
    display.setTextSize(2);
    display.setTextColor(COLOR_GREEN);
    
    int16_t x1, y1;
    uint16_t w, h;
    String title = "QUALITE AIR";
    display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 30);
    display.println(title);
    yield();
    
    if (airQualityAvailable) {
      // AQI avec couleur selon la qualite
      display.setTextSize(4);
      uint16_t aqiColor = COLOR_GREEN;
      String aqiLabel = "BON";
      
      switch(airQuality.aqi) {
        case 1: 
          aqiColor = COLOR_GREEN;
          aqiLabel = "BON";
          break;
        case 2:
          aqiColor = COLOR_YELLOW;
          aqiLabel = "OK";
          break;
        case 3:
          aqiColor = COLOR_ORANGE;
          aqiLabel = "MOYEN";
          break;
        case 4:
          aqiColor = COLOR_RED;
          aqiLabel = "MAUVAIS";
          break;
        case 5:
          aqiColor = COLOR_MAGENTA;
          aqiLabel = "DANGER";
          break;
      }
      
      display.setTextColor(aqiColor);
      String aqiStr = String(airQuality.aqi);
      display.getTextBounds(aqiStr, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 70);
      display.print(airQuality.aqi);
      yield();
      
      // Label de qualite
      display.setTextSize(1);
      display.getTextBounds(aqiLabel, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 110);
      display.println(aqiLabel);
      
      // Details des polluants
      display.setTextSize(1);
      display.setTextColor(COLOR_WHITE);
      display.setCursor(5, 130);
      display.print("PM2.5: ");
      display.setTextColor(COLOR_CYAN);
      display.print(airQuality.pm25, 0);
      display.setTextColor(COLOR_WHITE);
      display.println(" ug/m3");
      
      display.setCursor(5, 145);
      display.print("PM10: ");
      display.setTextColor(COLOR_CYAN);
      display.print(airQuality.pm10, 0);
      display.setTextColor(COLOR_WHITE);
      display.println(" ug/m3");
      
      display.setCursor(5, 160);
      display.print("NO2: ");
      display.setTextColor(COLOR_YELLOW);
      display.print(airQuality.no2, 0);
      display.setTextColor(COLOR_WHITE);
      display.println(" ug/m3");
      
      display.setCursor(5, 175);
      display.print("O3: ");
      display.setTextColor(COLOR_GREEN);
      display.print(airQuality.o3, 0);
      display.setTextColor(COLOR_WHITE);
      display.println(" ug/m3");
      
      // Polluant principal
      display.setTextColor(COLOR_RED);
      display.setCursor(5, 195);
      display.print("Principal: ");
      display.println(airQuality.mainPollutant);
      
    } else {
      display.setTextSize(1);
      display.setTextColor(COLOR_YELLOW);
      display.setCursor(5, 80);
      display.println("Donnees qualite air");
      display.setCursor(5, 95);
      display.println("non disponibles");
      
      display.setTextColor(COLOR_GRAY);
      display.setCursor(5, 120);
      display.println("Verifiez la connexion");
      display.setCursor(5, 135);
      display.println("ou l'API key");
    }
    
    display.setTextColor(COLOR_WHITE);
    display.setCursor(SCREEN_WIDTH - 20, SCREEN_HEIGHT - 15);
    display.print("6/6");
  });
}

// ===================================
// FONCTION PRINCIPALE D'AFFICHAGE SECURISEE
// ===================================

void displayPage(int pageNumber) {
  // Limite la frequence d'update pour eviter les surcharges
  if (millis() - lastDisplayUpdate < displayUpdateInterval) {
    return;
  }
  
  if (!isDisplayReady()) {
    Serial.println("[TFT] Display non pret pour l'affichage");
    return;
  }
  
  Serial.printf("[TFT] Affichage page %d\n", pageNumber);
  
  safeDisplayOperation([]() {
    display.fillScreen(COLOR_BLACK);
  });
  
  yield();
  drawCommonHeader();
  yield();
  
  switch (pageNumber) {
    case 1:
      drawPage1();
      break;
    case 2:
      drawPage2();
      break;
    case 3:
      drawPage3();
      break;
    case 4:
      drawPage4();
      break;
    case 5:
      drawPage5();
      break;
    case 6:
      drawPage6();
      break;
    default:
      drawPage1();
  }
  
  lastDisplayUpdate = millis();
  Serial.println("[TFT] Page affichee avec succes");
}