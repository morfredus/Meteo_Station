#### `CONFIG_FR.md` (Français)
```markdown
# Configuration

## Fichier Secrets
**IMPORTANT :** Vous devez créer un fichier nommé `secrets.h` dans le dossier `include/`. Ce fichier est ignoré par git pour la sécurité.

Copiez le modèle suivant dans `include/secrets.h`:

```cpp
#ifndef SECRETS_H
#define SECRETS_H

// Identifiants WiFi (Support Multi-AP)
const char* ssid_1 = "VOTRE_SSID_1";
const char* password_1 = "VOTRE_MDP_1";
const char* ssid_2 = "VOTRE_SSID_2";
const char* password_2 = "VOTRE_MDP_2";

// Telegram Bot
#define TELEGRAM_BOT_TOKEN "VOTRE_TOKEN_BOT_ICI"
#define TELEGRAM_CHAT_ID "VOTRE_CHAT_ID_ICI"

#endif