#### `ARCHITECTURE.md` (English)
```markdown
# System Architecture

## Overview
The system operates on a non-blocking loop architecture to ensure responsiveness across all interfaces (Display, Web, Telegram).

## Core Components

1.  **Non-Blocking Loop:**
    * Uses `millis()` timers for periodic tasks (Sensors reading, Weather fetching, Display refresh).
    * GPS serial data is processed continuously.

2.  **Interrupts (ISR):**
    * Button presses (`PAGE` and `ACTION`) trigger Interrupt Service Routines.
    * This sets flags that are handled in the main loop immediately, preventing UI lag.

3.  **Networking:**
    * **WiFiMulti:** Connects to the best available network from the list.
    * **AsyncWebServer:** Handles HTTP requests asynchronously without blocking the sensor loop.

4.  **Data Flow:**
    * `GPS` -> Updates Coordinates -> Triggers `Open-Meteo API`.
    * `Open-Meteo` -> Provides QNH (Pressure at Sea Level) -> Calibrates `BMP280` Altitude.
    * `AHT20/BMP280` -> Updates Local Data -> Updates `Display` & `Web JSON`.