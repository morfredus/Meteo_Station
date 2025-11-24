# Release v2.8.6 — Short Notes

## English summary

v2.8.6 — Patch: French weather descriptions

- Added French weather descriptions from OpenWeatherMap API
- Display weather descriptions in French on screen and web interface
- Added `weatherDesc` and `forecastDesc` fields to API JSON response
- Improved user experience with localized weather information
- Project version bumped to `2.8.6` and user-facing docs updated.

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

v2.8.6 — Correctif : Descriptions météo en français

- Ajout des descriptions météo en français depuis l'API OpenWeatherMap
- Affichage des descriptions météo en français sur l'écran et l'interface web
- Ajout des champs `weatherDesc` et `forecastDesc` à la réponse JSON de l'API
- Amélioration de l'expérience utilisateur avec informations météo localisées
- Numéro de version du projet mis à jour en `2.8.6` et documentation utilisateur synchronisée.

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
