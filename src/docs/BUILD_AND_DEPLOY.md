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