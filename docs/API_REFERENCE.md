# Latest release: **v2.8.7** — Configurable weather language.

# API Reference

## Local JSON API

The device hosts a web server that exposes a JSON API endpoint for real-time data.

**Endpoint:** `GET /api/data`

### Response Body

The JSON object contains sensor readings and system status.

```json
{
  "temperature": 23.5,
  "humidity": 45.8,
  "pressure": 1013.25,
  "altitude": 150.5,
  "gps_lat": 48.8584,
  "gps_lon": 2.2945,
  "gps_sats": 8,
  "uptime": "1d 02:30:45",
  "free_heap": 123456,
  "cpu_freq": 240,
  "wifi_rssi": -55
}
```

## External APIs

- **Open-Meteo (weather):** Used to fetch weather forecast data. The request includes the `daily` parameter to get 3-day forecasts.
- **Open‑Meteo Air Quality:** Now used as the primary AQI provider (no API key required). AQI is converted to the station's 1–5 display scale.