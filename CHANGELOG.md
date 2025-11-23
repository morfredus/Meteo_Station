# Changelog

All notable changes to this project are documented in this file.

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