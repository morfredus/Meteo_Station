# Latest release: **v2.6.0** — AQI provider switched to Open‑Meteo Air Quality API (free, no API key).

# Configuration

## Secrets File
**IMPORTANT:** You must create a file named `secrets.h` in the `include/` folder. This file is ignored by git for security.

Copy the following template into `include/secrets.h`:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// WiFi Credentials (Multi-AP support)
const char* ssid_1 = "YOUR_SSID_1";
const char* password_1 = "YOUR_PASSWORD_1";
const char* ssid_2 = "YOUR_SSID_2";
const char* password_2 = "YOUR_PASSWORD_2";

// Telegram Bot
#define TELEGRAM_BOT_TOKEN "YOUR_BOT_TOKEN_HERE"
#define TELEGRAM_CHAT_ID "YOUR_CHAT_ID_HERE"

#endif