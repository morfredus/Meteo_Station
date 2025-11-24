# Release v2.8.5 — Short Notes

## English summary

v2.8.5 — Patch: Weather interval, language support and I2C scan endpoint

- Reverted weather update interval to 15 minutes (INTERVAL_WEATHER: 120000 → 900000 ms)
- Added `lang=fr` parameter to OpenWeatherMap APIs (current weather & forecast)
- Implemented `/api/scan` POST endpoint for manual I2C scanning
- Project version bumped to `2.8.5` and user-facing docs updated.

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

v2.8.5 — Correctif : Intervalle météo, langue et endpoint scan I2C

- Retour à l'intervalle de mise à jour météo de 15 minutes (INTERVAL_WEATHER : 120000 → 900000 ms)
- Ajout du paramètre `lang=fr` aux API OpenWeatherMap (météo actuelle & prévisions)
- Implémentation de l'endpoint `/api/scan` (POST) pour scan I2C manuel
- Numéro de version du projet mis à jour en `2.8.5` et documentation utilisateur synchronisée.

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
