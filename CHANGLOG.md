# Changelog

## [1.2.0] - 2025-11-21
### Added
- **Page 5 (Forecast):** Added 3-day weather forecast page (Max/Min temps, Weather Icons).
- **API:** Extended Open-Meteo request to fetch daily forecast data.

## [1.1.5] - 2025-11-21
### Improved
- **UI/UX:** Complete redesign of the Startup Screen with graphical logo and progress bar.
- **Sensors:** Dynamic Barometric Altitude calibration using local QNH from API.
- **GPS:** Added visual "SIMULATION" tag on GPS page when no fix is available.

## [1.1.4] - 2025-11-21
### Changed
- **Core:** Migrated to **ArduinoJson v7**. Codebase refactored for modern JSON syntax.

## [1.1.1] - 2025-11-21
### Fixed
- **Inputs:** Switched buttons to **Interrupts (ISR)** for instant responsiveness.
- **Hardware:** Added I2C Address scanner for BMP280 (auto-detect 0x76/0x77).

## [1.0.0] - 2025-11-21
- Initial Release.