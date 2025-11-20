// icons.h - Icônes météo et WiFi pour TFT ST7789
#ifndef ICONS_H
#define ICONS_H

#include <Arduino.h>
#if defined(ESP32) || defined(ESP8266)
  #include <pgmspace.h>
#else
  #include <avr/pgmspace.h>
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include "config.h"

extern Adafruit_ST7789 display;

// ===================================
// DONNEES D'ICONES METEO
// ===================================

const unsigned char epd_bitmap_brouillard [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xfc, 0x00, 0x00, 0x7f, 0xf0, 0x00, 0x00, 0x0f, 0xfe,
  0x00, 0x00, 0x7f, 0xfe, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_couvert [] PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x22, 0x20, 0x17, 0x40, 0x08, 0x80, 0x10, 0x40, 0x70, 0x70, 0x16, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_grele [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x04, 0x20, 0x20, 0x88, 0x02, 0x24, 0x08, 0x80
};

const unsigned char epd_bitmap_neige [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x00, 0x28, 0x14, 0x10, 0x08, 0x28, 0x14, 0x00
};

const unsigned char epd_bitmap_nuageux [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_orage [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x00, 0x40, 0x00, 0x40, 0x00, 0x80, 0x00, 0x80
};

const unsigned char epd_bitmap_pluie [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x08, 0x88, 0x11, 0x10, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_pluie_neige [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x06, 0x88,
  0x09, 0x04, 0x10, 0x02, 0x20, 0x02, 0x3f, 0xfe, 0x08, 0xa8, 0x15, 0x10, 0x08, 0x28, 0x14, 0x00
};

const unsigned char epd_bitmap_soleil [] PROGMEM = {
  0x00, 0x00, 0x02, 0x00, 0x22, 0x20, 0x17, 0x40, 0x08, 0x80, 0x10, 0x40, 0x70, 0x70, 0x10, 0x40,
  0x08, 0x80, 0x17, 0x40, 0x22, 0x20, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_tres_nuageux [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xc0, 0x1a, 0x20, 0x24, 0x30, 0x40, 0x08,
  0x80, 0x0c, 0xff, 0xfa, 0x20, 0x02, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_vent_fort [] PROGMEM = {
  0x00, 0xc0, 0x00, 0x20, 0x06, 0x20, 0x01, 0x40, 0x01, 0x18, 0x02, 0x04, 0x60, 0x04, 0x7f, 0xf8,
  0x60, 0x04, 0x00, 0x82, 0x00, 0x52, 0x00, 0x4c, 0x01, 0x88, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00
};

const unsigned char epd_bitmap_verglas [] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x11, 0x40, 0x28, 0x80, 0x10, 0x10, 0x00, 0x28, 0x42, 0x10,
  0xa5, 0x00, 0x42, 0x04, 0x00, 0x4a, 0x10, 0xa4, 0x28, 0x40, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00
};

// ===================================
// FONCTIONS ICONES WIFI COLOREES
// ===================================

void drawIconWifiNoSignal(int x, int y, uint16_t color = COLOR_RED) {
    // Barres WiFi grises
    display.drawLine(x + 1, y + 15, x + 1, y + 12, COLOR_GRAY);
    display.drawLine(x + 5, y + 15, x + 5, y + 9, COLOR_GRAY);
    display.drawLine(x + 9, y + 15, x + 9, y + 6, COLOR_GRAY);
    display.drawLine(x + 13, y + 15, x + 13, y + 3, COLOR_GRAY);
    display.drawFastHLine(x + 0, y + 15, 15, COLOR_GRAY);
    
    // Croix rouge pour indiquer pas de signal
    display.drawLine(x + 0, y + 0, x + 15, y + 15, color);
    display.drawLine(x + 15, y + 0, x + 0, y + 15, color);
}

void drawIconWifiBar1(int x, int y, uint16_t color = COLOR_RED) {
    // 1 barre - signal faible
    display.fillRect(x + 1, y + 12, 2, 4, color);
    display.drawFastHLine(x + 0, y + 15, 15, COLOR_WHITE);
    
    // Barres inactives en gris
    display.drawLine(x + 5, y + 15, x + 5, y + 9, COLOR_GRAY);
    display.drawLine(x + 9, y + 15, x + 9, y + 6, COLOR_GRAY);
    display.drawLine(x + 13, y + 15, x + 13, y + 3, COLOR_GRAY);
}

void drawIconWifiBar2(int x, int y, uint16_t color = COLOR_YELLOW) {
    // 2 barres - signal moyen
    display.fillRect(x + 1, y + 12, 2, 4, color);
    display.fillRect(x + 5, y + 9, 2, 7, color);
    display.drawFastHLine(x + 0, y + 15, 15, COLOR_WHITE);
    
    // Barres inactives en gris
    display.drawLine(x + 9, y + 15, x + 9, y + 6, COLOR_GRAY);
    display.drawLine(x + 13, y + 15, x + 13, y + 3, COLOR_GRAY);
}

void drawIconWifiBar3(int x, int y, uint16_t color = COLOR_YELLOW) {
    // 3 barres - bon signal
    display.fillRect(x + 1, y + 12, 2, 4, color);
    display.fillRect(x + 5, y + 9, 2, 7, color);
    display.fillRect(x + 9, y + 6, 2, 10, color);
    display.drawFastHLine(x + 0, y + 15, 15, COLOR_WHITE);
    
    // Barre inactive en gris
    display.drawLine(x + 13, y + 15, x + 13, y + 3, COLOR_GRAY);
}

void drawIconWifiBar4(int x, int y, uint16_t color = COLOR_GREEN) {
    // 4 barres - excellent signal
    display.fillRect(x + 1, y + 12, 2, 4, color);
    display.fillRect(x + 5, y + 9, 2, 7, color);
    display.fillRect(x + 9, y + 6, 2, 10, color);
    display.fillRect(x + 13, y + 3, 2, 13, color);
    display.drawFastHLine(x + 0, y + 15, 15, COLOR_WHITE);
}

// ===================================
// FONCTIONS UTILITAIRES D'AFFICHAGE
// ===================================

// Fonction pour afficher un texte centré avec une couleur de fond
void drawCenteredTextWithBackground(String text, int y, uint16_t textColor, uint16_t bgColor, int textSize = 1) {
    display.setTextSize(textSize);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
    
    int x = (SCREEN_WIDTH - w) / 2;
    
    // Fond coloré
    display.fillRect(x - 2, y - 2, w + 4, h + 4, bgColor);
    
    // Texte
    display.setTextColor(textColor);
    display.setCursor(x, y);
    display.print(text);
}

// Fonction pour dessiner un indicateur d'état coloré
void drawStatusIndicator(int x, int y, String status, uint16_t color) {
    // Cercle coloré
    display.fillCircle(x, y + 4, 3, color);
    display.drawCircle(x, y + 4, 3, COLOR_WHITE);
    
    // Texte du statut
    display.setTextSize(1);
    display.setTextColor(COLOR_WHITE);
    display.setCursor(x + 8, y);
    display.print(status);
}

// Fonction pour dessiner une icône de batterie (pour usage futur)
void drawBatteryIcon(int x, int y, int percentage, uint16_t color = COLOR_GREEN) {
    // Contour de la batterie
    display.drawRect(x, y + 2, 20, 8, COLOR_WHITE);
    display.fillRect(x + 20, y + 4, 2, 4, COLOR_WHITE);
    
    // Niveau de batterie
    int fillWidth = map(percentage, 0, 100, 0, 18);
    uint16_t fillColor = color;
    
    if (percentage < 20) fillColor = COLOR_RED;
    else if (percentage < 50) fillColor = COLOR_YELLOW;
    
    display.fillRect(x + 1, y + 3, fillWidth, 6, fillColor);
}

// Fonction pour afficher un graphique en barres simple
void drawSimpleBarChart(int x, int y, int width, int height, float values[], int count, uint16_t color) {
    if (count <= 0) return;
    
    // Trouver les valeurs min/max
    float minVal = values[0];
    float maxVal = values[0];
    for (int i = 1; i < count; i++) {
        if (values[i] < minVal) minVal = values[i];
        if (values[i] > maxVal) maxVal = values[i];
    }
    
    int barWidth = width / count;
    
    for (int i = 0; i < count; i++) {
        int barHeight = map(values[i], minVal, maxVal, 1, height);
        int barX = x + i * barWidth;
        int barY = y + height - barHeight;
        
        display.fillRect(barX, barY, barWidth - 1, barHeight, color);
        display.drawRect(barX, barY, barWidth - 1, barHeight, COLOR_WHITE);
    }
}

#endif