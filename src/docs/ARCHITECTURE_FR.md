#### `ARCHITECTURE.md` (English)
```markdown
# System Architecture

## Overview
The system operates on a non-blocking loop architecture to ensure responsiveness across all interfaces (Display, Web, Telegram).

## Core Components

1.  **Non-Blocking Loop:**
    * Uses `millis()` timers for periodic tasks (Sensors reading, Weather fetching, Display refresh).
    * GPS serial data is processed continuously.

2.  **Interrupts (ISR):**
    * Button presses (`PAGE` and `ACTION`) trigger Interrupt Service Routines.
    * This sets flags that are handled in the main loop immediately, preventing UI lag.

3.  **Networking:**
    * **WiFiMulti:** Connects to the best available network from the list.
    * **AsyncWebServer:** Handles HTTP requests asynchronously without blocking the sensor loop.

4.  **Data Flow:**
    * `GPS` -> Updates Coordinates -> Triggers `Open-Meteo API`.
    * `Open-Meteo` -> Provides QNH (Pressure at Sea Level) -> Calibrates `BMP280` Altitude.
    * `AHT20/BMP280` -> Updates Local Data -> Updates `Display` & `Web JSON`.
ARCHITECTURE_FR.md (Français)
Markdown

# Architecture Système

## Vue d'ensemble
Le système fonctionne sur une architecture de boucle non-bloquante pour assurer la réactivité sur toutes les interfaces (Écran, Web, Telegram).

## Composants Clés

1.  **Boucle Non-Bloquante :**
    * Utilise des timers `millis()` pour les tâches périodiques (Lecture capteurs, Météo, Rafraîchissement écran).
    * Les données série du GPS sont traitées en continu.

2.  **Interruptions (ISR) :**
    * Les appuis boutons (`PAGE` et `ACTION`) déclenchent des routines d'interruption.
    * Cela active des drapeaux (flags) traités immédiatement dans la boucle principale, évitant le lag de l'interface.

3.  **Réseau :**
    * **WiFiMulti :** Se connecte au meilleur réseau disponible dans la liste.
    * **AsyncWebServer :** Gère les requêtes HTTP de manière asynchrone sans bloquer la boucle des capteurs.

4.  **Flux de Données :**
    * `GPS` -> Met à jour Coordonnées -> Déclenche `API Open-Meteo`.
    * `Open-Meteo` -> Fournit QNH (Pression niveau mer) -> Calibre l'altitude `BMP280`.
    * `AHT20/BMP280` -> Met à jour Données Locales -> Met à jour `Écran` & `Web JSON`.