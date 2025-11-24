# Historique des Versions (Changelog)

## [2.8.5] - 2025-11-24
### Correctif : Intervalle météo, langue et endpoint scan I2C
- **Intervalle météo :** Retour à l'intervalle de 15 minutes (900000 ms) pour INTERVAL_WEATHER
  - L'intervalle de 2 minutes était trop fréquent, retour à la valeur optimale précédente
- **Langue française :** Ajout du paramètre `lang=fr` aux API OpenWeatherMap
  - Ajouté à l'API météo actuelle (`/data/2.5/weather`)
  - Ajouté à l'API prévisions (`/data/2.5/forecast`)
  - Garantit la réception des données en français (déjà présent sur One Call API)
- **Endpoint I2C :** Implémentation de l'endpoint `/api/scan` (POST)
  - Permet de déclencher manuellement un scan I2C depuis l'interface web
  - Retourne la liste des périphériques détectés au format JSON
  - Améliore les capacités de diagnostic matériel

## [2.8.4] - 2025-11-24
### Correctif : Interface web et intervalle de mise à jour météo
- **Page web :** Correction de la page web qui n'affichait rien à cause d'une duplication de code corrompue dans `web_page.h`
  - Suppression du code dupliqué et reconstruction de la page complète
  - Amélioration de l'interface avec section diagnostics et tests API
  - Ajout de boutons pour rafraîchir les diagnostics et afficher les périphériques I2C
- **Intervalle météo :** Réduction de l'intervalle de mise à jour météo de 15 minutes à 2 minutes
  - `INTERVAL_WEATHER` passé de 900000 ms (15 min) à 120000 ms (2 min)
  - Permet une actualisation plus fréquente des données météo
- **Stabilité :** Amélioration de la réactivité du système avec des mises à jour plus fréquentes

## [2.8.3] - 2025-11-24
### Correctif : Robustesse initialisation I2C
- **AHT20 :** Ajout d'un flag `ahtAvailable` et protection des lectures AHT20 si le capteur n'est pas présent ou échoue à l'initialisation.
  - Évite l'erreur runtime I2C : `requestFrom(): i2cRead returned Error -1`.
  - Température/humidité marquées `NAN` si capteur absent, le système continue de fonctionner.
- **Bump version :** Passage en `2.8.3` et mise à jour des docs utilisateur.

## [2.8.2] - 2025-11-24
### Correctif : Erreur I2C et réactivation des alertes météo
- **Erreur I2C :** Correction de l'erreur `i2cRead returned Error -1`
  - Ajout d'un flag `bmpAvailable` pour tracker si le BMP280 est initialisé
  - Ne tente de lire le BMP280 que s'il est disponible
  - Affiche un message clair si le BMP280 n'est pas trouvé
  - Évite les appels I2C inutiles qui causaient l'erreur
- **Alertes météo :** Réactivation de la récupération des alertes
  - Utilise One Call API 3.0 (`/data/3.0/onecall`)
  - Affiche les alertes si disponibles (nécessite abonnement)
  - Gère proprement les codes d'erreur (401 = pas d'accès)
  - Logs détaillés pour diagnostic
- **Stabilité :** Amélioration de la robustesse du système avec gestion des périphériques manquants

## [2.8.1] - 2025-11-24
### Correctif : Calcul des prévisions et logs de débogage
- **Prévisions :** Correction du calcul des min/max par jour (valeurs 999/-999 corrigées)
  - Nouveau calcul basé sur les index : 8 prévisions 3h = 1 jour
  - Index 0-7: Aujourd'hui, 8-15: Demain, 16-23: J+2, 24-31: J+3
  - Logs détaillés de chaque prévision pour diagnostic
- **Débogage :** Ajout de logs au démarrage pour vérifier :
  - Statut WiFi avant récupération météo
  - SSID et IP si connecté
  - Confirmation d'appel et de complétion de fetchWeather()
  - Confirmation d'appel et de complétion de updateSensors()
- **Stabilité :** Résolution du problème d'affichage de valeurs incorrectes (999)

## [2.8.0] - 2025-11-24
### Changement Majeur : Migration vers API OpenWeatherMap Gratuite
- **API OpenWeatherMap :** Migration complète vers les API gratuites :
  - **Météo actuelle :** Utilisation de l'API `/data/2.5/weather` au lieu de `onecall`
  - **Prévisions :** Utilisation de l'API `/data/2.5/forecast` (prévisions 3h sur 5 jours)
  - **Raison :** L'API One Call 2.5 nécessite maintenant un abonnement payant
- **Décodage JSON :** Adaptation complète du décodage pour les nouvelles structures de réponse :
  - Extraction de `main.temp`, `main.pressure` pour la météo actuelle
  - Calcul des min/max par jour à partir des prévisions 3h
  - Logs de débogage détaillés pour chaque étape
- **Alertes météo :** Temporairement désactivées (nécessitent One Call API 3.0 avec abonnement)
- **Stabilité :** Correction du problème de valeurs à zéro grâce au nouveau décodage JSON correct

## [2.7.1] - 2025-11-24
### Correctif : Alertes Météo et Débogage OpenWeatherMap
- **Page Alertes :** Modification de l'affichage "NOMINAL" → "AUCUNE" pour plus de clarté quand aucune alerte n'est active.
- **OpenWeatherMap :** Ajout de logs de débogage complets pour diagnostiquer les problèmes de récupération des alertes :
  - Affichage des coordonnées GPS utilisées (valides ou par défaut)
  - Logs détaillés de la requête HTTP et de la réponse JSON
  - Vérification explicite de la présence d'alertes dans la réponse
  - Comptage du nombre d'alertes trouvées
- **Coordonnées :** Utilisation explicite des coordonnées GPS valides ou des coordonnées par défaut de Bordeaux (44.8378, -0.5792).
- **Priorité API :** Confirmation qu'OpenWeatherMap est toujours utilisé en priorité pour obtenir les alertes météo officielles.

## [2.7.0] - 2025-11-24
### Amélioration Majeure : Réorganisation des Pages
- **Navigation :** Réorganisation complète de l'ordre des pages pour une meilleure expérience utilisateur.
  - **Page 0 :** Résumé (inchangé)
  - **Page 1 :** Prévisions (anciennement page 2)
  - **Page 2 :** Environnement (anciennement page 1)
  - **Page 3 :** **NOUVELLE** - Alertes Météo (page dédiée)
  - **Page 4 :** Données GPS (anciennement page 3)
  - **Page 5 :** Réseau & Système (anciennement page 4)
  - **Page 6 :** État Système (anciennement page 5)
- **Alertes :** Nouvelle page dédiée aux alertes météorologiques avec affichage visuel amélioré.
  - Affiche les alertes provenant d'OpenWeatherMap en temps réel.
  - Interface visuelle optimisée avec icône d'alerte rouge ou statut nominal vert.
- **UX :** Ordre des pages plus logique : Résumé → Prévisions → Environnement → Alertes → GPS → Réseau → Système.

## [2.6.4] - 2025-11-23
-### Correctif : Telegram & UX
- **Telegram :** Envoi au démarrage d'un message de démarrage court ; le premier rapport complet est envoyé automatiquement lorsque les mesures (capteurs / météo) sont disponibles.
- **Commandes Telegram :** Ajout de `/help`, `/weather`, `/system`, `/gps`, `/alerts`, amélioration de `/status` et ajout de `/getreport` pour demander le rapport complet à la demande.
- **Sécurité :** Le bot ne répond qu'au `TELEGRAM_CHAT_ID` configuré.

## [2.6.2] - 2025-11-23
### Correctif : UX & Fun
- **Nouveau :** Ajout du "Party Mode" déclenché par un appui long sur le bouton Action — animation NeoPixel arc-en-ciel et petite mélodie.
- **Docs :** Mise à jour du numéro de version à `2.6.4` dans la documentation utilisateur.

## [2.6.0] - 2025-11-22
### Amélioration Majeure (Fiabilité)
- **Qualité de l'Air (AQI) :** Remplacement total du fournisseur de données.
    - **Avant :** OpenWeatherMap (Nécessitait une clé API, causait souvent des erreurs "N/A").
    - **Maintenant :** **Open-Meteo Air Quality API**.
    - **Avantage :** Gratuit, sans clé API, données basées sur le modèle Européen CAMS.
- **Logique :** Conversion automatique de l'indice CAQI (0-100) fourni par Open-Meteo vers l'échelle simple (1-5) utilisée par l'affichage de la station.

## [2.5.2] - 2025-11-22
### Correctifs (Patch)
- **Débogage :** Ajout de traces séries (`Serial.print`) détaillées pour diagnostiquer les échecs de requêtes AQI (OpenWeather) et la lecture du capteur de lumière.
- **Matériel (Hotfix) :** Changement de la broche par défaut du capteur de lumière (LDR) de `9` à `4` dans `config.h`.
    - *Raison :* La GPIO 9 est souvent utilisée par le Flash interne sur les modules ESP32-S3, rendant la lecture analogique impossible (lecture à 0V constant).
- **Cosmétique :** Clarification de l'unité "lx" (Lux) sur l'affichage, souvent confondue avec "1x".

## [2.5.1] - 2025-11-22
### Correctifs (Hotfix)
- **Compilation :** Ajout de la fonction manquante `drawTrendArrow()` dans `weather_graphics.h`.
    - Résout l'erreur `'drawTrendArrow' was not declared in this scope`.
    - Réintégration des définitions de couleurs `FLAT_GREEN` et `FLAT_RED`.

## [2.5.0] - 2025-11-22
### Retour à la Stabilité (Rollback)
- **Suppression OTA :** Retrait complet de la fonctionnalité de mise à jour sans fil (`ElegantOTA`) qui causait des conflits de dépendances insolubles avec le serveur web.
- **Bibliothèques :** Retour aux versions stables des bibliothèques réseau (`esphome/ESPAsyncWebServer` et `esphome/AsyncTCP`) qui fonctionnaient avant la tentative de mise à jour.
- **Système :** Le code est désormais identique à la v2.3.1 (stable), prêt pour une utilisation quotidienne fiable.

## [2.4.4] - 2025-11-22
### Maintenance Critique
- **Bibliothèques :** Verrouillage strict des versions `mathieucarbou/ESPAsyncWebServer` (v3.6.0) et `mathieucarbou/AsyncTCP` (v3.3.2).
- **Nettoyage :** Procédure de suppression du cache `.pio` requise pour éliminer les conflits de types (`const AsyncServer`) hérités des anciennes bibliothèques `esphome`.

## [2.4.3] - 2025-11-22
### Correctifs Techniques (Compilation)
- **Dépendances :** Remplacement des bibliothèques `ESPAsyncWebServer` et `AsyncTCP` (forks `esphome`) par les forks de **`mathieucarbou`**.
    - Résout l'erreur critique de compilation C++ (`discarding qualifiers` / `const` correctness).
    - Assure une meilleure compatibilité avec le Core ESP32 récent.
- **Code Web :** Remplacement de la méthode dépréciée `request->send_P()` par `request->send()`. La gestion du stockage PROGMEM est désormais automatique.

## [2.4.2] - 2025-11-22
### Correctifs de Compilation
- **OTA / Conflits :** Résolution des erreurs "conflicts with a previous declaration" (HTTP_GET, HTTP_POST).
    - Réorganisation stricte de l'ordre des `#include` dans `main.cpp`.
    - Définition forcée du flag `ELEGANTOTA_USE_ASYNC_WEBSERVER` avant l'inclusion de la librairie.
	
## [2.4.1] - 2025-11-22
### Correctifs Critiques (Compilation OTA)
- **ElegantOTA :** Correction du conflit de types entre `AsyncWebServer` et `WebServer` standard.
    - Ajout du flag `-DELEGANTOTA_USE_ASYNC_WEBSERVER=1` manquant dans la configuration de build.
    - Ce flag force la bibliothèque OTA à compiler en mode compatible Asynchrone, résolvant l'erreur `no matching function for call to begin`.
- **Conflits HTTP :** Nettoyage des conflits de déclaration d'énumérations (`HTTP_GET`, `HTTP_POST`...) entre le SDK ESP32 et la bibliothèque AsyncWeb.

## [2.4.0] - 2025-11-22
### Fonctionnalités Majeures
- **Mise à jour Sans Fil (OTA) :** Intégration de `ElegantOTA`.
    - Interface de mise à jour accessible via navigateur web à l'adresse `http://<IP>/update`.
    - Permet de téléverser les futurs firmwares (`firmware.bin`) sans connexion USB.

### Correctifs
- **Qualité de l'Air (AQI) :** Correction de l'erreur "N/A".
    - Cause : Échec de la validation SSL lors de la requête HTTPS vers OpenWeatherMap.
    - Solution : Implémentation explicite d'un `WiFiClientSecure` en mode `setInsecure()` pour cette requête spécifique.

### Technique
- **Dépendances :** Ajout de la bibliothèque `ayushsharma82/ElegantOTA`.

## [2.3.1] - 2025-11-22
### Correctifs (Patch)
- **Compilation :** Correction de l'erreur critique `TELEGRAM_CERTIFICATE_ROOT`. Remplacement par `clientSEC.setInsecure()` pour s'adapter à la nouvelle bibliothèque Telegram.
- **JSON :** Correction des méthodes dépréciées (`createNestedArray`) pour une compatibilité stricte avec ArduinoJson v7.
- **Versionning :** Incrémentation du numéro de patch suite aux correctifs de stabilité.

## [2.3.0] - 2025-11-22
### Interface & Ergonomie (UX)
- **Notifications Visuelles (Toast) :** Remplacement des messages textes simples par une **boîte de dialogue centrée** (Pop-up) pour les confirmations d'action (Changement de luminosité, Mise à jour).
    - Apparaît sous l'entête, texte noir sur fond blanc pour une lisibilité maximale.
    - Nettoyage parfait de la zone après affichage pour éviter les artefacts graphiques.
- **Gestion Rétroéclairage :**
    - Passage de la fréquence PWM à **12 kHz** (plus stable pour le rétroéclairage LED).
    - Garantie logicielle que le mode "MAX" envoie la valeur brute 255 au contrôleur.

### Correctifs
- **Affichage :** Correction du positionnement des messages de confirmation qui s'affichaient parfois sur deux lignes.

## [2.2.0] - 2025-11-22
### Expérience Utilisateur (UX)
- **Gestion Rétroéclairage :** Modification du comportement du Bouton 2 (Action).
    - Appui court : Bascule entre le mode **Automatique** (capteur LDR) et le mode **Manuel** (Luminosité 100% force).
    - Un bip distinctif indique le changement (1 bip = Auto, 2 bips = Max).
- **Texte & Encodage :**
    - Amélioration de la fonction `cleanText()` pour supporter les **Majuscules Accentuées** (É, È, À, Ù...).
    - Application systématique du correcteur sur tous les titres de pages qui s'affichaient mal.

### Suppression
- **Bouton Action :** Suppression du déclenchement manuel de la mise à jour météo sur le bouton 2 (remplacé par la gestion lumière). La mise à jour reste automatique via timer.

## [2.1.1] - 2025-11-22
### Correctifs
- **Versionning :** Correction de la numérotation de version (fusion des correctifs de maintenance 2.0.x et des ajouts graphiques de la 2.1.0).
- **Code :** Consolidation des correctifs de syntaxe JSON et de la gestion graphique vectorielle.

## [2.1.0] - 2025-11-22
### Interface & Graphisme
- **Indicateur de Tendance :** Remplacement des indicateurs textuels (sujets aux bugs d'encodage) par des **flèches graphiques vectorielles**.
    - ↗ Hausse : Flèche Verte dessinée vers le haut-droite.
    - ↘ Baisse : Flèche Rouge dessinée vers le bas-droite.
    - → Stable : Flèche Grise horizontale.
- **Gestion du Texte :** Introduction de la fonction `cleanText()` pour gérer l'encodage des caractères.
    - Conversion automatique des accents UTF-8 (é, è, à...) en ASCII standard.
    - Remplacement du symbole degré (°) par le caractère spécifique de la police GFX (`char 247`).

## [2.0.2] - 2025-11-22
### Maintenance Code
- **JSON API (ArduinoJson v7) :** Mise à jour stricte de la syntaxe pour éliminer les avertissements de compilation ("Deprecated").
    - Remplacement de `createNestedArray` par `.to<JsonArray>()`.
    - Remplacement de `createNestedObject` par `.add<JsonObject>()`.

## [2.0.1] - 2025-11-22
### Correctifs Critiques
- **Compilation :** Suppression définitive des variables obsolètes (`TELEGRAM_CERTIFICATE_ROOT`) issues des anciennes versions.
- **Configuration :** Ajout du flag `-D ARDUINOJSON_DEPRECATED_WARNINGS=0` pour nettoyer les logs de compilation liés aux dépendances externes non mises à jour.

## [2.0.0] - 2025-11-22
### VERSION MAJEURE (Major Release)
### Nouvelles Fonctionnalités
- **Luminosité Adaptative (Auto-Dimming) :** Le rétroéclairage de l'écran s'ajuste désormais automatiquement et fluidement en fonction de la lumière ambiante captée par la LDR.
- **Indicateur de Tendance :** Analyse de la pression atmosphérique sur 1 heure pour déterminer la tendance météo locale (Hausse/Baisse/Stable).
- **Dashboard Web 2.0 :** Refonte totale de l'interface Web (HTML5/CSS Grid).
    - Design "Dark Mode" réactif.
    - Affichage complet : Jauges, AQI coloré, Prévisions J+3, Infos GPS et Système.
    - Rafraîchissement automatique (AJAX) sans rechargement de page.

### Technique
- **Matériel :** Activation d'un second canal PWM (LEDC) pour le pilotage du rétroéclairage (`TFT_BL`).
- **API :** Enrichissement du JSON de sortie pour inclure les données de tendance, d'uptime et les tableaux de prévisions structurés.

## [1.9.3] - 2025-11-22
### Correctifs
- **SSL/Telegram :** Passage à la méthode `clientSEC.setInsecure()` pour la connexion HTTPS Telegram. Élimine le besoin de maintenir un certificat racine (Root CA) à jour manuellement.
- **Json :** Correction d'un warning sur `containsKey` remplacé par la syntaxe v7 `is<JsonArray>()`.

## [1.9.2] - 2025-11-22
### Correctifs
- **Dépendance :** Migration vers la librairie `cotestatnt/AsyncTelegram2` (Fork maintenu et robuste).
- **SSL :** Adaptation de l'initialisation du certificat SSL selon les requis de la nouvelle librairie.

## [1.9.1] - 2025-11-22
### Correctifs
- **PlatformIO :** Correction d'une régression sur la bibliothèque graphique. Rétablissement de `Adafruit ST7735 and ST7789 Library` indispensable pour le contrôleur d'écran utilisé.

## [1.9.0] - 2025-11-22
### Architecture Logicielle
- **Telegram :** Remplacement complet de la bibliothèque obsolète `UniversalTelegramBot` par **`AsyncTelegram2`**.
    - Gestion totalement asynchrone (non-bloquante).
    - Compatibilité native avec ArduinoJson v7.
    - Meilleure gestion des sessions SSL.

## [1.8.1] - 2025-11-22
### Améliorations Données
- **Qualité de l'Air (AQI) :** Découplage de la requête AQI. Le système tente désormais de récupérer la pollution via OpenWeatherMap même si la météo principale bascule sur le fournisseur de secours (Open-Meteo).
- **Interface :** Affichage du nom complet du fournisseur de données ("OpenWeatherMap" ou "Open-Meteo") dans les pages de détails.

## [1.8.0] - 2025-11-22
### Architecture de Données (Dual-Provider)
- **OpenWeatherMap (OWM) :** Devient le fournisseur principal (API One Call). Apporte les alertes officielles et l'AQI.
- **Failover (Secours) :** Bascule automatique et transparente vers **Open-Meteo** en cas d'échec de connexion ou de quota dépassé chez OWM.
- **Compatibilité :** Création d'un convertisseur de codes météo (OWM ID vers WMO Code) pour unifier l'affichage graphique quelle que soit la source.

## [1.7.0] - 2025-11-21
### Performance & Stabilité
- **Réseau (Anti-Freeze) :** Ajout d'un **Timeout de 3000ms** sur les requêtes HTTP. Empêche le blocage de l'interface si le serveur météo ne répond pas.
- **UX :** Feedback visuel "MAJ..." immédiat lors de l'appui sur le bouton d'action.
- **GPS :** Optimisation de la lecture du buffer série pour une acquisition plus rapide.

## [1.6.0] - 2025-11-21
### Système & Stabilité
- **Entrées (Boutons) :** Ajout d'une période réfractaire logicielle (Debounce 300ms) pour éliminer les doubles clics involontaires et les sauts de page.
- **Page Système (Page 5) :** Nouvelle page de diagnostic exhaustive (Uptime, RAM libre, Fréquence CPU, RSSI dBm, État Alertes détaillé).

## [1.5.0] - 2025-11-21
### Interface & UX
- **Page Alertes :** Rendue accessible en permanence (Indicateur "Nominal" vert ou "Alerte" rouge). Affichage automatique (pop-up) conservé en cas de déclenchement critique.
- **Boot Screen :** Nouvelle animation de démarrage avec rotation vectorielle des rayons du soleil pendant la recherche WiFi.

## [1.4.0] - 2025-11-21
### Navigation
- **Réorganisation :** Ordre des pages optimisé pour l'ergonomie (Résumé > Env > Prévisions > GPS > Réseau > Système/Alertes).
- **Logique :** Gestion intelligente de l'affichage des alertes.

## [1.3.1] - 2025-11-21
### Design Graphique
- **Moteur Vectoriel :** Remplacement des bitmaps statiques par un moteur de rendu géométrique ("Flat Design").
- **Icônes :** Création de fonctions de dessin procédural pour : Soleil, Nuage, Eclaircies, Pluie, Orage, Neige, Brouillard.

## [1.3.0] - 2025-11-21
### Audio
- **Son :** Réécriture de la fonction `beep()` utilisant le PWM non-bloquant pour assurer l'audibilité des notifications (WiFi Connecté, GPS Fix, Boutons).

## [1.2.0] - 2025-11-21
### Fonctionnalités
- **Page Prévisions :** Ajout d'une page dédiée aux prévisions sur 3 jours (J+1, J+2, J+3) avec icônes et températures Min/Max.
- **API :** Extension des requêtes pour récupérer les données journalières (`daily`).

## [1.1.5] - 2025-11-21
### Capteurs & UX
- **Altimètre :** Calibration dynamique de l'altitude basée sur la pression niveau mer (QNH) locale récupérée via API. Corrige les erreurs d'altitude.
- **GPS :** Ajout du tag "SIMULATION" sur l'écran GPS lorsque les coordonnées par défaut sont utilisées.
- **Boot :** Premier design graphique de l'écran de démarrage.

## [1.1.4] - 2025-11-21
### Modernisation
- **Noyau :** Migration complète vers **ArduinoJson v7**. Refactorisation du code pour utiliser la nouvelle syntaxe JSON (allocation dynamique automatique).

## [1.1.2] - 2025-11-21
### Correctifs
- **GPS :** Correction des coordonnées par défaut (Bordeaux) pour éviter des données météo erronées (26°C) au démarrage avant le Fix GPS.

## [1.1.1] - 2025-11-21
### Matériel
- **Boutons :** Passage à la gestion par **Interruptions (ISR)** pour une réactivité immédiate.
- **Capteurs :** Ajout d'un scanner d'adresses I2C pour le BMP280 (support 0x76/0x77).

## [1.1.0] - 2025-11-21
### Fonctionnalités
- **Pages :** Ajout des pages de détails (Alertes, GPS, Réseau).
- **Connectivité :** Activation du Bot Telegram et du Serveur Web.

## [1.0.0] - 2025-11-21
### Version Initiale
- Lancement du projet. Support ESP32-S3, Écran ST7789, GPS GT-U7, Capteurs AHT20+BMP280, WiFi Multi.