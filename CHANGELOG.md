# Changelog

All notable changes to this project are documented in this file.

## [2.8.7] - 2025-11-24
### Patch: Configurable weather language
- **Configuration:** Added `WEATHER_LANG` define in `config.h`
  - Allows selection between "fr" (French) or "en" (English)
  - Centralized language configuration for weather data
- **API integration:** Updated all OpenWeatherMap API calls to use `WEATHER_LANG`
  - Current weather API (`/data/2.5/weather`)
  - Forecast API (`/data/2.5/forecast`)
  - Alerts API (`/data/3.0/onecall`)
- **Flexibility:** Single configuration point for weather language preference
  - Change language by editing one line in `config.h`
  - Affects weather descriptions, forecasts, and alerts

## [2.8.6] - 2025-11-24
### Patch: French weather descriptions
- **Weather descriptions:** Added French weather descriptions from OpenWeatherMap
  - Added `weatherDesc` field for current weather
  - Added `forecastDesc[3]` field for 3-day forecasts
  - Retrieved from `weather[0].description` with `lang=fr` parameter
- **Display:** Weather descriptions now show in French
  - Web interface: displays description instead of WMO code
  - JSON API: added `weather.desc` and `forecast[].desc` fields
- **User experience:** Improved with readable localized weather information

## [2.8.5] - 2025-11-24
### Patch: Weather interval, language support and I2C scan endpoint
- **Weather interval:** Reverted to 15 minutes (900000 ms) for INTERVAL_WEATHER
  - The 2-minute interval was too frequent, returning to optimal previous value
- **French language:** Added `lang=fr` parameter to OpenWeatherMap APIs
  - Added to current weather API (`/data/2.5/weather`)
  - Added to forecast API (`/data/2.5/forecast`)
  - Ensures data is received in French (already present on One Call API)
- **I2C endpoint:** Implemented `/api/scan` endpoint (POST)
  - Allows manual triggering of I2C scan from web interface
  - Returns list of detected devices in JSON format
  - Improves hardware diagnostic capabilities

## [2.8.4] - 2025-11-24
### Patch: Web interface and weather update interval
- **Web page:** Fixed web page displaying nothing due to corrupted duplicate code in `web_page.h`
  - Removed duplicate code and rebuilt complete page
  - Improved interface with diagnostics section and API tests
  - Added buttons to refresh diagnostics and display I2C devices
- **Weather interval:** Reduced weather update interval from 15 minutes to 2 minutes
  - `INTERVAL_WEATHER` changed from 900000 ms (15 min) to 120000 ms (2 min)
  - Enables more frequent weather data updates
- **Stability:** Improved system responsiveness with more frequent updates

## [2.8.3] - 2025-11-24
### Patch: I2C sensor initialization robustness
- **AHT20 handling:** Added an `ahtAvailable` flag and guarded AHT20 reads to avoid I2C errors when the sensor is absent or fails to initialize.
  - Prevents runtime error: `[E][Wire.cpp:513] requestFrom(): i2cRead returned Error -1`.
  - When AHT20 is missing, temperature and humidity are marked as `NAN` and the system continues operating.
- **Version bump:** Project version set to `2.8.3` and user-facing docs updated.
- **Build:** Verified PlatformIO build succeeds after changes.

## [2.8.2] - 2025-11-24
## [2.8.2] - 2025-11-24
### Patch: I2C error fix and weather alerts reactivation
- **I2C Error:** Fixed `i2cRead returned Error -1` issue
  - Added `bmpAvailable` flag to track if BMP280 is initialized
  - Only attempt to read BMP280 if available
  - Clear message if BMP280 not found
  - Avoid unnecessary I2C calls causing the error
- **Weather alerts:** Reactivated alert retrieval
  - Uses One Call API 3.0 (`/data/3.0/onecall`)
  - Displays alerts if available (requires subscription)
  - Properly handles error codes (401 = no access)
  - Detailed logs for diagnosis
- **Stability:** Improved system robustness with missing device handling

## [2.8.1] - 2025-11-24
### Patch: Forecast calculation and debugging logs
- **Forecasts:** Fixed daily min/max calculation (999/-999 values corrected)
  - New calculation based on indices: 8 forecasts (3h) = 1 day
  - Index 0-7: Today, 8-15: Tomorrow, 16-23: Day+2, 24-31: Day+3
  - Detailed logs for each forecast item for diagnosis
- **Debugging:** Added startup logs to verify:
  - WiFi status before weather fetch
  - SSID and IP if connected
  - Confirmation of fetchWeather() call and completion
  - Confirmation of updateSensors() call and completion
- **Stability:** Resolved incorrect value display issue (999)

## [2.8.0] - 2025-11-24
### Major Change: Migration to Free OpenWeatherMap APIs
- **OpenWeatherMap API:** Complete migration to free APIs:
  - **Current weather:** Using `/data/2.5/weather` API instead of `onecall`
  - **Forecasts:** Using `/data/2.5/forecast` API (3-hour forecasts over 5 days)
  - **Reason:** One Call API 2.5 now requires a paid subscription
- **JSON Decoding:** Complete adaptation of decoding for new response structures:
  - Extraction of `main.temp`, `main.pressure` for current weather
  - Calculation of daily min/max from 3-hour forecasts
  - Detailed debugging logs for each step
- **Weather alerts:** Temporarily disabled (require One Call API 3.0 with subscription)
- **Stability:** Fixed zero values issue with correct new JSON decoding

## [2.7.1] - 2025-11-24
### Patch: Weather Alerts and OpenWeatherMap Debugging
- **Alerts Page:** Changed display from "NOMINAL" → "AUCUNE" (NONE) for better clarity when no alerts are active.
- **OpenWeatherMap:** Added comprehensive debugging logs to diagnose alert retrieval issues:
  - Display GPS coordinates used (valid or default)
  - Detailed HTTP request and JSON response logs
  - Explicit verification of alerts presence in response
  - Count of alerts found
- **Coordinates:** Explicit use of valid GPS coordinates or default Bordeaux coordinates (44.8378, -0.5792).
- **API Priority:** Confirmation that OpenWeatherMap is always used first to obtain official weather alerts.

## [2.7.0] - 2025-11-24
### Major Improvement: Page Reorganization
- **Navigation:** Complete reorganization of page order for improved user experience.
  - **Page 0:** Summary (unchanged)
  - **Page 1:** Forecast (formerly page 2)
  - **Page 2:** Environment (formerly page 1)
  - **Page 3:** **NEW** - Weather Alerts (dedicated page)
  - **Page 4:** GPS Data (formerly page 3)
  - **Page 5:** Network & System (formerly page 4)
  - **Page 6:** System Status (formerly page 5)
- **Alerts:** New dedicated page for weather alerts with improved visual display.
  - Displays real-time alerts from OpenWeatherMap.
  - Optimized visual interface with red alert icon or green nominal status.
- **UX:** More logical page order: Summary → Forecast → Environment → Alerts → GPS → Network → System.

## [2.6.4] - 2025-11-23
### Patch: Telegram & UX
- **Telegram:** Send a short startup message at boot; the first full status report is sent automatically when sensor/weather measurements become available.
- **Telegram Commands:** Added `/help`, `/weather`, `/system`, `/gps`, `/alerts`, improved `/status`, and added `/getreport` to request the full report on demand.
- **Diagnostics:** Ensures the bot only responds to the configured `TELEGRAM_CHAT_ID` and reports provider/uptime.

## [2.6.2] - 2025-11-23
### Patch: UX & Fun
- **New:** Added a user-triggered "Party Mode" (long-press Action button) — a rainbow NeoPixel animation with a short melody.
- **Docs:** Bumped project version to `2.6.4` in user-facing docs.


## [2.6.0] - 2025-11-22
### Major Improvement (Reliability)
- **AQI Provider:** Switched the air-quality data source to **Open‑Meteo Air Quality API**.
    - **Before:** OpenWeatherMap (required API key and produced intermittent "N/A" results).
    - **Now:** Open‑Meteo Air Quality (free, no API key required, CAMS‑based data).
    - **Benefit:** More reliable AQI values and automatic conversion to the station's 1–5 display scale.

## [2.5.0] - 2025-11-22
### Return to Stability (Rollback)
- **OTA Removal:** Full removal of the over-the-air update feature (`ElegantOTA`) which caused unsolvable dependency conflicts with the web server.
- **Libraries:** Reverted to the stable network libraries that worked before the attempted update (`esphome/ESPAsyncWebServer` and `esphome/AsyncTCP`).
- **System:** Code now matches v2.3.1 (stable), ready for reliable daily use.

## [2.4.4] - 2025-11-22
### Critical Maintenance
- **Libraries:** Strict pinning of `mathieucarbou/ESPAsyncWebServer` (v3.6.0) and `mathieucarbou/AsyncTCP` (v3.3.2).
- **Cleanup:** Clearing the `.pio` cache is required to remove type conflicts (`const AsyncServer`) inherited from older `esphome` forks.

## [2.4.3] - 2025-11-22
### Build Fixes (Compilation)
- **Dependencies:** Replaced `ESPAsyncWebServer` and `AsyncTCP` (esphome forks) with the `mathieucarbou` forks.
    - Fixes a critical C++ compilation error related to discarding qualifiers (`const` correctness).
    - Improves compatibility with recent ESP32 cores.
- **Web Code:** Replaced deprecated `request->send_P()` with `request->send()`. PROGMEM handling is now automatic.

## [2.4.2] - 2025-11-22
### Compilation Fixes
- **OTA / Conflicts:** Resolved "conflicts with a previous declaration" errors (HTTP_GET, HTTP_POST).
    - Reordered `#include` directives in `main.cpp`.
    - Forced define of `ELEGANTOTA_USE_ASYNC_WEBSERVER` before including the library.

## [2.4.1] - 2025-11-22
### Critical Fixes (OTA Compile)
- **ElegantOTA:** Fixed type conflicts between `AsyncWebServer` and the standard `WebServer`.
    - Added missing build flag `-DELEGANTOTA_USE_ASYNC_WEBSERVER=1`.
    - This flag forces the OTA library to compile in asynchronous-compatible mode, resolving "no matching function for call to begin" errors.
- **HTTP Conflicts:** Cleaned up enumeration conflicts (`HTTP_GET`, `HTTP_POST`, etc.) between the ESP32 SDK and AsyncWeb libraries.

## [2.4.0] - 2025-11-22
### Major Features
- **Over-The-Air Updates (OTA):** Integrated `ElegantOTA`.
    - Web update UI available at `http://<IP>/update`.
    - Allows uploading `firmware.bin` without USB.

### Fixes
- **AQI (Air Quality):** Fixed "N/A" result.
    - Cause: SSL validation failure when calling OpenWeatherMap HTTPS.
    - Solution: Use an explicit `WiFiClientSecure` with `setInsecure()` for that specific request.

### Technical
- **Dependencies:** Added `ayushsharma82/ElegantOTA`.

## [2.3.1] - 2025-11-22
### Patch Fixes
- **Compilation:** Fixed critical `TELEGRAM_CERTIFICATE_ROOT` issue by replacing it with `clientSEC.setInsecure()` to adapt to the new Telegram library.
- **JSON:** Updated deprecated methods (`createNestedArray`) for strict compatibility with ArduinoJson v7.
- **Versioning:** Bumped patch number after stability fixes.

## [2.3.0] - 2025-11-22
### UI & Ergonomics
- **Visual Notifications (Toasts):** Replaced simple text messages with a centered dialog for action confirmations (brightness change, update).
    - Appears under the header, black text on white background for readability.
    - Clears the display area after showing to avoid graphical artifacts.
- **Backlight Handling:** Switched PWM frequency to **12 kHz** for a more stable LED backlight.
    - Ensures the "MAX" mode sends raw value 255 to the controller.

### Fixes
- **Display:** Fixed mispositioned confirmation messages that sometimes wrapped onto two lines.

## [2.2.0] - 2025-11-22
### UX
- **Backlight Control:** Button 2 behavior changed:
    - Short press toggles between **Automatic** (LDR-controlled) and **Manual** (100% brightness).
    - Distinct beep: 1 beep = Auto, 2 beeps = Max.
- **Text & Encoding:** Improved `cleanText()` to support uppercase accented letters (É, È, À...).
    - Applied consistently to page titles that previously displayed incorrectly.

### Removal
- **Action Button:** Removed manual weather update on Button 2 (replaced by light control). Updates remain automatic via timer.

## [2.1.1] - 2025-11-22
### Fixes
- **Versioning:** Corrected version numbering (merged maintenance 2.0.x fixes with 2.1.0 graphical additions).
- **Code:** Consolidated JSON syntax fixes and vector graphics handling adjustments.

## [2.1.0] - 2025-11-22
### Interface & Graphics
- **Trend Indicator:** Replaced textual indicators with vector arrows:
    - ↗ Increase: Green up-right arrow.
    - ↘ Decrease: Red down-right arrow.
    - → Stable: Grey horizontal arrow.
- **Text Handling:** Introduced `cleanText()` to normalize accents and special characters.
    - Converts UTF-8 accents to ASCII-friendly equivalents.
    - Replaces degree symbol (°) with GFX font-specific character (`char 247`).

## [2.0.2] - 2025-11-22
### Maintenance
- **ArduinoJson v7:** Rewrote JSON usage to remove deprecation warnings.
    - `createNestedArray` -> `.to<JsonArray>()`
    - `createNestedObject` -> `.add<JsonObject>()`

## [2.0.1] - 2025-11-22
### Critical Fixes
- **Compilation:** Removed obsolete variables (`TELEGRAM_CERTIFICATE_ROOT`).
- **Build Flags:** Added `-D ARDUINOJSON_DEPRECATED_WARNINGS=0` to suppress deprecation noise from third-party libs.

## [2.0.0] - 2025-11-22
### Major Release
### New Features
- **Adaptive Brightness (Auto-Dimming):** Screen backlight adjusts smoothly according to ambient light (LDR).
- **Trend Analysis:** One-hour barometric pressure analysis to determine local weather trend.
- **Web Dashboard 2.0:** Full redesign with responsive dark theme, AJAX updates, gauges, AQI coloring, and 3-day forecast.

### Technical
- **Hardware:** Enabled second LEDC PWM channel for `TFT_BL` backlight control.
- **API:** Extended JSON output to include trend, uptime, and structured forecast arrays.

## [1.9.3] - 2025-11-22
### Fixes
- **SSL/Telegram:** Switched to `clientSEC.setInsecure()` for Telegram HTTPS connections to avoid maintaining a Root CA.
- **JSON:** Fixed a warning around `containsKey` by using ArduinoJson v7 syntax `is<JsonArray>()`.

## [1.9.2] - 2025-11-22
### Fixes
- **Dependency:** Migrated to `cotestatnt/AsyncTelegram2` (a maintained fork).
- **SSL:** Adjusted certificate initialization for the new library.

## [1.9.1] - 2025-11-22
### Fixes
- **PlatformIO:** Restored the `Adafruit ST7735 and ST7789 Library` required for the display controller.

## [1.9.0] - 2025-11-22
### Architecture
- **Telegram:** Replaced the obsolete `UniversalTelegramBot` with **`AsyncTelegram2`** for fully asynchronous handling and ArduinoJson v7 compatibility.

## [1.8.1] - 2025-11-22
### Improvements
- **AQI:** Decoupled AQI retrieval to attempt OpenWeatherMap even when the main provider fails.
- **Interface:** Display the full data provider name ("OpenWeatherMap" or "Open-Meteo") in details pages.

## [1.8.0] - 2025-11-22
### Data Provider Architecture
- **OpenWeatherMap (OWM):** Promoted as primary provider (One Call + AQI).
- **Failover:** Automatic fallback to Open-Meteo on failure or quota issues.
- **Compatibility:** Added OWM ID -> WMO code converter to unify graphics across providers.

## [1.7.0] - 2025-11-21
### Performance & Stability
- **Network Timeout:** Added a 3000ms timeout on HTTP requests to prevent blocking the UI.
- **UX Feedback:** Immediate "Updating..." message when action button is pressed.
- **GPS:** Improved serial buffer handling for faster acquisition.

## [1.6.0] - 2025-11-21
### Stability
- **Debounce:** Added 300ms software debounce for buttons.
- **Diagnostics:** New system diagnostics page showing uptime, free RAM, CPU frequency and RSSI.

## [1.0.0] - 2025-11-21
- Initial project release.