# Release v2.8.7 — Short Notes

## English summary

v2.8.7 — Patch: Configurable weather language

- Added `WEATHER_LANG` define in `config.h` for language selection ("fr"/"en")
- Updated all OpenWeatherMap API calls to use configurable language parameter
- Centralized language configuration for weather data, forecasts, and alerts
- Single configuration point: change language by editing one line in `config.h`
- Project version bumped to `2.8.7` and user-facing docs updated.

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

v2.8.7 — Correctif : Langue météo configurable

- Ajout de la directive `WEATHER_LANG` dans `config.h` pour choisir la langue ("fr"/"en")
- Mise à jour de tous les appels API OpenWeatherMap pour utiliser le paramètre de langue configurable
- Configuration centralisée de la langue pour les données météo, prévisions et alertes
- Point de configuration unique : changement de langue en modifiant une seule ligne dans `config.h`
- Numéro de version du projet mis à jour en `2.8.7` et documentation utilisateur synchronisée.

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
