# Dernière version : **v2.6.3** — Fournisseur AQI changé vers Open‑Meteo Air Quality (gratuit, sans clé API).

# Référence API

## API JSON Locale

L'appareil héberge un serveur web qui expose un point de terminaison d'API JSON pour les données en temps réel.

**Point de terminaison :** `GET /api/data`

### Corps de la Réponse

L'objet JSON contient les lectures des capteurs et l'état du système.

```json
{
  "temperature": 23.5,
  "humidity": 45.8,
  "pressure": 1013.25,
  "altitude": 150.5,
  "gps_lat": 48.8584,
  "gps_lon": 2.2945,
  "gps_sats": 8,
  "uptime": "1j 02:30:45",
  "free_heap": 123456,
  "cpu_freq": 240,
  "wifi_rssi": -55
}
```

## APIs Externes

- **Open-Meteo (météo) :** Utilisée pour récupérer les données de prévisions météo. La requête inclut le paramètre `daily` pour obtenir les prévisions sur 3 jours.
- **Open‑Meteo Air Quality :** Utilisée désormais comme fournisseur AQI principal (sans clé API). L'indice AQI est converti sur l'échelle 1–5 utilisée par la station.