# Release v2.5.0 — Short Notes

## English summary

v2.5.0 — Rollback to stability (OTA removed)

- Removed over-the-air update feature (`ElegantOTA`) due to unsolvable dependency conflicts with the async web server.
- Reverted/locked network libraries to stable versions to restore reliable builds (`ESPAsyncWebServer` / `AsyncTCP` forks).
- Documentation updated (changelog, READMEs, docs/*).
- Added PlatformIO cache-clean instructions to `docs/BUILD_AND_DEPLOY.md` (PowerShell + bash).

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
- OTA endpoint (`/update`) is removed in this release.
- Tag: `v2.5.0` (already created and pushed).

---

## Résumé (Français)

v2.5.0 — Retour à la stabilité (OTA supprimée)

- Suppression de la mise à jour OTA (`ElegantOTA`) en raison de conflits de dépendances avec le serveur web asynchrone.
- Restauration / verrouillage des bibliothèques réseau vers des versions stables.
- Documentation mise à jour (`CHANGELOG.md`, `README.md`, `docs/*`).
- Ajout d'instructions de nettoyage du cache PlatformIO dans `docs/BUILD_AND_DEPLOY_FR.md`.

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
