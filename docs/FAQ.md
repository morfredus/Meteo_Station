# FAQ

**Q: My Altitude shows -110m, why?**
A: This happens if the BMP280 uses a static standard pressure (1013.25 hPa) while the real weather is high pressure. **Update to v1.1.5+**: We now use dynamic calibration via the Weather API to fix this automatically.

**Q: The GPS says "NO FIX" or "SIMULATION".**
A: GPS modules like the GT-U7 need a clear view of the sky. Indoors, they often fail to lock satellites. The system defaults to Bordeaux coordinates until a fix is found.

**Q: The buttons are not responding instantly.**
A: Ensure you are using version **v1.1.1** or higher, which implemented Interrupts (ISR) for button handling.