// ===================================
// INCLUDES
// ===================================
#include "main.h" // Fichier principal contenant les déclarations globales
#include <WiFi.h> // Pour la connexion WiFi
#include <time.h> // Pour la gestion du temps (NTP)

// ===================================
// GLOBAL OBJECTS & VARIABLES
// ===================================
// TFT Display
Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);
bool tftInitialized = false;

// NeoPixel LED
Adafruit_NeoPixel pixels(NEOPIXEL_NUM_PIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

// GPS
TinyGPSPlus gps;
HardwareSerial SerialGPS(2); // UART 2

// DHT Sensor
DHT dht(DHTPIN, DHTTYPE);

// Structure pour stocker les identifiants WiFi
struct WifiNetwork {
  const char* ssid;
  const char* password;
};

// Liste des réseaux WiFi à essayer (depuis secrets.h)
const WifiNetwork wifiList[] = {
  {WIFI_SSID_1, WIFI_PASSWORD_1},
  {WIFI_SSID_2, WIFI_PASSWORD_2}
};
const int NUM_NETWORKS = sizeof(wifiList) / sizeof(wifiList[0]);


// System State
bool isConnected = false;
bool isGPSAcquired = false;
int currentPage = PAGE_GPS_DATA;

// Sensor Data
float dhtTemp = 20.0;
float dhtHum = 50.0;
int lightLevel = 0;

// Déclarations pour les fonctions locales à ce fichier
void initWiFi();
void initTime();


void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    delay(1000);
    LOG_PRINTLN("\n========================================");
    LOG_PRINTLN("       " PROJECT_NAME " - v" PROJECT_VERSION);
    LOG_PRINTLN("========================================");

    // --- Hardware Init ---
    LOG_PRINTLN("\n[INIT] Initializing Hardware...");

    // Buttons
    pinMode(PIN_BUTTON_1, INPUT_PULLUP);
    pinMode(PIN_BUTTON_2, INPUT_PULLUP);
    LOG_PRINTF("[INIT] - Buttons on GPIO%d, GPIO%d\n", PIN_BUTTON_1, PIN_BUTTON_2);

    // Buzzer
    ledcSetup(BUZZER_LEDC_CHANNEL, 5000, 8); // Setup LEDC channel
    ledcAttachPin(PIN_BUZZER, BUZZER_LEDC_CHANNEL);
    LOG_PRINTF("[INIT] - Buzzer on GPIO%d (LEDC ch%d)\n", PIN_BUZZER, BUZZER_LEDC_CHANNEL);

    // NeoPixel
    pixels.begin();
    pixels.setBrightness(NEOPIXEL_BRIGHTNESS);
    setStatusLED(NEOPIXEL_COLOR_BLUE);
    LOG_PRINTF("[INIT] - NeoPixel on GPIO%d\n", PIN_NEOPIXEL);

    // Light Sensor
    pinMode(PIN_LIGHT_SENSOR, INPUT);
    LOG_PRINTF("[INIT] - Light Sensor on GPIO%d\n", PIN_LIGHT_SENSOR);

    // I2C
    Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
    LOG_PRINTF("[INIT] - I2C on SDA:GPIO%d, SCL:GPIO%d\n", PIN_I2C_SDA, PIN_I2C_SCL);

    // GPS
    SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1, PIN_GPS_RXD, PIN_GPS_TXD);
    LOG_PRINTF("[INIT] - GPS on RX:GPIO%d, TX:GPIO%d @ %d baud\n", PIN_GPS_RXD, PIN_GPS_TXD, GPS_BAUD_RATE);

    // --- TFT Init ---
    LOG_PRINTLN("\n[INIT] Initializing TFT Display...");
    tft.init(TFT_WIDTH, TFT_HEIGHT, SPI_MODE3);
    tft.setRotation(TFT_ROTATION);
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH); // Turn on backlight
    tft.fillScreen(TFT_COLOR_BG);
    tftInitialized = true;
    LOG_PRINTLN("[INIT] - TFT Initialized.");

    // --- Sensor Init ---
    LOG_PRINTLN("\n[INIT] Initializing DHT Sensor...");
    dht.begin();
    delay(2000); // Wait for DHT to stabilize
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (!isnan(temp) && !isnan(hum)) {
        dhtTemp = temp;
        dhtHum = hum;
        LOG_PRINTF("[INIT] - DHT Sensor OK: %.1f°C, %.1f%%\n", dhtTemp, dhtHum);
        beep(2500, 100);
    } else {
        LOG_PRINTLN("[INIT] - DHT Sensor FAILED. Using default values.");
        beep(1000, 500);
    }

    // --- Network Init ---
    initWiFi();
    if (isConnected) {
        initTime();
    }

    // --- Finalizing ---
    LOG_PRINTLN("\n[INIT] Setup Complete. Starting main loop.");
    setStatusLED(NEOPIXEL_COLOR_GREEN);
    beep(3000, 50); delay(100); beep(3000, 50);
    delay(1000);
    setStatusLED(NEOPIXEL_COLOR_OFF);
}

void loop() {
    // Handle button press for page switching
    static unsigned long lastButtonPress = 0;
    if (digitalRead(PIN_BUTTON_1) == LOW && (millis() - lastButtonPress > BUTTON_DEBOUNCE_MS)) {
        lastButtonPress = millis();
        currentPage = (currentPage + 1) % NUM_PAGES;
        LOG_PRINTF("[UI] Button pressed. Switching to page %d\n", currentPage);
        beep(2000, 50);
        displayPage(currentPage); // Update display immediately
    }

    // Read GPS data
    while (SerialGPS.available() > 0) {
        gps.encode(SerialGPS.read());
    }

    // Periodically update display
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > GPS_UPDATE_RATE) {
        lastDisplayUpdate = millis();
        displayPage(currentPage);

        // Update LED status based on GPS fix
        if (gps.location.isValid() && gps.location.age() < GPS_TIMEOUT) {
            if (!isGPSAcquired) {
                isGPSAcquired = true;
                setStatusLED(NEOPIXEL_COLOR_GREEN);
                beep(BUZZER_BIP_FREQ, BUZZER_BIP_DURATION);
            }
        } else {
            if (isGPSAcquired) {
                isGPSAcquired = false;
                setStatusLED(NEOPIXEL_COLOR_RED);
                beep(BUZZER_ALERT_FREQ, BUZZER_ALERT_DURATION);
            }
        }
    }

    // Blink LED if no fix
    static unsigned long lastBlink = 0;
    if (!isGPSAcquired && (millis() - lastBlink > NEOPIXEL_BLINK_INTERVAL)) {
        lastBlink = millis();
        static bool ledState = false;
        ledState = !ledState;
        pixels.setPixelColor(0, ledState ? NEOPIXEL_COLOR_RED : NEOPIXEL_COLOR_OFF);
        pixels.show();
    }
}

// ===================================
// NETWORK FUNCTIONS
// ===================================

void initWiFi() {
    LOG_PRINTLN("\n[WIFI] Initializing WiFi connection...");
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    delay(100);

    for (int i = 0; i < NUM_NETWORKS; i++) {
        // Ne tente pas de se connecter si le SSID est un placeholder
        if (strcmp(wifiList[i].ssid, "Your_WiFi_SSID_1") == 0 || strcmp(wifiList[i].ssid, "Your_WiFi_SSID_2") == 0 || strlen(wifiList[i].ssid) == 0) {
            LOG_PRINTF("[WIFI] Skipping placeholder or empty network slot %d\n", i + 1);
            continue;
        }

        LOG_PRINTF("[WIFI] Attempting to connect to SSID: %s\n", wifiList[i].ssid);
        setStatusLED(NEOPIXEL_COLOR_BLUE);
        WiFi.begin(wifiList[i].ssid, wifiList[i].password);

        unsigned long startAttemptTime = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < WIFI_CONNECT_TIMEOUT) {
            LOG_PRINT(".");
            delay(500);
        }
        LOG_PRINTLN("");

        if (WiFi.status() == WL_CONNECTED) {
            isConnected = true;
            LOG_PRINTLN("[WIFI] ✅ WiFi Connected!");
            LOG_PRINTF("[WIFI] IP Address: %s\n", WiFi.localIP().toString().c_str());
            setStatusLED(NEOPIXEL_COLOR_GREEN);
            beep(2500, 150);
            delay(1000);
            setStatusLED(NEOPIXEL_COLOR_OFF);
            return; // Sortir de la fonction si connecté
        } else {
            LOG_PRINTF("[WIFI] ❌ Failed to connect to %s.\n", wifiList[i].ssid);
            WiFi.disconnect();
            delay(1000);
        }
    }

    LOG_PRINTLN("[WIFI] ❌ Could not connect to any WiFi network. Running in offline mode.");
    isConnected = false;
    setStatusLED(NEOPIXEL_COLOR_RED);
    beep(BUZZER_ALERT_FREQ, BUZZER_ALERT_DURATION);
    delay(1000);
    setStatusLED(NEOPIXEL_COLOR_OFF);
}

void initTime() {
    LOG_PRINTLN("\n[NTP] Configuring time...");
    LOG_PRINTF("[NTP] Server: %s, GMT Offset: %d, Daylight Offset: %d\n", NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
    
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        LOG_PRINTLN("[NTP] ❌ Failed to obtain time.");
        beep(BUZZER_ALERT_FREQ, 200);
        return;
    }

    LOG_PRINTLN("[NTP] ✅ Time synchronized successfully.");
    beep(2800, 100);

    // Afficher l'heure actuelle pour vérification
    char timeString[50];
    strftime(timeString, sizeof(timeString), "%A, %B %d %Y %H:%M:%S", &timeinfo);
    LOG_PRINTF("[NTP] Current time: %s\n", timeString);
}


// ===================================
// HELPER FUNCTIONS
// ===================================

void setStatusLED(uint32_t color) {
    pixels.setPixelColor(0, color);
    pixels.show();
}

void beep(int freq, int duration) {
    ledcWriteTone(BUZZER_LEDC_CHANNEL, freq);
    delay(duration);
    ledcWriteTone(BUZZER_LEDC_CHANNEL, 0); // Stop tone
}

void displayPage(int page) {
    if (!tftInitialized) return;

    tft.fillScreen(TFT_COLOR_BG);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_COLOR_HEADER);
    tft.setTextSize(FONT_SIZE_HEADER);

    switch (page) {
        case PAGE_GPS_DATA:
            tft.println("Page 1: GPS Data");
            // Add more display logic here
            break;
        case PAGE_DIAGNOSTICS:
            tft.println("Page 2: Diagnostics");
            // Add more display logic here
            break;
        case PAGE_SATELLITES:
            tft.println("Page 3: Satellites");
            // Add more display logic here
            break;
    }
}