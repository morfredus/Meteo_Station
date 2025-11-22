# Release v2.6.0 — Short Notes

## English summary

v2.6.0 — Reliability improvement (AQI provider change)

- Switched the air-quality data provider to **Open‑Meteo Air Quality API** (free, no API key required) to improve AQI reliability.
- Documentation updated across repository to reflect v2.6.0.

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

v2.6.0 — Amélioration de la fiabilité (changement du fournisseur AQI)

- Changement du fournisseur de qualité de l'air vers **Open‑Meteo Air Quality API** (gratuit, sans clé API) pour améliorer la fiabilité des valeurs AQI.
- Documentation mise à jour dans le dépôt pour refléter v2.6.0.

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
