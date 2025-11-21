# Changelog

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