# Dernière version : **v2.5.0** — Retour à la stabilité. OTA (`ElegantOTA`) supprimée ; les bibliothèques réseau sont revenues à des versions stables.

# Compilation & Déploiement

Ce projet utilise **PlatformIO**.

## Configuration de Build
Le schéma de partition est réglé sur `huge_app.csv` pour accueillir les nombreuses bibliothèques (Web, SSL, JSON).

## Étapes
1.  Ouvrez le dossier du projet dans VS Code.
2.  Assurez-vous que l'extension PlatformIO est chargée.
3.  Connectez l'ESP32-S3 via USB.
4.  Cliquez sur **PlatformIO: Upload**.

**Note sur le Moniteur :**
La vitesse du moniteur série est réglée sur `115200`.

## Nettoyage du cache PlatformIO (recommandé après changement de dépendances)

Si vous avez modifié ou rétabli des bibliothèques réseau (ou rencontré des erreurs de build étranges), supprimez le cache de build de PlatformIO et relancez une compilation propre.

PowerShell (depuis la racine du projet) :

```powershell
# Supprime le dossier de cache PlatformIO
Remove-Item -Recurse -Force .pio

# Optionnel : exécute une compilation propre avec PlatformIO
platformio run --target clean
platformio run
```

POSIX (bash) :

```bash
rm -rf .pio
platformio run --target clean
platformio run
```