# Changelog

## [1.7.0] - 2025-11-21
### Performance & Stability
- **Network (Anti-Freeze):** Added a **3000ms Timeout** on Weather HTTP requests.
    - Prevents the UI from freezing indefinitely if the Open-Meteo server is slow or WiFi is unstable.
- **Interface (Feedback):** Added an immediate "Updating..." visual indicator when the action button is pressed, before the network block.
- **GPS:** Optimized the serial reading loop to clear the buffer more efficiently.

## [1.6.0] - 2025-11-21
### System & Stability
- **Inputs (Buttons):**
    - Added a **software refractory period (300ms)** in the main loop.
    - This eliminates page "double jumps" caused by mechanical button bounce or noise.
- **System:** Added diagnostic functions (Uptime, Heap Memory).

### Interface (UI)
- **Navigation:** The "System Status" page is moved to the very end of the cycle (Page 5).
- **System Page (Page 5):** Completely redesigned to be comprehensive.
    - Now displays: Uptime, Free RAM, CPU Frequency, precise WiFi RSSI, and Alert Status.
    - Keeps the visual indicator (Green/Red Shield) but in a compact format to make room for data.
    
## [1.5.0] - 2025-11-21
### Interface & UX
- **Alerts Page (Page 3):**
    - The page is now **always accessible** in the navigation, even without an alert.
    - "Normal" State: Displays a Green Shield and "System Nominal".
    - "Alert" State: Still displays the critical red screen and buzzer.
    - The automatic pop-up when an alert is triggered is maintained.
- **Boot Screen:**
    - Added a **fluid animation**: Sun rays rotate around the logo during WiFi connection.
    - Removed the static progress bar in favor of this cyclic animation.
    
## [1.4.0] - 2025-11-21
### User Experience (UX)
- **Page Reorganization:** New, more logical navigation order:
    1. Summary
    2. Environment
    3. Forecast
    4. Alerts (Conditional)
    5. GPS
    6. Network
- **Smart Alerts Page:**
    - This page is **hidden** in normal navigation if no alert is active.
    - It appears **automatically** and immediately as soon as an alert is detected.

## [1.3.1] - 2025-11-21
### Design & Graphics
- **Graphics Engine:** Replaced static bitmaps with procedural **"Flat Design"**.
- **Weather Icons:** Created 6 vector drawing functions for sharp and colorful icons:
    - `drawSun` (Radiant sun)
    - `drawCloud` (Volumetric clouds)
    - `drawPartlyCloudy` (Sun hidden by cloud)
    - `drawRain` (Cloud + Blue drops)
    - `drawStorm` (Dark cloud + Yellow lightning)
    - `drawSnow` (Cloud + White flakes)
- **Logic:** Exhaustive mapping of Open-Meteo WMO codes to these new icons.

## [1.3.0] - 2025-11-21
### Audio & Visual
- **Sound:** Completely rewritten `beep()` function.
    - WiFi Connected: 2 fast beeps.
    - GPS Fix: 3 fast beeps (triggered only once upon fix).
    - Button Press: 1 short beep.
    - Replaced `tone()` with non-blocking PWM logic for better compatibility with passive buzzers on S3.
- **Graphics:** Introduced Bitmap Icons (`icons.h`) replacing colored circles.
    - Main Screen: Large icon for current weather.
    - Forecast Screen: Icons for each day (Sun, Cloud, Rain).
    - Icons are tinted dynamically (Yellow for sun, Grey for cloud, Blue for rain).

## [1.2.0] - 2025-11-21
### Added
- **Page 5 (Forecast):** Added a 3-day weather forecast page (Max/Min temps, Weather Icons).
- **API:** Extended Open-Meteo request to fetch daily forecast data (`daily`).
- **Code:** Added `getWeatherColor()` helper function for reusable icon color logic.

## [1.1.5] - 2025-11-21
### Improved
- **UI/UX:** Complete redesign of the Startup Screen with a graphical logo and progress bar.
- **Sensors:** Dynamic Barometric Altitude calibration using Sea Level Pressure (QNH) provided by the Open-Meteo API for more accurate readings.
- **GPS:** Added a visual "SIMULATION" tag on the GPS page when the module has no fix (to prevent confusion with default coordinates).

## [1.1.4] - 2025-11-21
### Changed
- **Core:** Migrated to **ArduinoJson v7**. The code was refactored to use the modern API syntax of this library.

## [1.1.3] - 2025-11-21
### Improved
- **UI/UX:** Displayed NTP time in the main screen header.

## [1.1.2] - 2025-11-21
### Fixed
- **GPS:** Fixed initialization of default coordinates to ensure API requests are always valid before a GPS Fix is obtained.

## [1.1.1] - 2025-11-21
### Fixed
- **Inputs:** Switched buttons to **Interrupts (ISR)** for non-blocking handling and instant UI responsiveness.
- **Hardware:** Added I2C address checking and initialization logic for the BMP280 sensor (supports both 0x76 and 0x77).

## [1.1.0] - 2025-11-21
### Added
- **Interface:** Added pages 1 through 4 to organize data (Details, Alerts, GPS, Network).
- **Feature:** Implemented **Telegram Bot** support for remote control and status.
- **Feature:** Web Server and JSON API (`/api/data`) implemented.

## [1.0.0] - 2025-11-21
- Initial project release.