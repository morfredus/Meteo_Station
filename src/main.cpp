#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <AsyncTelegram2.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h> 
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPSPlus.h>
#include <TinyGPSPlus.h>
#include <time.h>

#include "config.h"
#include "web_page.h"
#include "weather_graphics.h"

// ================= OBJETS GLOBAUX =================
WiFiMulti wifiMulti;
AsyncWebServer server(80);
WiFiClientSecure clientSEC;
AsyncTelegram2 bot(clientSEC);

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;
Adafruit_NeoPixel pixels(NEOPIXEL_NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
TinyGPSPlus gps;

// ================= VARIABLES D'ETAT =================
struct SensorData {
  float temp = 0.0;
  float hum = 0.0;
  float pres = 0.0;
  int lux = 0;
  String trend = "→";
} localSensor;

float pressureHistory = 0.0;
unsigned long lastPressureTime = 0;
bool autoBrightnessMode = true;
bool bmpAvailable = false;  // Track si BMP280 est disponible 
bool ahtAvailable = false;  // Track si AHT20 est disponible
// Log helper for missing AHT20 to avoid spamming the serial
unsigned long lastAhtWarn = 0;
const unsigned long AHT_WARN_INTERVAL = 300000; // 5 minutes
// Store I2C scan results for the debug endpoint
uint8_t i2cDevices[32];
int i2cDeviceCount = 0;

struct WeatherData {
  float temp = 0.0;
  int weatherCode = 0;
  float pressureMSL = 1013.25;
  int aqi = 0; 
  float forecastMax[3] = {0,0,0};
  float forecastMin[3] = {0,0,0};
  int forecastCode[3] = {0,0,0};
  String provider = "Recherche..."; 
} apiWeather;

struct GPSData {
  double lat = 44.8378;
  double lon = -0.5792; 
  double alt = 0.0;
  double speed = 0.0;
  double course = 0.0;
  bool isValid = false;
  int sats = 0;
  String date = "--/--/--";
  String time = "--:--:--";
} currentGPS;

// First full status sent flag and stored chat id
bool firstFullStatusSent = false;
unsigned long telegramChatId = 0;

bool alertActive = false;
bool lastAlertActive = false; 
String alertMessage = "Aucune";
bool gpsFixNotified = false;

int currentPage = 0;
const int TOTAL_PAGES = 7; 

unsigned long lastTimeSensor = 0;
unsigned long lastTimeWeather = 0;
unsigned long lastDisplayUpdate = 0;

// ================= INTERRUPTIONS =================
volatile bool flagPagePressed = false;
volatile bool flagActionPressed = false;
unsigned long lastButtonActionTime = 0; 
const unsigned long REFRACTORY_PERIOD = 300; 

void IRAM_ATTR isrPageBtn() { flagPagePressed = true; }
void IRAM_ATTR isrActionBtn() { flagActionPressed = true; }

// ================= PROTOTYPES =================
void drawStartupScreen(); 
bool measurementsAvailable();
void initSensors();
void updateSensors();
void adjustBrightness();
void updatePressureTrend(float currentPres);
void fetchWeather();          
bool fetchOpenWeatherMap();   
bool fetchOpenMeteo();        
bool fetchAQI_OpenMeteo(); // MODIFIE : Source Open-Meteo
int mapOWMtoWMO(int owmId);   
void handleTelegram(); 
void sendTelegramFullStatus(unsigned long chatId);
void refreshDisplayData();
void drawFullPage();
void beep(int count, int duration);
void setNeoPixelStatus();
void handleButtonLogic();
void drawSunRays(int cx, int cy, int offsetAngle, uint16_t color);
String getUptime();
String cleanText(String s);
void showToastMessage(String msg, uint16_t color);

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial2.begin(GPS_BAUD_RATE, SERIAL_8N1, PIN_GPS_RXD, PIN_GPS_TXD);
  
  Serial.println("\n--- BOOTING STATION v" + String(PROJECT_VERSION) + " ---");

  pinMode(PIN_BUTTON_PAGE, INPUT_PULLUP);
  pinMode(PIN_BUTTON_ACTION, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_PAGE), isrPageBtn, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON_ACTION), isrActionBtn, FALLING);

  pinMode(PIN_BUZZER, OUTPUT);
  #ifndef BUZZER_LEDC_CHANNEL
    #define BUZZER_LEDC_CHANNEL 0
  #endif
  ledcSetup(BUZZER_LEDC_CHANNEL, 2000, 8);
  ledcAttachPin(PIN_BUZZER, BUZZER_LEDC_CHANNEL);

  #ifndef BACKLIGHT_LEDC_CHANNEL
    #define BACKLIGHT_LEDC_CHANNEL 1 
  #endif
  ledcSetup(BACKLIGHT_LEDC_CHANNEL, 12000, 8);
  ledcAttachPin(TFT_BL, BACKLIGHT_LEDC_CHANNEL);
  ledcWrite(BACKLIGHT_LEDC_CHANNEL, 255);

  // Config LDR (GPIO 4 pour ESP32-S3 recommandé)
  pinMode(PIN_LIGHT_SENSOR, INPUT);
  Serial.printf("LDR configure sur PIN: %d\n", PIN_LIGHT_SENSOR);
  Serial.printf("LDR configure sur PIN: %d\n", PIN_LIGHT_SENSOR);

  pixels.begin();
  pixels.setBrightness(NEOPIXEL_BRIGHTNESS);
  pixels.show();

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  // I2C bus scan: lister les adresses détectées pour debugging matériel
  Serial.println("Scanning I2C bus for devices...");
  i2cDeviceCount = 0;
  for (uint8_t addr = 1; addr < 127; ++addr) {
    Wire.beginTransmission(addr);
    uint8_t err = Wire.endTransmission();
    if (err == 0) {
      Serial.printf(" - Found I2C device at 0x%02X\n", addr);
      if (i2cDeviceCount < (int)(sizeof(i2cDevices))) {
        i2cDevices[i2cDeviceCount++] = addr;
      }
    }
  }
  Serial.println("I2C scan complete.");
  // Reset last warn to force an immediate message if AHT missing
  lastAhtWarn = 0 - AHT_WARN_INTERVAL;
  
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(C_BLACK);
  
  drawStartupScreen(); 
  initSensors();

  configTime(3600, 3600, "pool.ntp.org");

  clientSEC.setInsecure(); 
  bot.setUpdateTime(2000); 
  bot.setTelegramToken(TELEGRAM_BOT_TOKEN);
  
  if (bot.begin()) {
      unsigned long chat = atol(TELEGRAM_CHAT_ID);
      telegramChatId = chat;
      bot.sendTo(chat, "Station v" + String(PROJECT_VERSION) + " demarre. En attente des mesures pour le premier rapport...");
  }
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc; 
    doc["sensor"]["temp"] = localSensor.temp;
    doc["sensor"]["hum"] = localSensor.hum;
    doc["sensor"]["pres"] = localSensor.pres;
    doc["sensor"]["lux"] = localSensor.lux;
    doc["sensor"]["trend"] = localSensor.trend;
    doc["weather"]["temp"] = apiWeather.temp;
    doc["weather"]["code"] = apiWeather.weatherCode;
    doc["weather"]["aqi"] = apiWeather.aqi;
    doc["weather"]["provider"] = apiWeather.provider;
    
    JsonArray f = doc["forecast"].to<JsonArray>();
    for(int i=0; i<3; i++) {
        JsonObject day = f.add<JsonObject>();
        day["min"] = apiWeather.forecastMin[i];
        day["max"] = apiWeather.forecastMax[i];
    }
    
    doc["gps"]["lat"] = currentGPS.lat;
    doc["gps"]["lon"] = currentGPS.lon;
    doc["gps"]["valid"] = currentGPS.isValid;
    doc["gps"]["sats"] = currentGPS.sats;
    doc["sys"]["uptime"] = getUptime();
    doc["sys"]["backlight"] = autoBrightnessMode ? "Auto" : "Max";
    doc["alert"] = alertActive;
    
    String response; serializeJson(doc, response);
    request->send(200, "application/json", response);
  });

  // Debug endpoint: retourne l'état des capteurs et la liste d'adresses I2C détectées
  server.on("/api/debug", HTTP_GET, [](AsyncWebServerRequest *request){
    DynamicJsonDocument doc(1024);
    doc["version"] = PROJECT_VERSION;
    doc["ahtAvailable"] = ahtAvailable;
    doc["bmpAvailable"] = bmpAvailable;
    doc["i2c_count"] = i2cDeviceCount;
    JsonArray arr = doc.createNestedArray("i2c_devices");
    for (int i = 0; i < i2cDeviceCount; i++) arr.add(i2cDevices[i]);
    JsonObject pins = doc.createNestedObject("pins");
    pins["sda"] = PIN_I2C_SDA;
    pins["scl"] = PIN_I2C_SCL;

    String response; serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  server.begin();

  Serial.println("\n=== Initial GPS Coordinates ===");
  Serial.printf("GPS Valid: %s\n", currentGPS.isValid ? "Yes" : "No (using defaults)");
  Serial.printf("Lat: %.4f, Lon: %.4f\n", currentGPS.lat, currentGPS.lon);
  Serial.println("===============================\n");

  Serial.println("\n=== Initial Weather Fetch ===");
  Serial.printf("WiFi Status: %s\n", WiFi.status() == WL_CONNECTED ? "Connected" : "Not Connected");
  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("WiFi SSID: %s\n", WiFi.SSID().c_str());
    Serial.printf("WiFi IP: %s\n", WiFi.localIP().toString().c_str());
  }
  Serial.println("Calling fetchWeather()...");
  fetchWeather();
  Serial.println("fetchWeather() completed\n");

  Serial.println("Calling updateSensors()...");
  updateSensors();
  Serial.println("updateSensors() completed\n");

  tft.fillScreen(C_BLACK);
  drawFullPage();
}

// ================= LOOP =================
void loop() {
  unsigned long now = millis();

  while (Serial2.available() > 0) {
    gps.encode(Serial2.read());
  }

  if (gps.location.isUpdated()) {
    currentGPS.lat = gps.location.lat();
    currentGPS.lon = gps.location.lng();
    currentGPS.alt = gps.altitude.meters();
    currentGPS.speed = gps.speed.kmph();
    currentGPS.course = gps.course.deg();
    currentGPS.sats = gps.satellites.value();
    currentGPS.isValid = true;
    
    if (!gpsFixNotified && currentGPS.isValid) {
        beep(3, 80); 
        gpsFixNotified = true;
    }
    char buffDate[12]; sprintf(buffDate, "%02d/%02d/%02d", gps.date.day(), gps.date.month(), gps.date.year());
    currentGPS.date = String(buffDate);
    char buffTime[10]; sprintf(buffTime, "%02d:%02d:%02d", gps.time.hour(), gps.time.minute(), gps.time.second());
    currentGPS.time = String(buffTime);
  }

  handleButtonLogic();

  if (now - lastTimeSensor > INTERVAL_SENSORS) {
    updateSensors();
    adjustBrightness();
    setNeoPixelStatus();
    refreshDisplayData();
    lastTimeSensor = now;
  }

  // Après la première lecture des capteurs / météo, envoyer le premier rapport complet
  if (!firstFullStatusSent && telegramChatId != 0 && measurementsAvailable()) {
    sendTelegramFullStatus(telegramChatId);
    firstFullStatusSent = true;
  }

  if (now - lastTimeWeather > INTERVAL_WEATHER) {
    fetchWeather();
    refreshDisplayData();
    lastTimeWeather = now;
  }

  handleTelegram(); 

  if (now - lastDisplayUpdate > 1000) {
    refreshDisplayData();
    lastDisplayUpdate = now;
  }
}

// ================= LOGIQUE METIER =================

String cleanText(String s) {
    s.replace("è", "e"); s.replace("é", "e"); s.replace("ê", "e"); s.replace("ë", "e");
    s.replace("à", "a"); s.replace("â", "a");
    s.replace("î", "i"); s.replace("ï", "i");
    s.replace("ô", "o");
    s.replace("ù", "u"); s.replace("û", "u");
    s.replace("ç", "c");
    s.replace("Ç", "C"); s.replace("È", "E"); s.replace("É", "E");
    s.replace("À", "A"); s.replace("Â", "A");
    s.replace("Ô", "O"); s.replace("Ù", "U"); s.replace("Û", "U");
    s.replace("÷", String((char)247)); 
    return s;
}

  bool measurementsAvailable() {
    // Consider measurements available if any sensor/weather/GPS provides non-default data
    if (currentGPS.isValid) return true;
    if (!isnan(localSensor.temp) && localSensor.hum > 0) return true;
    if (!isnan(localSensor.pres) && localSensor.pres > 0) return true;
    if (apiWeather.temp != 0.0) return true;
    if (localSensor.lux > 0) return true;
    return false;
  }

void adjustBrightness() {
    if (autoBrightnessMode) {
        int pwmVal = map(localSensor.lux, 0, 4095, 10, 255);
        if (pwmVal < 10) pwmVal = 10;
        if (pwmVal > 255) pwmVal = 255;
        ledcWrite(BACKLIGHT_LEDC_CHANNEL, pwmVal);
    } else {
        ledcWrite(BACKLIGHT_LEDC_CHANNEL, 255);
    }
}

void updatePressureTrend(float currentPres) {
    if (pressureHistory == 0.0) {
        pressureHistory = currentPres;
        lastPressureTime = millis();
        return;
    }
    if (millis() - lastPressureTime > 3600000) {
        float diff = currentPres - pressureHistory;
        if (diff > 0.5) localSensor.trend = "Hausse";
        else if (diff < -0.5) localSensor.trend = "Baisse";
        else localSensor.trend = "Stable";
        pressureHistory = currentPres;
        lastPressureTime = millis();
    }
}

void showToastMessage(String msg, uint16_t color) {
    int boxW = 200; int boxH = 40;
    int boxX = (240 - boxW) / 2;
    int boxY = 50; 
    tft.fillRoundRect(boxX, boxY, boxW, boxH, 8, C_WHITE);
    tft.drawRoundRect(boxX, boxY, boxW, boxH, 8, color);
    tft.setTextColor(C_BLACK); tft.setTextSize(2);
    int textX = boxX + (boxW - (msg.length() * 12)) / 2;
    tft.setCursor(textX, boxY + 12);
    tft.print(msg);
    delay(1000);
    tft.fillRect(boxX, boxY, boxW, boxH, C_BLACK);
    drawFullPage(); refreshDisplayData();
}

// Send a complete formatted status to a chat id
void sendTelegramFullStatus(unsigned long chatId) {
  String msg = "Station Meteo - Status v" + String(PROJECT_VERSION) + "\n\n";
  msg += "-- Sensors (local) --\n";
  msg += "Temp: " + String(localSensor.temp, 1) + " C\n";
  msg += "Hum: " + String(localSensor.hum, 0) + " %\n";
  if (isnan(localSensor.pres)) msg += "Pres: N/A\n";
  else msg += "Pres: " + String(localSensor.pres, 0) + " hPa\n";
  msg += "Lux: " + String(localSensor.lux) + " lx\n";
  msg += "Trend: " + localSensor.trend + "\n\n";

  msg += "-- Weather (api) --\n";
  msg += "Outside Temp: " + String(apiWeather.temp, 1) + " C\n";
  msg += "Weather Code: " + String(apiWeather.weatherCode) + "\n";
  msg += "AQI (1-5): " + String(apiWeather.aqi) + "\n";
  msg += "Provider: " + apiWeather.provider + "\n\n";

  msg += "-- Forecast (3 days) --\n";
  for (int i = 0; i < 3; i++) {
    msg += "Day " + String(i+1) + ": ";
    msg += "Min " + String(apiWeather.forecastMin[i], 0) + "C ";
    msg += "Max " + String(apiWeather.forecastMax[i], 0) + "C ";
    msg += "Code " + String(apiWeather.forecastCode[i]) + "\n";
  }
  msg += "\n";

  msg += "-- GPS --\n";
  if (currentGPS.isValid) {
    msg += "Lat: " + String(currentGPS.lat, 4) + " Lon: " + String(currentGPS.lon, 4) + "\n";
    msg += "Alt: " + String(currentGPS.alt, 1) + " m Speed: " + String(currentGPS.speed, 1) + " km/h\n";
    msg += "Sats: " + String(currentGPS.sats) + "\n";
  } else {
    msg += "GPS: No fix (using default)\n";
  }
  msg += "\n";

  msg += "-- System --\n";
  msg += "Uptime: " + getUptime() + "\n";
  msg += "Free RAM: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
  msg += "WiFi SSID: " + WiFi.SSID() + "\n";
  msg += "IP: " + WiFi.localIP().toString() + "\n";

  bot.sendTo(chatId, msg);
}

// Extended Telegram command handler
void handleTelegram() {
  TBMessage msg;
  if (bot.getNewMessage(msg)) {
    if (String(msg.chatId) != TELEGRAM_CHAT_ID) return;
    String text = msg.text;
    if (text.equalsIgnoreCase("/start")) {
      bot.sendMessage(msg, "Station v" + String(PROJECT_VERSION) + " ready. Use /help for commands.");
    }
    else if (text.equalsIgnoreCase("/help") || text.equalsIgnoreCase("/commands")) {
      String h = "/help - list commands\n";
      h += "/status - short sensor & weather\n";
      h += "/getreport - full status report now\n";
      h += "/weather - current + 3-day forecast\n";
      h += "/system - uptime, RAM, WiFi, IP\n";
      h += "/gps - current GPS fix data\n";
      h += "/alerts - current alerts\n";
      h += "/reboot - reboot device\n";
      bot.sendMessage(msg, h);
    }
    else if (text.equalsIgnoreCase("/status")) {
      String s = "Int Temp: " + String(localSensor.temp,1) + "C\n";
      s += "Ext Temp: " + String(apiWeather.temp,1) + "C\n";
      s += "AQI: " + String(apiWeather.aqi) + "\n";
      s += "Lux: " + String(localSensor.lux) + " lx\n";
      bot.sendMessage(msg, s);
    }
    else if (text.equalsIgnoreCase("/getreport") || text.equalsIgnoreCase("/get")) {
      // Send a full status on demand
      sendTelegramFullStatus(msg.chatId);
    }
    else if (text.equalsIgnoreCase("/weather")) {
      String w = "Weather Provider: " + apiWeather.provider + "\n";
      w += "Now: " + String(apiWeather.temp,1) + " C, Code " + String(apiWeather.weatherCode) + "\n";
      w += "Forecast:\n";
      for (int i=0;i<3;i++) w += "Day " + String(i+1) + ": Min " + String(apiWeather.forecastMin[i],0) + "C Max " + String(apiWeather.forecastMax[i],0) + "C Code " + String(apiWeather.forecastCode[i]) + "\n";
      bot.sendMessage(msg, w);
    }
    else if (text.equalsIgnoreCase("/system")) {
      String sy = "Uptime: " + getUptime() + "\n";
      sy += "Free RAM: " + String(ESP.getFreeHeap() / 1024) + " KB\n";
      sy += "WiFi: " + WiFi.SSID() + " (RSSI " + String(WiFi.RSSI()) + " dBm)\n";
      sy += "IP: " + WiFi.localIP().toString() + "\n";
      bot.sendMessage(msg, sy);
    }
    else if (text.equalsIgnoreCase("/gps")) {
      if (currentGPS.isValid) {
        String g = "Lat: " + String(currentGPS.lat,4) + " Lon: " + String(currentGPS.lon,4) + "\n";
        g += "Alt: " + String(currentGPS.alt,1) + " m Speed: " + String(currentGPS.speed,1) + " km/h\n";
        g += "Sats: " + String(currentGPS.sats) + "\n";
        bot.sendMessage(msg, g);
      } else bot.sendMessage(msg, "GPS: No fix available");
    }
    else if (text.equalsIgnoreCase("/alerts")) {
      if (alertActive) bot.sendMessage(msg, "ALERT: " + alertMessage);
      else bot.sendMessage(msg, "No alerts");
    }
    else if (text.equalsIgnoreCase("/reboot")) {
      bot.sendMessage(msg, "Rebooting..."); delay(200); ESP.restart();
    }
  }
}

void handleButtonLogic() {
  if (millis() - lastButtonActionTime < REFRACTORY_PERIOD) {
      flagPagePressed = false; flagActionPressed = false; return; 
  }
  if (flagPagePressed) {
    beep(1, 50);
    flagPagePressed = false;
    lastButtonActionTime = millis();
    currentPage++;
    if (currentPage >= TOTAL_PAGES) currentPage = 0;
    drawFullPage(); refreshDisplayData();
  }
  if (flagActionPressed) {
    flagActionPressed = false;
    lastButtonActionTime = millis();
    autoBrightnessMode = !autoBrightnessMode;
    if(autoBrightnessMode) { beep(1, 100); showToastMessage("LUM: AUTO", C_BLUE); } 
    else { beep(2, 80); showToastMessage("LUM: MAX", C_ORANGE); }
    adjustBrightness();
  }
}

String getUptime() {
    unsigned long sec = millis() / 1000;
    unsigned long min = sec / 60;
    unsigned long hr = min / 60;
    unsigned long day = hr / 24;
    sec %= 60; min %= 60; hr %= 24;
    char buf[20]; sprintf(buf, "%dd %02dh %02dm", day, hr, min);
    return String(buf);
}

void fetchWeather() {
    if (wifiMulti.run() != WL_CONNECTED) {
        Serial.println("WiFi not connected, skipping weather update");
        return;
    }

    tft.fillRect(210, 5, 20, 20, C_BLUE);
    tft.setCursor(215, 5); tft.setTextColor(C_YELLOW); tft.setTextSize(1); tft.print("DL");

    Serial.println("\n==============================");
    Serial.println("=== Fetching Weather Data ===");
    Serial.println("==============================");

    // PRIORITE 1: OpenWeatherMap (pour les alertes et données complètes)
    bool successOWM = fetchOpenWeatherMap();
    if (successOWM) {
        apiWeather.provider = "OpenWeatherMap";
        Serial.println("✓ OpenWeatherMap: SUCCESS");
    } else {
        Serial.println("✗ OpenWeatherMap: FAILED");
        Serial.println("Trying fallback: Open-Meteo...");
        if (fetchOpenMeteo()) {
            apiWeather.provider = "Open-Meteo";
            Serial.println("✓ Open-Meteo: SUCCESS (fallback)");
        } else {
            apiWeather.provider = "Erreur Reseau";
            Serial.println("✗ Open-Meteo: FAILED");
        }
    }

    // APPEL DE LA FONCTION AQI (OPEN METEO)
    Serial.println("--- Fetching AQI (Open-Meteo) ---");
    fetchAQI_OpenMeteo();

    Serial.printf("Final provider: %s\n", apiWeather.provider.c_str());
    Serial.println("==============================\n");

    tft.fillRect(210, 5, 20, 20, C_BLUE);
}

bool fetchOpenWeatherMap() {
    HTTPClient http; http.setTimeout(4000);

    // Utiliser coordonnées GPS ou coordonnées par défaut
    float latToUse = currentGPS.isValid ? currentGPS.lat : 44.8378;
    float lonToUse = currentGPS.isValid ? currentGPS.lon : -0.5792;

    Serial.println("=== OpenWeatherMap Request ===");
    Serial.printf("Coordinates: %.4f, %.4f (GPS: %s)\n", latToUse, lonToUse, currentGPS.isValid ? "Valid" : "Default");

    // ========================================
    // PARTIE 1: Météo actuelle (weather API)
    // ========================================
    String urlWeather = String("http://api.openweathermap.org/data/2.5/weather?lat=") +
                        String(latToUse, 4) + "&lon=" + String(lonToUse, 4) +
                        "&units=metric&appid=" + OPENWEATHER_API_KEY;

    Serial.println("Weather URL: " + urlWeather);
    http.begin(urlWeather);
    int codeWeather = http.GET();
    Serial.printf("Weather HTTP Code: %d\n", codeWeather);

    if (codeWeather == 200) {
        String payload = http.getString();
        Serial.printf("Weather Payload: %d bytes\n", payload.length());

        JsonDocument docWeather;
        DeserializationError error = deserializeJson(docWeather, payload);

        if (!error) {
            Serial.println("Weather JSON parsed OK!");

            // Extraire les données actuelles
            apiWeather.temp = docWeather["main"]["temp"].as<float>();
            apiWeather.pressureMSL = docWeather["main"]["pressure"].as<float>();
            int owmId = docWeather["weather"][0]["id"].as<int>();
            apiWeather.weatherCode = mapOWMtoWMO(owmId);

            Serial.printf("Current: Temp=%.1f°C, Press=%.1fhPa, Code=%d (OWM:%d)\n",
                         apiWeather.temp, apiWeather.pressureMSL, apiWeather.weatherCode, owmId);
        } else {
            Serial.print("Weather JSON parse error: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }
    } else {
        Serial.printf("Weather request failed: %d\n", codeWeather);
        http.end();
        return false;
    }
    http.end();

    // ========================================
    // PARTIE 2: Prévisions (forecast API)
    // ========================================
    String urlForecast = String("http://api.openweathermap.org/data/2.5/forecast?lat=") +
                         String(latToUse, 4) + "&lon=" + String(lonToUse, 4) +
                         "&units=metric&appid=" + OPENWEATHER_API_KEY;

    Serial.println("Forecast URL: " + urlForecast);
    http.begin(urlForecast);
    int codeForecast = http.GET();
    Serial.printf("Forecast HTTP Code: %d\n", codeForecast);

    if (codeForecast == 200) {
        String payload = http.getString();
        Serial.printf("Forecast Payload: %d bytes\n", payload.length());

        JsonDocument docForecast;
        DeserializationError error = deserializeJson(docForecast, payload);

        if (!error) {
            Serial.println("Forecast JSON parsed OK!");

            // L'API forecast retourne des prévisions toutes les 3h
            // On va extraire les min/max par jour
            JsonArray list = docForecast["list"].as<JsonArray>();
            Serial.printf("Forecast items: %d\n", list.size());

            // Organiser les données par jour (8 prévisions = 1 jour)
            // Index 0-7: Aujourd'hui, 8-15: Demain, 16-23: J+2, 24-31: J+3
            float dayMin[4] = {999, 999, 999, 999};
            float dayMax[4] = {-999, -999, -999, -999};
            int dayCode[4] = {0, 0, 0, 0};

            // Parcourir toutes les prévisions 3h
            for (int i = 0; i < list.size() && i < 32; i++) {
                JsonObject item = list[i].as<JsonObject>();

                // Calculer le jour (8 prévisions = 1 jour)
                int dayIndex = i / 8;
                if (dayIndex > 3) break;

                float temp = item["main"]["temp"].as<float>();
                int code = item["weather"][0]["id"].as<int>();

                // Mise à jour min/max
                if (temp < dayMin[dayIndex]) dayMin[dayIndex] = temp;
                if (temp > dayMax[dayIndex]) dayMax[dayIndex] = temp;

                // Prendre le code météo du milieu de la journée (index 4 = ~12h)
                if ((i % 8) == 4) {
                    dayCode[dayIndex] = mapOWMtoWMO(code);
                }

                Serial.printf("Item %d: Day=%d, Temp=%.1f, Code=%d\n", i, dayIndex, temp, code);
            }

            // Stocker les prévisions pour J+1, J+2, J+3
            for (int i = 0; i < 3; i++) {
                apiWeather.forecastMin[i] = dayMin[i+1];
                apiWeather.forecastMax[i] = dayMax[i+1];
                apiWeather.forecastCode[i] = dayCode[i+1];

                Serial.printf("FINAL Day %d: Min=%.1f, Max=%.1f, Code=%d\n",
                             i+1, apiWeather.forecastMin[i], apiWeather.forecastMax[i], apiWeather.forecastCode[i]);
            }

        } else {
            Serial.print("Forecast JSON parse error: ");
            Serial.println(error.c_str());
            http.end();
            return false;
        }
    } else {
        Serial.printf("Forecast request failed: %d\n", codeForecast);
        http.end();
        return false;
    }
    http.end();

    // ========================================
    // PARTIE 3: Alertes (One Call API 3.0)
    // ========================================
    Serial.println("\n--- Checking for Weather Alerts ---");
    String urlAlerts = String("https://api.openweathermap.org/data/3.0/onecall?lat=") +
                       String(latToUse, 4) + "&lon=" + String(lonToUse, 4) +
                       "&exclude=minutely,hourly,daily&units=metric&appid=" + OPENWEATHER_API_KEY;

    Serial.println("Alerts URL: " + urlAlerts);
    http.begin(urlAlerts);
    int codeAlerts = http.GET();
    Serial.printf("Alerts HTTP Code: %d\n", codeAlerts);

    if (codeAlerts == 200) {
        String payload = http.getString();
        Serial.printf("Alerts Payload: %d bytes\n", payload.length());

        JsonDocument docAlerts;
        DeserializationError error = deserializeJson(docAlerts, payload);

        if (!error) {
            Serial.println("Alerts JSON parsed OK!");

            // Vérifier la présence d'alertes
            if (docAlerts.containsKey("alerts") && docAlerts["alerts"].is<JsonArray>()) {
                JsonArray alerts = docAlerts["alerts"].as<JsonArray>();
                Serial.printf("Found %d alert(s)!\n", alerts.size());

                if (alerts.size() > 0) {
                    alertActive = true;
                    alertMessage = docAlerts["alerts"][0]["event"].as<String>();
                    Serial.println("Alert: " + alertMessage);

                    // Force l'affichage immédiat de la page d'alerte
                    lastAlertActive = false;
                    currentPage = 3;
                    beep(5, 100);
                    drawFullPage();
                    refreshDisplayData();
                } else {
                    alertActive = false;
                    alertMessage = "Aucune alerte";
                }
            } else {
                Serial.println("No alerts in response");
                alertActive = false;
                alertMessage = "Aucune alerte";
            }
        } else {
            Serial.print("Alerts JSON parse error: ");
            Serial.println(error.c_str());
        }
    } else if (codeAlerts == 401) {
        Serial.println("⚠ One Call API 3.0 requires subscription (alerts disabled)");
        alertActive = false;
        alertMessage = "Aucune alerte";
    } else {
        Serial.printf("Alerts request failed: %d\n", codeAlerts);
        alertActive = false;
        alertMessage = "Aucune alerte";
    }
    http.end();

    Serial.println("✓ OpenWeatherMap fetch complete!");
    return true;
}

bool fetchOpenMeteo() {
    HTTPClient http; http.setTimeout(3000); 
    String url = String("http://api.open-meteo.com/v1/forecast?latitude=") + 
               String(currentGPS.lat, 4) + "&longitude=" + String(currentGPS.lon, 4) + 
               "&current=temperature_2m,weather_code,pressure_msl&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=auto";
    http.begin(url);
    if (http.GET() == 200) {
        JsonDocument doc; deserializeJson(doc, http.getString());
        apiWeather.temp = doc["current"]["temperature_2m"];
        apiWeather.weatherCode = doc["current"]["weather_code"];
        float p = doc["current"]["pressure_msl"];
        if(p > 800) apiWeather.pressureMSL = p;
        for(int i=0; i<3; i++) {
            apiWeather.forecastCode[i] = doc["daily"]["weather_code"][i+1];
            apiWeather.forecastMax[i] = doc["daily"]["temperature_2m_max"][i+1];
            apiWeather.forecastMin[i] = doc["daily"]["temperature_2m_min"][i+1];
        }
        http.end(); return true;
    }
    http.end(); return false;
}

// === NOUVELLE FONCTION : AQI VIA OPEN-METEO (GRATUIT) ===
bool fetchAQI_OpenMeteo() {
    HTTPClient http; http.setTimeout(3000);
    
    // Verification coordonnees
    float latToUse = currentGPS.isValid ? currentGPS.lat : 44.8378;
    float lonToUse = currentGPS.isValid ? currentGPS.lon : -0.5792;

    // URL Open-Meteo Air Quality (Indice Europeen CAQI)
    String url = String("https://air-quality-api.open-meteo.com/v1/air-quality?latitude=") + 
                 String(latToUse, 4) + "&longitude=" + String(lonToUse, 4) + 
                 "&current=european_aqi";
                 
    Serial.print("AQI Req: "); Serial.println(url);

    http.begin(url); 
    int code = http.GET();
    
    if(code == 200) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, http.getString());
        if(!error) {
            int rawAqi = doc["current"]["european_aqi"]; // Echelle 0-100+
            Serial.printf("Raw CAQI: %d\n", rawAqi);
            
            // Conversion échelle CAQI (0-100) vers Echelle 1-5 de votre affichage
            // 1=Bon, 2=Moyen, 3=Degrade, 4=Mauvais, 5=Dangereux
            if (rawAqi < 20) apiWeather.aqi = 1;
            else if (rawAqi < 40) apiWeather.aqi = 2;
            else if (rawAqi < 60) apiWeather.aqi = 3;
            else if (rawAqi < 80) apiWeather.aqi = 4;
            else apiWeather.aqi = 5;

            Serial.printf("Mapped AQI (1-5): %d\n", apiWeather.aqi);
            http.end(); return true;
        }
    }
    http.end(); return false;
}

int mapOWMtoWMO(int id) {
    if (id == 800) return 0; 
    if (id == 801) return 1; 
    if (id == 802) return 2; 
    if (id >= 803) return 3; 
    if (id >= 200 && id < 300) return 95; 
    if (id >= 300 && id < 400) return 51; 
    if (id >= 500 && id < 600) return 61; 
    if (id >= 600 && id < 700) return 71; 
    if (id >= 700 && id < 800) return 45; 
    return 3; 
}

void beep(int count, int duration) {
    for (int i = 0; i < count; i++) {
        ledcWriteTone(BUZZER_LEDC_CHANNEL, 2000); delay(duration);
        ledcWriteTone(BUZZER_LEDC_CHANNEL, 0); if (i < count - 1) delay(duration);
    }
}

void drawSunRays(int cx, int cy, int offsetAngle, uint16_t color) {
    for (int i = 0; i < 8; i++) {
        float angle = (i * 45 + offsetAngle) * 3.14 / 180.0;
        int x1 = cx + cos(angle) * 35; int y1 = cy + sin(angle) * 35;
        int x2 = cx + cos(angle) * 45; int y2 = cy + sin(angle) * 45;
        tft.drawLine(x1, y1, x2, y2, color); tft.drawLine(x1+1, y1, x2+1, y2, color);
    }
}

void drawStartupScreen() {
  tft.fillScreen(C_BLACK);
  tft.fillCircle(120, 90, 30, C_ORANGE); tft.fillCircle(120, 90, 25, C_YELLOW);
  tft.setTextSize(2); tft.setTextColor(C_WHITE);
  tft.setCursor(40, 150); tft.print("METEO STATION");
  tft.setTextSize(1); tft.setTextColor(C_GREY);
  tft.setCursor(90, 175); tft.print("v"); tft.print(PROJECT_VERSION);

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);
  tft.setCursor(50, 210); tft.setTextColor(C_WHITE); tft.print(cleanText("Connexion WiFi..."));

  int angle = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    drawSunRays(120, 90, angle, C_BLACK);
    angle += 10; if (angle >= 360) angle = 0;
    drawSunRays(120, 90, angle, C_YELLOW);
    while (Serial2.available() > 0) gps.encode(Serial2.read());
    delay(100);
  }
  drawSunRays(120, 90, angle, C_YELLOW);
  tft.fillRect(0, 200, 240, 40, C_BLACK); 
  tft.setCursor(10, 210); tft.setTextColor(C_GREEN); tft.setTextSize(1);
  tft.print(cleanText("Connecte: ")); tft.println(WiFi.SSID());
  beep(2, 100); delay(1500);
}

void initSensors() {
  // Initialisation AHT20
  ahtAvailable = aht.begin();
  if(!ahtAvailable) {
    Serial.println("AHT20 ERROR - Temperature/Humidity disabled");
    localSensor.temp = NAN;
    localSensor.hum = NAN;
  } else {
    Serial.println("AHT20 OK");
  }

    // Essayer d'initialiser BMP280 aux deux adresses possibles
    bmpAvailable = bmp.begin(0x76) || bmp.begin(0x77);

    if (!bmpAvailable) {
        Serial.println("BMP280 NOT FOUND - Pressure readings disabled");
        localSensor.pres = NAN;
    } else {
        Serial.println("BMP280 OK");
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2,
                       Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16,
                       Adafruit_BMP280::STANDBY_MS_500);
    }
}

void updateSensors() {
  // Lire AHT20 seulement si disponible
  if (ahtAvailable) {
    sensors_event_t h, t; aht.getEvent(&h, &t);
    localSensor.temp = t.temperature; localSensor.hum = h.relative_humidity;
  } else {
    // Marquer comme non disponible
    localSensor.temp = NAN; localSensor.hum = NAN;
    // Log périodique pour aider le debugging matériel (toutes les 5 minutes)
    unsigned long now = millis();
    if (now - lastAhtWarn > AHT_WARN_INTERVAL) {
      Serial.println("AHT20 still not available — skipping temperature/humidity readings.");
      lastAhtWarn = now;
    }
  }

  // Lire la pression seulement si BMP280 est disponible
  if (bmpAvailable) {
    float p = bmp.readPressure();
    if (!isnan(p) && p > 0) {
      localSensor.pres = p / 100.0F;
      updatePressureTrend(localSensor.pres);
    }
  } else {
    localSensor.pres = NAN;
  }
  
  // LDR DEBUG
  int rawLux = analogRead(PIN_LIGHT_SENSOR);
  Serial.printf("LDR Raw Pin %d: %d\n", PIN_LIGHT_SENSOR, rawLux);
  localSensor.lux = rawLux;

  if (localSensor.temp > ALERT_TEMP_HIGH) { alertActive = true; alertMessage = "Temp HAUTE"; }
  else if (localSensor.temp < ALERT_TEMP_LOW) { alertActive = true; alertMessage = "Temp BASSE"; }

  if (alertActive && !lastAlertActive) {
      currentPage = 3; beep(5, 100); drawFullPage(); refreshDisplayData();
  }
  lastAlertActive = alertActive;
}

void setNeoPixelStatus() {
  if (localSensor.lux < LDR_THRESHOLD_NIGHT) pixels.clear();
  else if (alertActive) pixels.setPixelColor(0, pixels.Color(255, 0, 0));
  else pixels.setPixelColor(0, pixels.Color(0, 255, 0));
  pixels.show();
}

// ================= AFFICHAGE =================
void drawFullPage() {
  tft.fillScreen(C_BLACK); 
  tft.fillRect(0, 0, TFT_WIDTH, 35, C_BLUE); 
  tft.setTextColor(C_GREY); tft.setTextSize(1);
  tft.setCursor(5, 225); tft.printf("Page %d/%d", currentPage + 1, TOTAL_PAGES);
  tft.setTextColor(C_WHITE); tft.setTextSize(2);

  if (currentPage == 0) { // RESUME
    tft.setCursor(10, 50); tft.print(cleanText("INTERIEUR"));
    tft.drawLine(10, 70, 230, 70, C_GREY);
    tft.setCursor(10, 130); tft.print(cleanText("EXTERIEUR"));
    tft.drawLine(10, 150, 230, 150, C_GREY);
  }
  else if (currentPage == 1) { // PREVISIONS
     tft.setCursor(10, 45); tft.print(cleanText("PREVISIONS (3J)")); tft.drawLine(10, 65, 230, 65, C_GREY);
     tft.drawLine(10, 115, 230, 115, 0x2104);
     tft.drawLine(10, 165, 230, 165, 0x2104);
  }
  else if (currentPage == 2) { // ENVIRONNEMENT
    tft.setCursor(10, 45); tft.print(cleanText("ENVIRONNEMENT")); tft.drawLine(10, 65, 230, 65, C_GREY);
    tft.setTextSize(1); tft.setTextColor(C_GREY);
    tft.setCursor(10, 80); tft.print(cleanText("HUMIDITE")); tft.setCursor(120, 80); tft.print("PRESSION");
    tft.setCursor(10, 140); tft.print(cleanText("LUMINOSITE")); tft.setCursor(120, 140); tft.print(cleanText("QUALITE AIR"));
  }
  else if (currentPage == 3) { // ALERTES METEO
    tft.setCursor(10, 45); tft.print(cleanText("ALERTES METEO")); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 4) { // GPS
    tft.setCursor(10, 45); tft.print(cleanText("DONNEES GPS")); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 5) { // RESEAU
     tft.setCursor(10, 45); tft.print(cleanText("RESEAU & SYS")); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 6) { // ETAT SYSTEME
    tft.setCursor(10, 45); tft.print(cleanText("ETAT SYSTEME")); tft.drawLine(10, 65, 230, 65, C_GREY);
    tft.drawLine(10, 120, 230, 120, 0x2104);
    tft.drawLine(10, 180, 230, 180, 0x2104);
  }
}

void refreshDisplayData() {
  int rssi = WiFi.RSSI();
  int bars = (rssi > -55) ? 4 : (rssi > -75) ? 3 : (rssi > -85) ? 2 : (rssi > -95) ? 1 : 0;
  tft.fillRect(5, 5, 25, 20, C_BLUE); 
  for(int i=0; i<4; i++) {
     if(i < bars) tft.fillRect(5 + (i*5), 25 - (i*4), 3, (i*4)+4, C_WHITE);
     else tft.drawRect(5 + (i*5), 25 - (i*4), 3, (i*4)+4, C_GREY);
  }
  tft.setTextSize(2); tft.setCursor(40, 10);
  if(currentGPS.isValid) { tft.setTextColor(C_GREEN, C_BLUE); tft.print("GPS OK"); } 
  else { tft.setTextColor(C_RED, C_BLUE); tft.print("NO FIX"); }
  struct tm timeinfo;
  if(getLocalTime(&timeinfo)){
    tft.setCursor(160, 10); tft.setTextColor(C_WHITE, C_BLUE);
    tft.printf("%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
  }

  tft.setTextColor(C_WHITE, C_BLACK);

  if (currentPage == 0) {
    tft.setTextSize(3);
    tft.setCursor(10, 80); tft.printf("%.1f C  ", localSensor.temp);
    tft.setCursor(10, 160); tft.printf("%.1f C  ", apiWeather.temp);
    tft.fillRect(180, 140, 50, 50, C_BLACK);
    drawWeatherByCode(tft, 200, 165, apiWeather.weatherCode);
  }
  else if (currentPage == 1) {
      const char* labels[3] = {"Demain", "J + 2", "J + 3"};
      for(int i=0; i<3; i++) {
          int y = 80 + (i*50);
          tft.setTextSize(2); tft.setTextColor(C_WHITE, C_BLACK);
          tft.setCursor(10, y); tft.print(cleanText(labels[i]));
          tft.fillRect(100, y-10, 40, 30, C_BLACK);
          drawWeatherByCode(tft, 120, y+5, apiWeather.forecastCode[i]);
          tft.setTextColor(C_BLUE, C_BLACK); tft.setCursor(160, y); tft.printf("%.0f", apiWeather.forecastMin[i]);
          tft.setTextColor(C_RED, C_BLACK); tft.setCursor(200, y); tft.printf("%.0f", apiWeather.forecastMax[i]);
      }
  }
  else if (currentPage == 2) {
    tft.setTextSize(2);
    tft.setCursor(10, 95); tft.printf("%.0f %% ", localSensor.hum);
    tft.setCursor(120, 95);
    if(isnan(localSensor.pres)) tft.print("-- hPa");
    else {
        tft.printf("%.0f ", localSensor.pres);
        int trendVal = 0;
        if(localSensor.trend == "Hausse") trendVal = 1;
        else if(localSensor.trend == "Baisse") trendVal = -1;

        tft.fillRect(190, 90, 30, 30, C_BLACK);
        drawTrendArrow(tft, 200, 95, trendVal);
    }

    tft.setCursor(10, 155); tft.printf("%d lx   ", localSensor.lux);
    tft.setCursor(120, 155);
    if (apiWeather.aqi == 0) {
        tft.setTextColor(C_GREY, C_BLACK); tft.print("N/A");
    } else {
        uint16_t aqiColor = C_GREEN;
        if(apiWeather.aqi == 2) aqiColor = C_YELLOW;
        else if(apiWeather.aqi == 3) aqiColor = C_ORANGE;
        else if(apiWeather.aqi >= 4) aqiColor = C_RED;
        tft.setTextColor(aqiColor, C_BLACK); tft.printf("AQI: %d", apiWeather.aqi);
    }
  }
  else if (currentPage == 3) { // ALERTES METEO
      tft.fillRect(10, 75, 220, 145, C_BLACK);
      if(alertActive) {
          tft.setCursor(10, 80); tft.setTextColor(C_RED); tft.setTextSize(2); tft.print("ALERTE !");
          tft.setCursor(10, 105); tft.setTextSize(1); tft.setTextColor(C_WHITE); tft.print(cleanText(alertMessage));
          tft.fillCircle(200, 165, 30, C_RED); tft.setTextColor(C_WHITE); tft.setCursor(192, 156); tft.setTextSize(3); tft.print("!");
      } else {
          tft.setCursor(10, 100); tft.setTextColor(C_GREEN); tft.setTextSize(2); tft.print("AUCUNE");
          tft.setCursor(10, 125); tft.setTextSize(1); tft.setTextColor(C_GREY); tft.print("Aucune alerte active");
          tft.fillCircle(200, 165, 30, C_GREEN); tft.setTextColor(C_BLACK); tft.setCursor(186, 156); tft.setTextSize(2); tft.print("OK");
      }
  }
  else if (currentPage == 4) { // GPS
    if(!currentGPS.isValid) { tft.setTextSize(1); tft.setTextColor(C_YELLOW, C_BLACK); tft.setCursor(96, 75); tft.print("SIMULATION"); } else { tft.fillRect(96, 75, 70, 10, C_BLACK); }
    tft.setTextColor(C_WHITE, C_BLACK); tft.setTextSize(2);
    tft.setCursor(10, 95); tft.printf("%.4f", currentGPS.lat); tft.setCursor(120, 95); tft.printf("%.4f", currentGPS.lon);
    tft.setCursor(10, 145); tft.printf("%.0f m ", currentGPS.alt); tft.setCursor(120, 145); tft.printf("%.1f km", currentGPS.speed);
    tft.setCursor(10, 195); tft.printf("%d  ", currentGPS.sats); tft.setCursor(120, 195); tft.printf("%.0f  ", currentGPS.course);
    tft.setTextSize(1); tft.setCursor(60, 220); tft.setTextColor(C_GREY, C_BLACK); tft.print("UTC: " + currentGPS.date + " " + currentGPS.time);
  }
  else if (currentPage == 5) { // RESEAU
    tft.setTextSize(1);
    tft.setCursor(10, 80); tft.print("IP: "); tft.println(WiFi.localIP());
    tft.setCursor(10, 100); tft.print("SSID: "); tft.println(WiFi.SSID());
    tft.setCursor(10, 120); tft.print("RSSI: "); tft.print(WiFi.RSSI()); tft.println(" dBm");
    tft.setCursor(10, 140); tft.print("MAC: "); tft.println(WiFi.macAddress());
    tft.setCursor(10, 160); tft.print("Provider: "); tft.println(apiWeather.provider);
  }
  else if (currentPage == 6) { // ETAT SYSTEME
      tft.fillRect(10, 75, 220, 35, C_BLACK);
      if(alertActive) {
          tft.setCursor(10, 80); tft.setTextColor(C_RED); tft.setTextSize(2); tft.print("ALERTE !");
          tft.setCursor(10, 100); tft.setTextSize(1); tft.print(cleanText(alertMessage));
          tft.fillCircle(200, 90, 25, C_RED); tft.setTextColor(C_WHITE); tft.setCursor(194, 83); tft.setTextSize(2); tft.print("!");
      } else {
          tft.setCursor(10, 80); tft.setTextColor(C_GREEN); tft.setTextSize(2); tft.print("NOMINAL");
          tft.setCursor(10, 100); tft.setTextSize(1); tft.setTextColor(C_GREY); tft.print("Aucune alerte");
          tft.fillCircle(200, 90, 25, C_GREEN); tft.setTextColor(C_BLACK); tft.setCursor(188, 83); tft.setTextSize(2); tft.print("OK");
      }

      tft.setTextColor(C_WHITE, C_BLACK); tft.setTextSize(1);
      tft.setCursor(10, 130); tft.print("Uptime: "); tft.print(getUptime());
      tft.setCursor(10, 145); tft.print("RAM: "); tft.print(ESP.getFreeHeap() / 1024); tft.print(" KB libre");
      tft.setCursor(10, 160); tft.print(cleanText("Source Meteo: ")); tft.print(apiWeather.provider);

      tft.setCursor(10, 190); tft.print("WiFi: "); tft.print(WiFi.SSID());
      tft.setCursor(10, 205); tft.print("Signal: "); tft.print(WiFi.RSSI()); tft.print(" dBm");
  }
}
