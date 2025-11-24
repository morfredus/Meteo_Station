# Release v2.8.4 — Short Notes

## English summary

v2.8.4 — Patch: Web interface and weather update interval

- Fixed web page displaying nothing due to corrupted duplicate code in `web_page.h`
- Reduced weather update interval from 15 minutes to 2 minutes (`INTERVAL_WEATHER`: 900000 → 120000 ms)
- Improved web interface with diagnostics section and API test buttons
- Project version bumped to `2.8.4` and user-facing docs updated.

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

v2.8.4 — Correctif : Interface web et intervalle de mise à jour météo

- Correction de la page web qui n'affichait rien à cause d'une duplication de code corrompue dans `web_page.h`
- Réduction de l'intervalle de mise à jour météo de 15 minutes à 2 minutes (`INTERVAL_WEATHER` : 900000 → 120000 ms)
- Amélioration de l'interface web avec section diagnostics et boutons de tests API
- Numéro de version du projet mis à jour en `2.8.4` et documentation utilisateur synchronisée.

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
