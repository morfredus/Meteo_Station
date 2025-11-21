
### 2. `API_REFERENCE_FR.md` (Français)

```markdown
# Référence API

La Station Météo ESP32-S3 expose un serveur Web local fournissant les données en temps réel au format JSON.

## URL de base
`http://<ADRESSE_IP_DE_L_ESP32>`

---

## Points d'accès HTTP (Endpoints)

### `GET /api/data`
Renvoie toutes les mesures des capteurs, les données météo locales, le statut GPS et les alertes système dans un objet JSON unique.

**Structure de la réponse :**

```json
{
  "sensor": {
    "temp": 22.5,      // Température Intérieure (C)
    "hum": 45,         // Humidité Intérieure (%)
    "pres": 1013.2,    // Pression (hPa) - Optionnel selon init capteur
    "lux": 1200        // Niveau de luminosité (0-4095)
  },
  "weather": {
    "temp": 21.0,      // Température Extérieure via API (C)
    "code": 1          // Code Météo WMO (0=Clair, 1-3=Nuageux, etc.)
  },
  "gps": {
    "lat": 44.8378,    // Latitude
    "lon": -0.5792,    // Longitude
    "valid": true      // true si Fix GPS réel, false si simulation/défaut
  },
  "alert": false       // true si une alerte de température est active
}
````

**Codes de statut :**

  * `200 OK`: Requête réussie.

-----

## Commandes Bot Telegram

Vous pouvez interagir avec la station via le Chat ID Telegram configuré dans `secrets.h`.

| Commande | Description |
| :--- | :--- |
| `/start` | Vérifie la connexion et renvoie la version actuelle du firmware. |
| `/status` | Renvoie un résumé texte immédiat des capteurs internes et de la météo externe. |
| `/reboot` | Redémarre le module ESP32 à distance. |

```