# Historique des versions (Changelog)

## [1.2.0] - 2025-11-21
### Ajouts
- **Page 5 (Prévisions) :** Ajout d'une page de prévisions météo sur 3 jours (Temp Min/Max, Icônes).
- **API :** Extension de la requête Open-Meteo pour récupérer les données journalières (`daily`).
- **Code :** Ajout d'une fonction d'aide `getWeatherColor()` pour la réutilisation de la logique des couleurs d'icônes.

## [1.1.5] - 2025-11-21
### Améliorations
- **UI/UX :** Refonte complète de l'écran de démarrage avec logo graphique et barre de progression.
- **Capteurs :** Calibration dynamique de l'Altitude barométrique utilisant la pression au niveau de la mer (QNH) fournie par l'API Open-Meteo pour une mesure plus précise.
- **GPS :** Ajout d'un tag visuel "SIMULATION" sur la page GPS lorsque le module n'a pas de Fix (pour éviter la confusion avec les coordonnées par défaut).

## [1.1.4] - 2025-11-21
### Changements
- **Noyau :** Migration vers **ArduinoJson v7**. Le code a été refactorisé pour utiliser la syntaxe moderne de l'API de cette bibliothèque.

## [1.1.3] - 2025-11-21
### Améliorations
- **UI/UX :** Affichage de l'heure NTP dans l'en-tête de l'écran principal.

## [1.1.2] - 2025-11-21
### Corrections
- **GPS :** Correction de l'initialisation des coordonnées par défaut pour assurer que les requêtes API sont toujours valides avant d'obtenir un Fix GPS.

## [1.1.1] - 2025-11-21
### Corrections
- **Entrées :** Passage des boutons sur **Interruptions (ISR)** pour une gestion non-bloquante et une réactivité instantanée de l'interface.
- **Matériel :** Ajout d'une logique de vérification et d'initialisation des adresses I2C pour le capteur BMP280 (supporte 0x76 et 0x77).

## [1.1.0] - 2025-11-21
### Ajouts
- **Interface :** Ajout des pages 1 à 4 pour organiser les données (Détails, Alertes, GPS, Réseau).
- **Fonctionnalité :** Implémentation du support **Telegram Bot** pour le contrôle et le statut à distance.
- **Fonctionnalité :** Serveur Web et API JSON (`/api/data`) implémentés.

## [1.0.0] - 2025-11-21
- Première version initiale du projet.