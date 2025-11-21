# Station Météo Pro ESP32-S3

Une station météo autonome et sophistiquée construite sur **ESP32-S3**, intégrant géolocalisation GPS, capteurs environnementaux, contrôle Telegram et une interface TFT moderne.

![Version](https://img.shields.io/badge/version-1.2.0-blue.svg) ![License](https://img.shields.io/badge/license-MIT-green.svg)

## 🌟 Fonctionnalités
* **Mesures Temps Réel :** Température, Humidité, Pression, Luminosité (AHT20 + BMP280 + LDR).
* **Suivi GPS :** Géolocalisation automatique et synchronisation de l'heure (GT-U7).
* **Météo Intelligente :** Récupère la météo locale et prévisions à 3 jours via l'API Open-Meteo basée sur la position GPS.
* **Interface Riche :** Écran TFT 1.54" avec 6 pages interactives (Résumé, Détails, Alertes, GPS, Réseau, Prévisions).
* **Connectivité :** Support WiFi Multi-AP & Serveur Web avec API JSON.
* **Contrôle à Distance :** Contrôle complet et rapports d'état via **Bot Telegram**.
* **Alertes Intelligentes :** Alertes visuelles (NeoPixel) et sonores (Buzzer) pour les conditions extrêmes.

## 🛠 Matériel
* **MCU :** ESP32-S3 DevKitC-1 (N16R8)
* **Écran :** 1.54" ST7789 SPI TFT (240x240)
* **Capteurs :** AHT20 (Temp/Hum), BMP280 (Pression/Alt), LDR (Lumière)
* **GPS :** Module GT-U7
* **Divers :** Buzzer, NeoPixel (Intégré), 2x Boutons poussoirs.

## 🚀 Démarrage Rapide
1.  Câblez votre matériel (voir [INSTALL_FR.md](INSTALL_FR.md)).
2.  Configurez le WiFi et les Clés (voir [CONFIG_FR.md](CONFIG_FR.md)).
3.  Compilez et téléversez avec PlatformIO.

## 📂 Documentation
* [Guide d'Installation](INSTALL_FR.md)
* [Configuration](CONFIG_FR.md)
* [Référence API](API_REFERENCE_FR.md)
* [Architecture](ARCHITECTURE_FR.md)
* [Contribuer](CONTRIBUTING_FR.md)