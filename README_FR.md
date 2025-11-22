# Station Météo ESP32 (Projet Meteo_Station)

**Version Actuelle : 2.6.0** (au 2025-11-22)

Note : v2.6.0 améliore la fiabilité en changeant le fournisseur de données AQI vers l'API Open‑Meteo Air Quality (gratuite, sans clé API). Le projet conserve l'ensemble de bibliothèques réseau stable introduit lors du rollback v2.5.x.

## 📖 À propos du projet

Ce projet transforme un microcontrôleur ESP32-S3 en une station météo complète et multifonctionnelle. Il récupère les données de capteurs locaux (température, humidité, pression) et les enrichit avec des prévisions et des données externes via l'API Open-Meteo.

L'interface utilisateur est affichée sur un écran TFT couleur et peut être contrôlée via des boutons physiques. Le projet intègre également un serveur web pour un accès à distance, une API JSON pour l'intégration de données, et un bot Telegram pour les notifications et les commandes.

## ✨ Fonctionnalités clés

- **Interface Multi-Pages :** Navigation claire entre plusieurs écrans :
  1.  **Résumé :** Météo actuelle, température, et icône dynamique.
  2.  **Environnement :** Données des capteurs locaux (humidité, pression).
  3.  **Prévisions :** Météo sur 3 jours avec températures min/max.
  4.  **GPS :** Coordonnées, vitesse, et statut du fix.
  5.  **État Système :** Uptime, RAM libre, fréquence CPU, et statut WiFi.
- **Graphismes Modernes :** Icônes météo vectorielles ("Flat Design") dessinées de manière procédurale pour un affichage net.
- **Connectivité Avancée :**
  - **Serveur Web Asynchrone :** Interface web pour visualiser les données depuis un navigateur.
  - **API JSON (`/api/data`) :** Permet d'intégrer facilement les données de la station dans d'autres systèmes.
  - **Bot Telegram :** Contrôlez la station et recevez des alertes directement sur Telegram.
- **Stabilité et Performance :**
  - **Anti-Freeze Réseau :** Un timeout sur les requêtes HTTP empêche l'appareil de se bloquer.
  - **Gestion des Boutons Fiable :** Utilisation d'interruptions et d'une période réfractaire pour éviter les rebonds et les doubles pressions.
- **Système d'Alertes Intelligent :** Une page dédiée s'affiche automatiquement en cas d'alerte météo, avec un indicateur visuel et sonore.

## 🛠️ Matériel

- **Microcontrôleur :** ESP32-S3 (Modèle utilisé : `4d_systems_esp32s3_gen4_r8n16`)
- **Affichage :** Écran TFT couleur compatible ST7789.
- **Capteurs :**
  - **Température & Humidité :** Adafruit AHT20
  - **Pression & Altitude :** Adafruit BMP280
  - **Géolocalisation :** Module GPS (utilisant TinyGPSPlus)
- **Interaction :** Boutons poussoirs connectés aux broches GPIO.
- **Audio :** Buzzer passif pour les notifications sonores.
- **Visuel :** LED RGB (NeoPixel) pour des indicateurs d'état.

## 📚 Bibliothèques logicielles (Dépendances)

Ce projet est construit avec PlatformIO et s'appuie sur les bibliothèques suivantes :

| Bibliothèque                                   | Version | Rôle                                                              |
| ---------------------------------------------- | ------- | ----------------------------------------------------------------- |
| `esphome/ESPAsyncWebServer-esphome`            | `^3.1.0`  | Crée un serveur web asynchrone performant.                        |
| `esphome/AsyncTCP-esphome`                     | `^2.1.0`  | Couche TCP asynchrone requise par le serveur web.                 |
| `bblanchon/ArduinoJson`                        | `^7.0.4`  | Analyse des réponses JSON de l'API Open-Meteo.                    |
| `witnessmenow/UniversalTelegramBot`            | `^1.3.0`  | Gère la communication avec l'API de Telegram.                     |
| `adafruit/Adafruit GFX Library`                | `^1.11.9` | Moteur graphique de base pour le dessin de formes et de texte.    |
| `adafruit/Adafruit ST7735 and ST7789 Library` | `^1.10.3` | Pilote pour l'écran TFT couleur.                                  |
| `mikalhart/TinyGPSPlus`                        | `^1.1.0`  | Analyse des phrases NMEA provenant du module GPS.                 |
| `adafruit/Adafruit NeoPixel`                   | `^1.12.0` | Contrôle des LED RGB adressables.                                 |
| `adafruit/Adafruit Unified Sensor`             | `^1.1.14` | Abstraction matérielle pour les capteurs Adafruit.                |
| `adafruit/Adafruit AHTX0`                      | `^2.0.5`  | Pilote pour le capteur de température et d'humidité AHT20.        |
| `adafruit/Adafruit BMP280 Library`             | `^2.6.8`  | Pilote pour le capteur de pression barométrique BMP280.           |