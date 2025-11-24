# Release v2.8.3 — Short Notes

## English summary

v2.8.3 — Patch: I2C sensor initialization robustness

- Added a guard for the AHT20 sensor (`ahtAvailable`) and protected I2C reads when the sensor is absent or fails to initialise.
- Prevents the runtime I2C error: `requestFrom(): i2cRead returned Error -1`.
- Project version bumped to `2.8.3` and user-facing docs updated.

## Instructions (short)

After pulling `main`, perform a clean PlatformIO build to avoid stale cache issues:

PowerShell:
```powershell
# From project root
Remove-Item -Recurse -Force .pio
platformio run --target clean
platformio run
```

Bash:
```bash
rm -rf .pio
platformio run --target clean
platformio run
```

## Notes
- AQI provider changed to Open‑Meteo Air Quality API; no OTA-related changes in this release.

---

## Résumé (Français)

v2.8.3 — Correctif : Robustesse initialisation I2C

- Ajout d'une protection pour le capteur AHT20 (`ahtAvailable`) et des lectures I2C conditionnelles si le capteur est absent ou échoue à l'initialisation.
- Évite l'erreur runtime I2C : `requestFrom(): i2cRead returned Error -1`.
- Numéro de version du projet mis à jour en `2.8.3` et documentation utilisateur synchronisée.

## Instructions (court)

PowerShell :
```powershell
Remove-Item -Recurse -Force .pio
platformio run --target clean
platformio run
```

Bash :
```bash
rm -rf .pio
platformio run --target clean
platformio run
```

---

Paste this file content into the GitHub Release body when creating the draft, or use the API/CLI commands below.
