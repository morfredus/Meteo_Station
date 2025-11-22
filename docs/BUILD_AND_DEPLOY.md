# Latest release: **v2.5.0** — Rollback to stable build. OTA (`ElegantOTA`) removed; network libraries reverted to stable versions.

# Build & Deploy

This project uses **PlatformIO**.

## Build Configuration
The partition scheme is set to `huge_app.csv` to accommodate the extensive libraries (Web, SSL, JSON).

## Steps
1.  Open the project folder in VS Code.
2.  Ensure the PlatformIO extension is loaded.
3.  Connect the ESP32-S3 via USB.
4.  Click **PlatformIO: Upload**.

**Note on Monitoring:**
The serial monitor speed is set to `115200`.

## Cleaning PlatformIO cache (recommended after dependency changes)

If you updated or reverted network libraries (or encounter strange build errors), clear PlatformIO's build cache and re-run a clean build.

Windows PowerShell (from project root):

```powershell
# Delete PlatformIO cache folder
Remove-Item -Recurse -Force .pio

# Optional: run a clean build with PlatformIO
.
# Ensure PlatformIO CLI is available in PATH, then:
platformio run --target clean
platformio run
```

POSIX (bash):

```bash
rm -rf .pio
platformio run --target clean
platformio run
```