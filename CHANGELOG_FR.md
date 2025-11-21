# Historique des versions

## [1.2.0] - 2025-11-21
### Ajouts
- **Page 5 (Prévisions) :** Ajout d'une page de prévisions météo sur 3 jours (Temp Min/Max, Icônes).
- **API :** Extension de la requête Open-Meteo pour récupérer les données journalières.

## [1.1.5] - 2025-11-21
### Améliorations
- **UI/UX :** Refonte complète de l'écran de démarrage avec logo graphique et barre de progression.
- **Capteurs :** Calibration dynamique de l'altitude barométrique utilisant le QNH local via API.
- **GPS :** Ajout d'un tag visuel "SIMULATION" sur la page GPS en l'absence de signal.

## [1.1.4] - 2025-11-21
### Changements
- **Noyau :** Migration vers **ArduinoJson v7**. Refactorisation du code pour la syntaxe JSON moderne.

## [1.1.1] - 2025-11-21
### Corrections
- **Entrées :** Passage des boutons sur **Interruptions (ISR)** pour une réactivité instantanée.
- **Matériel :** Ajout d'un scanner d'adresses I2C pour le BMP280 (détection auto 0x76/0x77).

## [1.0.0] - 2025-11-21
- Version Initiale.