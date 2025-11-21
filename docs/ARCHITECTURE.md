# Project Architecture

This document provides an overview of the software architecture of the Meteo Station.

## Core Concepts

### Main Loop & State Machine
The system is built around a non-blocking main loop (`loop()`). A state machine manages the current page displayed on the screen. User inputs (button presses) trigger state transitions to navigate between pages.

### Input Management
- **Interrupt-Based Buttons:** Buttons are handled via interrupts (ISR) for immediate responsiveness without blocking the main loop.
- **Software Debounce:** A 300ms refractory period is implemented in the main loop to prevent mechanical bounces or noise from causing multiple page jumps on a single press.

### Network & APIs
- **Open-Meteo API:** Used to fetch current weather, daily forecasts, and sea-level pressure (QNH) for altitude calibration.
- **HTTP Request Timeout:** All weather API requests have a **3000ms timeout** to prevent the device from freezing if the network is slow or the server is unresponsive.
- **Local Web Server:** An `ESPAsyncWebServer` provides a web interface and a JSON API.
- **JSON API (`/api/data`):** Exposes real-time sensor data and system diagnostics.
- **Telegram Bot:** Allows for remote control and status checks.

### Graphics Engine
- **Procedural "Flat Design":** Weather icons are drawn procedurally (vector-based) rather than using static bitmap files. This results in crisp, scalable graphics and reduces storage needs.
- **GFX Libraries:** The Adafruit GFX and ST7789 libraries form the basis of the display driver.

### System Stability & Diagnostics
- **Non-Blocking Audio:** The `beep()` function uses non-blocking PWM logic, ensuring that sound notifications do not halt other processes.
- **System Page:** A dedicated page (Page 5) displays critical system diagnostics: Uptime, Free Heap Memory, CPU Frequency, and WiFi RSSI.
- **Smart Alerts:** The alert page is always accessible but will also pop up automatically and immediately if a critical alert is triggered.

### Sensor Management
- **Dynamic Altitude Calibration:** Barometric altitude is dynamically calibrated using the Sea Level Pressure (QNH) provided by the Open-Meteo API for improved accuracy.
- **I2C Discovery:** The code automatically checks for the BMP280 sensor at both common I2C addresses (0x76 and 0x77).