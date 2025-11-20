// features.h - Nouvelles fonctionnalités avancées - VERSION TFT ST7789 CORRIGEE
#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <vector>
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

// Déclarations anticipées des fonctions externes
extern void sendTelegramMessage(const String &text);
extern void beep(int count);
extern const char* getWeatherLabel(int code);
extern Adafruit_ST7789 display;

// Déclaration anticipée de showAlertOnScreen
void showAlertOnScreen(String alertName, float value);

// ===================================
// STRUCTURES DE DONNEES
// ===================================

// Structure pour les alertes personnalisées
struct AlertRule {
  bool active;
  String type;     // "temp", "hum", "lux", "aqi"
  String op;       // ">", "<", "="
  float value;
  String name;     // Nom de l'alerte
  unsigned long lastTriggered;
  bool triggered;  // Pour éviter le spam
};

// Structure pour l'historique
struct Measurement {
  float temp;
  float humidity;
  int light;
  int aqi;
  unsigned long timestamp;
};

// Structure pour les prévisions
struct Forecast {
  int day;         // 0=aujourd'hui, 1=demain, etc.
  int tempMin;
  int tempMax;
  int weatherCode;
  String description;
  int precipProb;  // Probabilité de précipitation en %
};

// Structure pour la qualité de l'air
struct AirQuality {
  int aqi;         // 1-5 (Bon à Très mauvais)
  float pm25;
  float pm10;
  float no2;
  float o3;
  float co;
  String mainPollutant;
  String recommendation;
};

// ===================================
// VARIABLES GLOBALES
// ===================================

// Alertes (max 10)
AlertRule alerts[10];
int activeAlertCount = 0;

// Historique circulaire (144 mesures = 24h avec une mesure/10min)
Measurement history[144];
int historyIndex = 0;
int historyCount = 0;
unsigned long lastHistorySave = 0;
const unsigned long historyInterval = 10 * 60 * 1000; // 10 minutes

// Prévisions
Forecast forecasts[5]; // 5 jours
bool forecastsAvailable = false;

// Qualité de l'air
AirQuality airQuality;
bool airQualityAvailable = false;
unsigned long lastAirQualityUpdate = 0;

// ===================================
// FONCTIONS ALERTES PERSONNALISEES
// ===================================

void initAlerts() {
  for (int i = 0; i < 10; i++) {
    alerts[i].active = false;
    alerts[i].triggered = false;
    alerts[i].lastTriggered = 0;
  }
  Serial.println("[Features] Système d'alertes initialisé");
}

bool parseAlert(String command, AlertRule &alert) {
  command.trim();
  int spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) return false;
  
  alert.type = command.substring(0, spaceIndex);
  command = command.substring(spaceIndex + 1);
  command.trim();
  
  // Trouver l'opérateur
  if (command.startsWith(">")) {
    alert.op = ">";
    command = command.substring(1);
  } else if (command.startsWith("<")) {
    alert.op = "<";
    command = command.substring(1);
  } else if (command.startsWith("=")) {
    alert.op = "=";
    command = command.substring(1);
  } else {
    return false;
  }
  
  command.trim();
  spaceIndex = command.indexOf(' ');
  if (spaceIndex == -1) {
    alert.value = command.toFloat();
    alert.name = alert.type + alert.op + String(alert.value);
  } else {
    alert.value = command.substring(0, spaceIndex).toFloat();
    alert.name = command.substring(spaceIndex + 1);
  }
  
  alert.active = true;
  alert.triggered = false;
  alert.lastTriggered = 0;
  
  return true;
}

bool addAlert(String command) {
  for (int i = 0; i < 10; i++) {
    if (!alerts[i].active) {
      if (parseAlert(command, alerts[i])) {
        activeAlertCount++;
        Serial.println("[Alertes] Nouvelle alerte: " + alerts[i].name);
        return true;
      }
      return false;
    }
  }
  return false; // Plus de place
}

void clearAlerts() {
  for (int i = 0; i < 10; i++) {
    alerts[i].active = false;
  }
  activeAlertCount = 0;
  Serial.println("[Alertes] Toutes les alertes supprimées");
}

String listAlerts() {
  if (activeAlertCount == 0) {
    return "Aucune alerte active";
  }
  
  String list = "🔔 Alertes actives (" + String(activeAlertCount) + "/10):\n";
  for (int i = 0; i < 10; i++) {
    if (alerts[i].active) {
      list += "• " + alerts[i].name + " (" + alerts[i].type + alerts[i].op + alerts[i].value + ")";
      if (alerts[i].triggered) {
        list += " ⚠️";
      }
      list += "\n";
    }
  }
  return list;
}

void checkAlerts() {
  for (int i = 0; i < 10; i++) {
    if (!alerts[i].active) continue;
    
    bool shouldTrigger = false;
    float currentValue = 0;
    
    // Récupérer la valeur actuelle
    if (alerts[i].type == "temp") {
      currentValue = dhtTemp;
    } else if (alerts[i].type == "hum") {
      currentValue = dhtHum;
    } else if (alerts[i].type == "lux") {
      currentValue = lightLevel;
    } else if (alerts[i].type == "aqi" && airQualityAvailable) {
      currentValue = airQuality.aqi;
    } else {
      continue;
    }
    
    // Vérifier la condition
    if (alerts[i].op == ">") {
      shouldTrigger = currentValue > alerts[i].value;
    } else if (alerts[i].op == "<") {
      shouldTrigger = currentValue < alerts[i].value;
    } else if (alerts[i].op == "=") {
      shouldTrigger = abs(currentValue - alerts[i].value) < 0.5;
    }
    
    // Déclencher l'alerte si nécessaire (avec anti-spam de 30 min)
    if (shouldTrigger && !alerts[i].triggered) {
      if (millis() - alerts[i].lastTriggered > 30 * 60 * 1000) {
        String msg = "⚠️ ALERTE: " + alerts[i].name + "\n";
        msg += "Valeur actuelle: " + String(currentValue);
        sendTelegramMessage(msg);
        
        alerts[i].triggered = true;
        alerts[i].lastTriggered = millis();
        
        // Bip d'alerte
        beep(5);
        
        // Affichage visuel d'alerte sur l'écran TFT
        showAlertOnScreen(alerts[i].name, currentValue);
      }
    } else if (!shouldTrigger && alerts[i].triggered) {
      alerts[i].triggered = false; // Réinitialiser quand la condition n'est plus vraie
    }
  }
}

// Nouvelle fonction pour afficher une alerte sur l'écran TFT
void showAlertOnScreen(String alertName, float value) {
  // Sauvegarder l'état actuel de l'écran
  display.fillScreen(COLOR_BLACK);
  
  // Bordure rouge clignotante
  for (int i = 0; i < 3; i++) {
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_RED);
    display.drawRect(1, 1, SCREEN_WIDTH-2, SCREEN_HEIGHT-2, COLOR_RED);
    delay(200);
    display.drawRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_BLACK);
    display.drawRect(1, 1, SCREEN_WIDTH-2, SCREEN_HEIGHT-2, COLOR_BLACK);
    delay(200);
  }
  
  // Affichage du message d'alerte
  display.fillScreen(COLOR_RED);
  display.setTextSize(2);
  display.setTextColor(COLOR_WHITE);
  
  int16_t x1, y1;
  uint16_t w, h;
  String title = "ALERTE!";
  display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 30);
  display.println(title);
  
  display.setTextSize(1);
  display.setCursor(5, 70);
  display.println(alertName);
  
  display.setCursor(5, 90);
  display.print("Valeur: ");
  display.println(value, 1);
  
  display.setTextColor(COLOR_YELLOW);
  display.setCursor(5, 120);
  display.println("Appuyez sur le bouton");
  display.setCursor(5, 135);
  display.println("pour continuer");
  
  delay(3000); // Afficher pendant 3 secondes
}

// ===================================
// FONCTIONS HISTORIQUE ET GRAPHIQUES
// ===================================

void saveToHistory() {
  if (millis() - lastHistorySave < historyInterval) {
    return; // Pas encore le moment
  }
  
  history[historyIndex].temp = dhtTemp;
  history[historyIndex].humidity = dhtHum;
  history[historyIndex].light = lightLevel;
  history[historyIndex].aqi = airQualityAvailable ? airQuality.aqi : 0;
  history[historyIndex].timestamp = millis();
  
  historyIndex = (historyIndex + 1) % 144;
  if (historyCount < 144) historyCount++;
  
  lastHistorySave = millis();
  Serial.println("[History] Mesure sauvegardée (index=" + String(historyIndex) + ")");
}

String getHistory(int hours) {
  if (historyCount == 0) {
    return "Historique vide";
  }
  
  int samples = min((hours * 6), historyCount); // 6 mesures par heure
  String result = "📊 Historique " + String(hours) + "h:\n\n";
  
  int startIdx = (historyIndex - samples + 144) % 144;
  
  for (int i = 0; i < samples; i++) {
    int idx = (startIdx + i) % 144;
    unsigned long age = (millis() - history[idx].timestamp) / 60000; // en minutes
    
    result += "Il y a " + String(age) + "min: ";
    result += String(history[idx].temp, 1) + "°C ";
    result += String(history[idx].humidity, 0) + "% ";
    result += String(history[idx].light) + "lux";
    if (history[idx].aqi > 0) {
      result += " AQI:" + String(history[idx].aqi);
    }
    result += "\n";
  }
  
  return result;
}

String getStats() {
  if (historyCount == 0) {
    return "Pas assez de données";
  }
  
  float minTemp = 100, maxTemp = -100, avgTemp = 0;
  float minHum = 100, maxHum = 0, avgHum = 0;
  int minLight = 9999, maxLight = 0, avgLight = 0;
  
  for (int i = 0; i < historyCount; i++) {
    minTemp = min(minTemp, history[i].temp);
    maxTemp = max(maxTemp, history[i].temp);
    avgTemp += history[i].temp;
    
    minHum = min(minHum, history[i].humidity);
    maxHum = max(maxHum, history[i].humidity);
    avgHum += history[i].humidity;
    
    minLight = min(minLight, history[i].light);
    maxLight = max(maxLight, history[i].light);
    avgLight += history[i].light;
  }
  
  avgTemp /= historyCount;
  avgHum /= historyCount;
  avgLight /= historyCount;
  
  String stats = "📈 Statistiques du jour:\n\n";
  stats += "🌡️ Température:\n";
  stats += "  Min: " + String(minTemp, 1) + "°C\n";
  stats += "  Max: " + String(maxTemp, 1) + "°C\n";
  stats += "  Moy: " + String(avgTemp, 1) + "°C\n\n";
  
  stats += "💧 Humidité:\n";
  stats += "  Min: " + String(minHum, 0) + "%\n";
  stats += "  Max: " + String(maxHum, 0) + "%\n";
  stats += "  Moy: " + String(avgHum, 0) + "%\n\n";
  
  stats += "💡 Luminosité:\n";
  stats += "  Min: " + String(minLight) + "\n";
  stats += "  Max: " + String(maxLight) + "\n";
  stats += "  Moy: " + String(avgLight);
  
  return stats;
}

String drawASCIIGraph(String type) {
  if (historyCount < 2) {
    return "Pas assez de données pour un graphique";
  }
  
  const int graphWidth = 20;
  const int graphHeight = 10;
  String graph = "📉 Graphique " + type + " (24h):\n```\n";
  
  // Obtenir les données
  float values[graphWidth];
  float minVal = 9999, maxVal = -9999;
  
  int step = max(1, historyCount / graphWidth);
  for (int i = 0; i < graphWidth && i * step < historyCount; i++) {
    int idx = (historyIndex - historyCount + i * step + 144) % 144;
    
    if (type == "temp") {
      values[i] = history[idx].temp;
    } else if (type == "hum") {
      values[i] = history[idx].humidity;
    } else {
      values[i] = history[idx].light;
    }
    
    minVal = min(minVal, values[i]);
    maxVal = max(maxVal, values[i]);
  }
  
  // Dessiner le graphique
  for (int y = graphHeight; y >= 0; y--) {
    if (y == graphHeight) {
      graph += String(maxVal, 1) + " ";
    } else if (y == 0) {
      graph += String(minVal, 1) + " ";
    } else {
      graph += "    ";
    }
    
    for (int x = 0; x < graphWidth; x++) {
      float threshold = minVal + (maxVal - minVal) * y / graphHeight;
      if (values[x] >= threshold) {
        graph += "█";
      } else {
        graph += " ";
      }
    }
    graph += "\n";
  }
  
  graph += "    └─────────────────────────\n";
  graph += "    -24h            Maintenant\n";
  graph += "```";
  
  return graph;
}

// ===================================
// FONCTIONS PREVISIONS ETENDUES
// ===================================

void updateForecasts() {
  if (!isConnected) return;
  
  HTTPClient http;
  String url = "https://api.meteo-concept.com/api/forecast/daily?token=" + 
               METEOCONSULT_TOKEN + "&insee=" + dynamicInsee;
  
  http.begin(url);
  int httpCode = http.GET();
  
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (!error) {
      JsonArray dailyForecasts = doc["forecast"];
      
      for (int i = 0; i < 5 && i < dailyForecasts.size(); i++) {
        JsonObject day = dailyForecasts[i];
        forecasts[i].day = day["day"].as<int>();
        forecasts[i].tempMin = day["tmin"].as<int>();
        forecasts[i].tempMax = day["tmax"].as<int>();
        forecasts[i].weatherCode = day["weather"].as<int>();
        forecasts[i].precipProb = day["probarain"].as<int>();
        forecasts[i].description = getWeatherLabel(forecasts[i].weatherCode);
      }
      
      forecastsAvailable = true;
      Serial.println("[Forecasts] Prévisions mises à jour");
    }
  }
  
  http.end();
}

String getExtendedForecast() {
  if (!forecastsAvailable) {
    updateForecasts();
    if (!forecastsAvailable) {
      return "Prévisions non disponibles";
    }
  }
  
  String result = "☀️ Prévisions 5 jours:\n\n";
  const char* days[] = {"Auj", "Dem", "J+2", "J+3", "J+4"};
  
  for (int i = 0; i < 5; i++) {
    result += String(days[i]) + ": ";
    
    // Icône selon le code météo
    if (forecasts[i].weatherCode <= 2) result += "☀️";
    else if (forecasts[i].weatherCode <= 5) result += "☁️";
    else if (forecasts[i].weatherCode <= 11) result += "🌧️";
    else if (forecasts[i].weatherCode <= 14) result += "❄️";
    else if (forecasts[i].weatherCode == 15) result += "⛈️";
    else result += "🌤️";
    
    result += " " + String(forecasts[i].tempMin) + "/" + String(forecasts[i].tempMax) + "°C";
    
    if (forecasts[i].precipProb > 30) {
      result += " 💧" + String(forecasts[i].precipProb) + "%";
    }
    result += "\n";
    result += "  " + forecasts[i].description + "\n\n";
  }
  
  return result;
}

// ===================================
// FONCTIONS QUALITE DE L'AIR
// ===================================

void updateAirQuality() {
  if (!isConnected) return;
  if (millis() - lastAirQualityUpdate < 30 * 60 * 1000) return; // Mise à jour toutes les 30 min
  
  // Simulation de données pour test (remplacer par vraie API)
  airQuality.aqi = random(1, 4);
  airQuality.pm25 = random(10, 50);
  airQuality.pm10 = random(20, 80);
  airQuality.no2 = random(10, 40);
  airQuality.o3 = random(30, 100);
  airQuality.co = random(200, 800);
  
  // Déterminer le polluant principal
  float maxPollutant = max(airQuality.pm25, max(airQuality.pm10, max(airQuality.no2, airQuality.o3)));
  if (maxPollutant == airQuality.pm25) airQuality.mainPollutant = "PM2.5";
  else if (maxPollutant == airQuality.pm10) airQuality.mainPollutant = "PM10";
  else if (maxPollutant == airQuality.no2) airQuality.mainPollutant = "NO2";
  else airQuality.mainPollutant = "O3";
  
  // Recommandations selon l'AQI
  switch(airQuality.aqi) {
    case 1: 
      airQuality.recommendation = "Bon - Activités extérieures sans restriction";
      break;
    case 2:
      airQuality.recommendation = "Correct - OK pour la plupart des personnes";
      break;
    case 3:
      airQuality.recommendation = "Moyen - Personnes sensibles: limiter efforts prolongés";
      break;
    case 4:
      airQuality.recommendation = "Mauvais - Éviter efforts intenses à l'extérieur";
      break;
    case 5:
      airQuality.recommendation = "Très mauvais - Éviter toute activité extérieure";
      break;
  }
  
  airQualityAvailable = true;
  lastAirQualityUpdate = millis();
  Serial.println("[AirQuality] Données simulées - AQI: " + String(airQuality.aqi));
}

String getAirQualityReport() {
  if (!airQualityAvailable) {
    updateAirQuality();
    if (!airQualityAvailable) {
      return "Données qualité de l'air non disponibles";
    }
  }
  
  String report = "🌬️ Qualité de l'air:\n\n";
  
  // Icône selon l'AQI
  String aqiIcon;
  String aqiLabel;
  switch(airQuality.aqi) {
    case 1: aqiIcon = "🟢"; aqiLabel = "Bon"; break;
    case 2: aqiIcon = "🟡"; aqiLabel = "Correct"; break;
    case 3: aqiIcon = "🟠"; aqiLabel = "Moyen"; break;
    case 4: aqiIcon = "🔴"; aqiLabel = "Mauvais"; break;
    case 5: aqiIcon = "🟣"; aqiLabel = "Très mauvais"; break;
  }
  
  report += aqiIcon + " AQI: " + String(airQuality.aqi) + " - " + aqiLabel + "\n\n";
  
  report += "📊 Détails:\n";
  report += "• PM2.5: " + String(airQuality.pm25, 1) + " µg/m³\n";
  report += "• PM10: " + String(airQuality.pm10, 1) + " µg/m³\n";
  report += "• NO₂: " + String(airQuality.no2, 1) + " µg/m³\n";
  report += "• O₃: " + String(airQuality.o3, 1) + " µg/m³\n";
  report += "• CO: " + String(airQuality.co/1000, 1) + " mg/m³\n\n";
  
  report += "⚠️ Polluant principal: " + airQuality.mainPollutant + "\n\n";
  report += "💡 " + airQuality.recommendation;
  
  return report;
}

// ===================================
// FONCTION D'INITIALISATION
// ===================================

void initFeatures() {
  initAlerts();
  Serial.println("[Features] Module features.h chargé");
}