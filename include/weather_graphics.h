#ifndef WEATHER_GRAPHICS_H
#define WEATHER_GRAPHICS_H

#include <Adafruit_ST7789.h>

// Couleurs Flat Design (Plus douces)
#define FLAT_YELLOW   0xFFE0
#define FLAT_ORANGE   0xFD20
#define FLAT_WHITE    0xFFFF
#define FLAT_GREY     0xBDF7
#define FLAT_DARKGREY 0x7BEF
#define FLAT_BLUE     0x4D19 
#define FLAT_SKY      0x001F
// Ajout des couleurs pour les tendances
#define FLAT_GREEN    0x0605  // Vert "Flat"
#define FLAT_RED      0xF945  // Rouge "Flat"

// Helper pour dessiner un nuage de base
void drawBaseCloud(Adafruit_ST7789 &tft, int x, int y, uint16_t color) {
    tft.fillCircle(x, y, 14, color);       
    tft.fillCircle(x - 12, y + 4, 10, color); 
    tft.fillCircle(x + 12, y + 2, 12, color); 
    tft.fillRect(x - 12, y + 4, 25, 10, color);
}

// 1. Soleil (Clear)
void drawSun(Adafruit_ST7789 &tft, int x, int y) {
    tft.fillCircle(x, y, 18, FLAT_ORANGE);
    tft.fillCircle(x, y, 14, FLAT_YELLOW);
    for (int i = 0; i < 8; i++) {
        float angle = i * 45 * 3.14 / 180;
        int x1 = x + cos(angle) * 22;
        int y1 = y + sin(angle) * 22;
        int x2 = x + cos(angle) * 28;
        int y2 = y + sin(angle) * 28;
        tft.drawLine(x1, y1, x2, y2, FLAT_YELLOW);
    }
}

// 2. Nuageux (Cloudy)
void drawCloud(Adafruit_ST7789 &tft, int x, int y) {
    drawBaseCloud(tft, x, y, FLAT_GREY);
    drawBaseCloud(tft, x+3, y-3, FLAT_WHITE); 
}

// 3. Eclaircies (Partly Cloudy)
void drawPartlyCloudy(Adafruit_ST7789 &tft, int x, int y) {
    tft.fillCircle(x + 10, y - 10, 12, FLAT_YELLOW);
    drawBaseCloud(tft, x - 5, y + 5, FLAT_WHITE);
}

// 4. Pluie (Rain)
void drawRain(Adafruit_ST7789 &tft, int x, int y) {
    drawBaseCloud(tft, x, y, FLAT_GREY);
    tft.drawLine(x-5, y+15, x-5, y+22, FLAT_BLUE);
    tft.drawLine(x, y+15, x, y+25, FLAT_BLUE);
    tft.drawLine(x+5, y+15, x+5, y+22, FLAT_BLUE);
}

// 5. Orage (Storm)
void drawStorm(Adafruit_ST7789 &tft, int x, int y) {
    drawBaseCloud(tft, x, y, FLAT_DARKGREY);
    int lx = x; int ly = y + 10;
    tft.fillTriangle(lx, ly, lx-5, ly+15, lx+2, ly+15, FLAT_YELLOW);
    tft.fillTriangle(lx+2, ly+15, lx-3, ly+25, lx+5, ly+15, FLAT_YELLOW);
}

// 6. Neige (Snow)
void drawSnow(Adafruit_ST7789 &tft, int x, int y) {
    drawBaseCloud(tft, x, y, FLAT_GREY);
    tft.fillCircle(x-8, y+18, 2, FLAT_WHITE);
    tft.fillCircle(x, y+22, 2, FLAT_WHITE);
    tft.fillCircle(x+8, y+18, 2, FLAT_WHITE);
}

// 7. Brouillard (Fog)
void drawFog(Adafruit_ST7789 &tft, int x, int y) {
    tft.fillRoundRect(x-15, y-5, 30, 4, 2, FLAT_GREY);
    tft.fillRoundRect(x-20, y+2, 40, 4, 2, FLAT_GREY);
    tft.fillRoundRect(x-15, y+9, 30, 4, 2, FLAT_GREY);
}

// === NOUVELLE FONCTION AJOUTÉE (FIX v2.5.1) ===
// Dessine une flèche de tendance (1=Hausse, -1=Baisse, 0=Stable)
void drawTrendArrow(Adafruit_ST7789 &tft, int x, int y, int trend) {
    if (trend == 1) { // Hausse (Flèche verte vers haut-droite)
        tft.drawLine(x-8, y+8, x+8, y-8, FLAT_GREEN);
        tft.drawLine(x-7, y+9, x+9, y-7, FLAT_GREEN); // Epaisseur
        // Pointe
        tft.drawLine(x+8, y-8, x+8, y+2, FLAT_GREEN);
        tft.drawLine(x+8, y-8, x-2, y-8, FLAT_GREEN);
    } 
    else if (trend == -1) { // Baisse (Flèche rouge vers bas-droite)
        tft.drawLine(x-8, y-8, x+8, y+8, FLAT_RED);
        tft.drawLine(x-7, y-9, x+9, y+7, FLAT_RED); // Epaisseur
        // Pointe
        tft.drawLine(x+8, y+8, x+8, y-2, FLAT_RED);
        tft.drawLine(x+8, y+8, x-2, y+8, FLAT_RED);
    } 
    else { // Stable (Flèche grise horizontale)
        tft.drawLine(x-10, y, x+10, y, FLAT_GREY);
        tft.drawLine(x-10, y+1, x+10, y+1, FLAT_GREY); // Epaisseur
        // Pointe
        tft.drawLine(x+10, y, x+5, y-5, FLAT_GREY);
        tft.drawLine(x+10, y, x+5, y+5, FLAT_GREY);
    }
}

// FONCTION MAITRESSE
void drawWeatherByCode(Adafruit_ST7789 &tft, int x, int y, int code) {
    if (code == 0) drawSun(tft, x, y);
    else if (code == 1 || code == 2) drawPartlyCloudy(tft, x, y);
    else if (code == 3) drawCloud(tft, x, y);
    else if (code == 45 || code == 48) drawFog(tft, x, y);
    else if (code >= 51 && code <= 67) drawRain(tft, x, y);
    else if (code >= 71 && code <= 77) drawSnow(tft, x, y);
    else if (code >= 80 && code <= 82) drawRain(tft, x, y);
    else if (code >= 85 && code <= 86) drawSnow(tft, x, y);
    else if (code >= 95) drawStorm(tft, x, y);
    else drawCloud(tft, x, y);
}

#endif