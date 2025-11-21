#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_NeoPixel.h>
#include <TinyGPSPlus.h>
#include <time.h>

#include "config.h"
#include "web_page.h"
#include "weather_graphics.h" // <--- NOUVEAU FICHIER

// ================= OBJETS GLOBAUX =================
WiFiMulti wifiMulti;
AsyncWebServer server(80);
WiFiClientSecure clientSEC;
UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, clientSEC);

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
} localSensor;

struct WeatherData {
  float temp = 0.0;
  int weatherCode = 0;
  float pressureMSL = 1013.25;
  float forecastMax[3] = {0,0,0};
  float forecastMin[3] = {0,0,0};
  int forecastCode[3] = {0,0,0};
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

bool alertActive = false;
String alertMessage = "Aucune alerte";
bool gpsFixNotified = false;

int currentPage = 0;
const int TOTAL_PAGES = 6;

unsigned long lastTimeSensor = 0;
unsigned long lastTimeWeather = 0;
unsigned long lastTimeTelegram = 0;
unsigned long lastDisplayUpdate = 0;

// ================= INTERRUPTIONS =================
volatile bool flagPagePressed = false;
volatile bool flagActionPressed = false;
unsigned long lastDebounceTime = 0;
const unsigned long DEBOUNCE_DELAY = 200;

void IRAM_ATTR isrPageBtn() {
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
    flagPagePressed = true; lastDebounceTime = millis();
  }
}
void IRAM_ATTR isrActionBtn() {
  if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
    flagActionPressed = true; lastDebounceTime = millis();
  }
}

// ================= PROTOTYPES =================
void drawStartupScreen(); 
void initSensors();
void updateSensors();
void fetchWeather();
void handleTelegram();
void refreshDisplayData();
void drawFullPage();
void beep(int count, int duration);
void setNeoPixelStatus();
void handleButtonLogic();
// Plus besoin de getWeatherIcon() -> géré par weather_graphics.h

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial2.begin(GPS_BAUD_RATE, SERIAL_8N1, PIN_GPS_RXD, PIN_GPS_TXD);

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

  pinMode(PIN_LIGHT_SENSOR, INPUT);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  pixels.begin();
  pixels.setBrightness(NEOPIXEL_BRIGHTNESS);
  pixels.show();

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  
  tft.init(TFT_WIDTH, TFT_HEIGHT);
  tft.setRotation(TFT_ROTATION);
  tft.fillScreen(C_BLACK);
  
  drawStartupScreen();
  initSensors();

  configTime(3600, 3600, "pool.ntp.org");

  clientSEC.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  bot.sendMessage(TELEGRAM_CHAT_ID, "Station v" + String(PROJECT_VERSION) + " Online.", "");
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });
  
  server.on("/api/data", HTTP_GET, [](AsyncWebServerRequest *request){
    JsonDocument doc;
    doc["sensor"]["temp"] = localSensor.temp;
    doc["sensor"]["hum"] = localSensor.hum;
    doc["weather"]["temp"] = apiWeather.temp;
    doc["weather"]["code"] = apiWeather.weatherCode;
    doc["gps"]["lat"] = currentGPS.lat;
    doc["gps"]["lon"] = currentGPS.lon;
    doc["gps"]["valid"] = currentGPS.isValid;
    String response; serializeJson(doc, response);
    request->send(200, "application/json", response);
  });
  
  server.begin();

  fetchWeather();
  updateSensors();
  
  tft.fillScreen(C_BLACK);
  drawFullPage();
}

// ================= LOOP =================
void loop() {
  unsigned long now = millis();

  while (Serial2.available() > 0) gps.encode(Serial2.read());
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
    setNeoPixelStatus();
    refreshDisplayData();
    lastTimeSensor = now;
  }

  if (now - lastTimeWeather > INTERVAL_WEATHER) {
    fetchWeather();
    refreshDisplayData();
    lastTimeWeather = now;
  }

  if (now - lastTimeTelegram > INTERVAL_TELEGRAM) {
    handleTelegram();
    lastTimeTelegram = now;
  }

  if (now - lastDisplayUpdate > 1000) {
    refreshDisplayData();
    lastDisplayUpdate = now;
  }
}

// ================= LOGIQUE =================

void handleButtonLogic() {
  if (flagPagePressed) {
    beep(1, 50);
    flagPagePressed = false;
    currentPage++;
    if (currentPage >= TOTAL_PAGES) currentPage = 0;
    drawFullPage(); refreshDisplayData();
  }
  if (flagActionPressed) {
    beep(1, 50);
    flagActionPressed = false;
    tft.setCursor(220, 5); tft.setTextColor(C_YELLOW, C_BLUE); tft.print("*");
    fetchWeather(); refreshDisplayData();
  }
}

void fetchWeather() {
  if (wifiMulti.run() != WL_CONNECTED) return;
  HTTPClient http;
  String url = String("http://api.open-meteo.com/v1/forecast?latitude=") + 
               String(currentGPS.lat, 4) + "&longitude=" + String(currentGPS.lon, 4) + 
               "&current=temperature_2m,weather_code,pressure_msl&daily=weather_code,temperature_2m_max,temperature_2m_min&timezone=auto";
  
  http.begin(url);
  if (http.GET() == 200) {
    String payload = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (!error) {
      apiWeather.temp = doc["current"]["temperature_2m"];
      apiWeather.weatherCode = doc["current"]["weather_code"];
      float pMSL = doc["current"]["pressure_msl"];
      if(pMSL > 800 && pMSL < 1200) apiWeather.pressureMSL = pMSL;

      for(int i=0; i<3; i++) {
          apiWeather.forecastCode[i] = doc["daily"]["weather_code"][i+1];
          apiWeather.forecastMax[i] = doc["daily"]["temperature_2m_max"][i+1];
          apiWeather.forecastMin[i] = doc["daily"]["temperature_2m_min"][i+1];
      }
    }
  }
  http.end();
}

void beep(int count, int duration) {
    for (int i = 0; i < count; i++) {
        ledcWriteTone(BUZZER_LEDC_CHANNEL, 2000);
        delay(duration);
        ledcWriteTone(BUZZER_LEDC_CHANNEL, 0);
        if (i < count - 1) delay(duration);
    }
}

void handleTelegram() {
  int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  while (numNewMessages) {
    for (int i = 0; i < numNewMessages; i++) {
      if (String(bot.messages[i].chat_id) != TELEGRAM_CHAT_ID) continue;
      String text = bot.messages[i].text;
      if (text == "/start") bot.sendMessage(bot.messages[i].chat_id, "Station v" + String(PROJECT_VERSION), "");
      else if (text == "/status") bot.sendMessage(bot.messages[i].chat_id, "Temp: " + String(localSensor.temp) + "C", "");
      else if (text == "/reboot") ESP.restart();
    }
    numNewMessages = bot.getUpdates(bot.last_message_received + 1);
  }
}

void drawStartupScreen() {
  tft.fillScreen(C_BLACK);
  // On utilise aussi notre moteur graphique pour le boot !
  drawSun(tft, 120, 80); 
  
  tft.setTextSize(2); tft.setTextColor(C_WHITE);
  tft.setCursor(40, 140); tft.print("METEO STATION");
  tft.setTextSize(1); tft.setTextColor(C_GREY);
  tft.setCursor(90, 165); tft.print("v"); tft.print(PROJECT_VERSION);

  wifiMulti.addAP(ssid_1, password_1);
  wifiMulti.addAP(ssid_2, password_2);

  tft.drawRect(40, 200, 160, 10, C_WHITE);
  tft.setCursor(40, 185); tft.setTextColor(C_WHITE); tft.print("Connexion WiFi...");

  int progress = 0;
  while (wifiMulti.run() != WL_CONNECTED) {
    progress += 5; if (progress > 156) progress = 0;
    tft.fillRect(42, 202, 156, 6, C_BLACK);
    tft.fillRect(42, 202, progress, 6, C_GREEN);
    while (Serial2.available() > 0) gps.encode(Serial2.read());
    delay(100);
  }
  tft.fillRect(42, 202, 156, 6, C_GREEN);
  beep(2, 100);
  tft.setCursor(40, 215); tft.setTextColor(C_GREEN); tft.print("Succes: " + WiFi.SSID());
  delay(1500);
}

void initSensors() {
    if(!aht.begin()) Serial.println("AHT20 ERROR");
    if (!bmp.begin(0x76) && !bmp.begin(0x77)) localSensor.pres = NAN;
    else bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, Adafruit_BMP280::STANDBY_MS_500);
}

void updateSensors() {
  sensors_event_t h, t; aht.getEvent(&h, &t);
  localSensor.temp = t.temperature; localSensor.hum = h.relative_humidity;
  float p = bmp.readPressure();
  if (!isnan(p) && p > 0) localSensor.pres = p / 100.0F;
  localSensor.lux = analogRead(PIN_LIGHT_SENSOR);

  if (localSensor.temp > ALERT_TEMP_HIGH) { alertActive = true; alertMessage = "Temp HAUTE"; }
  else if (localSensor.temp < ALERT_TEMP_LOW) { alertActive = true; alertMessage = "Temp BASSE"; }
  else { alertActive = false; }
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
    tft.setCursor(10, 50); tft.print("INTERIEUR");
    tft.drawLine(10, 70, 230, 70, C_GREY);
    tft.setCursor(10, 130); tft.print("EXTERIEUR");
    tft.drawLine(10, 150, 230, 150, C_GREY);
  }
  else if (currentPage == 1) { // DETAILS
    tft.setCursor(10, 45); tft.print("ENVIRONNEMENT"); tft.drawLine(10, 65, 230, 65, C_GREY);
    tft.setTextSize(1); tft.setTextColor(C_GREY);
    tft.setCursor(10, 80); tft.print("HUMIDITE"); tft.setCursor(120, 80); tft.print("PRESSION");
    tft.setCursor(10, 140); tft.print("LUMINOSITE"); tft.setCursor(120, 140); tft.print("ALTITUDE");
  }
  else if (currentPage == 2) { // ALERTES
    tft.setCursor(10, 45); tft.print("ETAT ALERTES"); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 3) { // GPS
    tft.setCursor(10, 45); tft.print("DONNEES GPS"); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 4) { // RESEAU
     tft.setCursor(10, 45); tft.print("RESEAU & SYS"); tft.drawLine(10, 65, 230, 65, C_GREY);
  }
  else if (currentPage == 5) { // PREVISIONS
     tft.setCursor(10, 45); tft.print("PREVISIONS (3J)"); tft.drawLine(10, 65, 230, 65, C_GREY);
     tft.drawLine(10, 115, 230, 115, 0x2104);
     tft.drawLine(10, 165, 230, 165, 0x2104);
  }
}

void refreshDisplayData() {
  // Header
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
    
    // === ICONE METEO DESSINÉE ===
    // On efface d'abord la zone (pour éviter les traces des anciens dessins)
    tft.fillRect(180, 140, 50, 50, C_BLACK); 
    drawWeatherByCode(tft, 200, 165, apiWeather.weatherCode);
  }
  else if (currentPage == 1) {
    tft.setTextSize(2);
    tft.setCursor(10, 95); tft.printf("%.0f %% ", localSensor.hum);
    tft.setCursor(120, 95); if(isnan(localSensor.pres)) tft.print("-- hPa"); else tft.printf("%.0f hPa", localSensor.pres);
    tft.setCursor(10, 155); tft.printf("%d lx   ", localSensor.lux);
    tft.setCursor(120, 155);
    if(isnan(localSensor.pres)) tft.print("-- m   ");
    else {
        float altBaro = 44330 * (1.0 - pow(localSensor.pres / apiWeather.pressureMSL, 0.1903));
        tft.printf("%.0f m  ", altBaro);
    }
  }
  else if (currentPage == 2) {
    tft.setTextSize(2); tft.setCursor(10, 80);
    if(alertActive) { tft.setTextColor(C_RED, C_BLACK); tft.println("! ATTENTION !"); tft.setTextSize(1); tft.println(alertMessage); } 
    else { tft.setTextColor(C_GREEN, C_BLACK); tft.println("AUCUNE ALERTE"); }
  }
  else if (currentPage == 3) {
    if(!currentGPS.isValid) { tft.setTextSize(1); tft.setTextColor(C_ORANGE, C_BLACK); tft.setCursor(130, 55); tft.print("SIMULATION"); } else { tft.fillRect(130, 55, 100, 10, C_BLACK); }
    tft.setTextColor(C_WHITE, C_BLACK); tft.setTextSize(2);
    tft.setCursor(10, 95); tft.printf("%.4f", currentGPS.lat); tft.setCursor(120, 95); tft.printf("%.4f", currentGPS.lon);
    tft.setCursor(10, 145); tft.printf("%.0f m ", currentGPS.alt); tft.setCursor(120, 145); tft.printf("%.1f km", currentGPS.speed);
    tft.setCursor(10, 195); tft.printf("%d  ", currentGPS.sats); tft.setCursor(120, 195); tft.printf("%.0f  ", currentGPS.course);
    tft.setTextSize(1); tft.setCursor(60, 220); tft.setTextColor(C_GREY, C_BLACK); tft.print("UTC: " + currentGPS.date + " " + currentGPS.time);
  }
  else if (currentPage == 4) {
    tft.setTextSize(1);
    tft.setCursor(10, 80); tft.print("IP: "); tft.println(WiFi.localIP());
    tft.setCursor(10, 100); tft.print("SSID: "); tft.println(WiFi.SSID());
    tft.setCursor(10, 120); tft.print("RSSI: "); tft.print(WiFi.RSSI()); tft.println(" dBm");
    tft.setCursor(10, 140); tft.print("MAC: "); tft.println(WiFi.macAddress());
  }
  else if (currentPage == 5) { // PREVISIONS
      const char* labels[3] = {"Demain", "J + 2", "J + 3"};
      for(int i=0; i<3; i++) {
          int y = 80 + (i*50); 
          tft.setTextSize(2); tft.setTextColor(C_WHITE, C_BLACK);
          tft.setCursor(10, y); tft.print(labels[i]);
          
          // ICONE METEO PREVUE (Vectoriel)
          // On efface zone icone
          tft.fillRect(100, y-10, 40, 30, C_BLACK);
          drawWeatherByCode(tft, 120, y+5, apiWeather.forecastCode[i]);
          
          tft.setTextColor(C_BLUE, C_BLACK); tft.setCursor(160, y); tft.printf("%.0f", apiWeather.forecastMin[i]);
          tft.setTextColor(C_RED, C_BLACK); tft.setCursor(200, y); tft.printf("%.0f", apiWeather.forecastMax[i]);
      }
  }
}