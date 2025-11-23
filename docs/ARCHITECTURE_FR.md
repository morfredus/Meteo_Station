# Architecture du Projet

Dernière version : **v2.6.3** — Fournisseur AQI changé vers Open‑Meteo Air Quality (gratuit, sans clé API). Le projet utilise toujours les forks réseau stables.

Ce document donne un aperçu de l'architecture logicielle de la station météo.

## Concepts Clés

### Boucle Principale & Machine à États
Le système est construit autour d'une boucle principale non-bloquante (`loop()`). Une machine à états gère la page actuellement affichée à l'écran. Les entrées utilisateur (appui sur les boutons) déclenchent des transitions d'état pour naviguer entre les pages.

### Gestion des Entrées
- **Boutons sur Interruptions :** Les boutons sont gérés via des interruptions (ISR) pour une réactivité instantanée sans bloquer la boucle principale. [1]
- **Anti-Rebond Logiciel :** Une période réfractaire de 300ms est implémentée dans la boucle principale pour éviter que les rebonds mécaniques ou les parasites ne provoquent des sauts de page multiples sur un seul appui. [2]

### Réseau & APIs
- **API Open-Meteo :** Utilisée pour récupérer la météo actuelle, les prévisions journalières et la pression au niveau de la mer (QNH) pour le calibrage de l'altitude.
- **Timeout sur Requêtes HTTP :** Toutes les requêtes vers l'API météo ont un **timeout de 3000ms** pour empêcher l'appareil de se figer si le réseau est lent ou si le serveur ne répond pas. [3]
- **Serveur Web Local :** Un `ESPAsyncWebServer` fournit une interface web et une API JSON.
- **API JSON (`/api/data`) :** Expose les données des capteurs et les diagnostics système en temps réel.
- **Bot Telegram :** Permet le contrôle à distance et la consultation de l'état.

### Moteur Graphique
- **"Flat Design" Procédural :** Les icônes météo sont dessinées de manière procédurale (vectorielle) plutôt qu'en utilisant des fichiers bitmaps statiques. Le résultat est un affichage net, adaptable et qui réduit l'espace de stockage nécessaire. [4]
- **Bibliothèques GFX :** Les bibliothèques Adafruit GFX et ST7789 constituent la base du pilote d'affichage.

### Stabilité & Diagnostics
- **Audio Non-Bloquant :** La fonction `beep()` utilise une logique PWM non-bloquante, garantissant que les notifications sonores ne stoppent pas les autres processus. [5]
- **Page Système :** Une page dédiée (Page 5) affiche des diagnostics système critiques : Uptime (temps de fonctionnement), Mémoire Heap libre, Fréquence CPU et RSSI WiFi. [6]
- **Alertes Intelligentes :** La page d'alertes est toujours accessible mais s'affiche aussi automatiquement et immédiatement si une alerte critique est déclenchée. [7]

### Gestion des Capteurs
- **Calibration Dynamique de l'Altitude :** L'altitude barométrique est calibrée dynamiquement en utilisant la pression au niveau de la mer (QNH) fournie par l'API Open-Meteo pour une meilleure précision. [8]
- **Découverte I2C :** Le code vérifie automatiquement la présence du capteur BMP280 aux deux adresses I2C communes (0x76 et 0x77). [1]