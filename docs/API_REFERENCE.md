### 1\. `API_REFERENCE.md` (English)

````markdown
# API Reference

The ESP32-S3 Weather Station exposes a local Web Server providing real-time data in JSON format.

## Base URL
`http://<IP_ADDRESS_OF_ESP32>`

---

## HTTP Endpoints

### `GET /api/data`
Returns all available sensor readings, local weather data, GPS status, and system alerts in a single JSON object.

**Response Structure:**

```json
{
  "sensor": {
    "temp": 22.5,      // Internal Temperature (C)
    "hum": 45,         // Internal Humidity (%)
    "pres": 1013.2,    // Pressure (hPa) - Optional depending on sensor init
    "lux": 1200        // Light Level (0-4095)
  },
  "weather": {
    "temp": 21.0,      // External Temperature from API (C)
    "code": 1          // WMO Weather Code (0=Clear, 1-3=Cloudy, etc.)
  },
  "gps": {
    "lat": 44.8378,    // Latitude
    "lon": -0.5792,    // Longitude
    "valid": true      // true if real GPS fix, false if default/simulation
  },
  "alert": false       // true if a temperature alert is active
}
````

**Status Codes:**

  * `200 OK`: Request successful.

-----

## Telegram Bot Commands

You can interact with the station via the Telegram Chat ID configured in `secrets.h`.

| Command | Description |
| :--- | :--- |
| `/start` | Checks connection and returns the current firmware version. |
| `/status` | Returns an immediate text summary of internal sensors and external weather. |
| `/reboot` | Remotely restarts the ESP32 module. |