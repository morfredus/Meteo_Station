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