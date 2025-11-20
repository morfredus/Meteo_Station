// telegram.h - gestion Telegram pour TFT ST7789 - VERSION FINALE
#pragma once
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "config.h"
#include "utils.h"

static WiFiClientSecure secured_client;
static UniversalTelegramBot bot(TELEGRAM_BOT_TOKEN, secured_client);

void initTelegram() {
  secured_client.setInsecure(); // pour ESP32, accepte le certificat
  Serial.println("[Telegram] initialise");
  
  // Vider la file d'attente des messages pour eviter les commandes fantomes
  delay(1000);
  int numOldMessages = bot.getUpdates(bot.last_message_received + 1);
  if (numOldMessages > 0) {
    Serial.printf("[Telegram] %d anciens messages effaces\n", numOldMessages);
  }
}

void sendTelegramMessage(const String &text) {
  bot.sendMessage(TELEGRAM_CHAT_ID, text, "");
}

void handleTelegramCommands() {
  // Protection contre les commandes repetees apres reboot
  if (rebootRequested) {
    return; // Ne traite pas les commandes si un reboot est en attente
  }
  
  int numNew = bot.getUpdates(bot.last_message_received + 1);
  if (numNew <= 0) return;
  
  for (int i = 0; i < numNew; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    Serial.println("[Telegram] msg: " + text);
    
    if (chat_id != String(TELEGRAM_CHAT_ID)) {
      bot.sendMessage(chat_id, "Acces refuse", "");
      continue;
    }
    
    // ===== COMMANDE HELP MISE A JOUR =====
    if (text == "/start" || text == "/help") {
      String helpMsg = "Station Meteo GPS v2.0\n\n";
      helpMsg += "Commandes de base:\n";
      helpMsg += "/help - Liste des commandes\n";
      helpMsg += "/status - Etat du systeme\n";
      helpMsg += "/meteo - Meteo actuelle\n";
      helpMsg += "/alerte - Dernieres alertes meteo\n";
      helpMsg += "/luminosite - Niveau de lumiere\n";
      helpMsg += "/gps - Informations GPS\n";
      helpMsg += "/reboot - Redemarrer le systeme\n";
      helpMsg += "/uptime - Temps de fonctionnement\n\n";
      
      helpMsg += "Alertes personnalisees:\n";
      helpMsg += "/setalert <type><op><val> [nom] - Creer alerte\n";
      helpMsg += "  Ex: /setalert temp>25 Canicule\n";
      helpMsg += "  Types: temp, hum, lux, aqi\n";
      helpMsg += "  Ops: > < =\n";
      helpMsg += "/listalerts - Voir les alertes actives\n";
      helpMsg += "/clearalerts - Supprimer toutes alertes\n\n";
      
      helpMsg += "Historique & Stats:\n";
      helpMsg += "/history [heures] - Historique (defaut 1h)\n";
      helpMsg += "/stats - Min/Max/Moyenne du jour\n";
      helpMsg += "/graph <type> - Graphique ASCII\n";
      helpMsg += "  Types: temp, hum, lux\n\n";
      
      helpMsg += "Previsions & Air:\n";
      helpMsg += "/previsions - Previsions 5 jours\n";
      helpMsg += "/air - Qualite de l'air (AQI)\n\n";
      
      bot.sendMessage(chat_id, helpMsg, "Markdown");
      
    // ===== COMMANDES STATUS ET METEO (EXISTANTES) =====
    } else if (text == "/status") {
      String status = "Etat du systeme :\n";
      status += "WiFi : " + String(WiFi.SSID()) + "\n";
      status += "IP : " + WiFi.localIP().toString() + "\n";
      status += "GPS : " + String(isGPSAcquired ? "OK" : "Non") + "\n";
      status += "Temp DHT : " + String(dhtTemp) + " C\n";
      status += "Hum DHT : " + String(dhtHum) + " %\n";
      status += "Luminosite : " + String(lightLevel) + "\n";
      status += "Alertes actives : " + String(activeAlertCount) + "/10\n";
      status += "Historique : " + String(historyCount) + " mesures\n";
      bot.sendMessage(chat_id, status, "");
      
    } else if (text == "/meteo") {
      String msg = "Meteo : " + weatherDescription + " (" + String(temperature) + " C)\n";
      msg += "Temp capteur: " + String(dhtTemp) + " C\n";
      msg += "Humidite: " + String(dhtHum) + " %\n";
      msg += "Luminosite: " + String(lightLevel) + "\n";
      if (airQualityAvailable) {
        msg += "Qualite air: AQI " + String(airQuality.aqi) + "\n";
      }
      bot.sendMessage(chat_id, msg, "");
      
      // Indication LED pour commande meteo
      ledBlue();
      beepFast(1);
      delay(500);
      ledOff();
      
    } else if (text == "/alerte") {
      bot.sendMessage(chat_id, "Alerte : " + weatherAlert + " [" + alertColor + "]", "");
      
    } else if (text == "/luminosite") {
      bot.sendMessage(chat_id, "Luminosite actuelle : " + String(lightLevel), "");
      
    // ===== NOUVELLES COMMANDES ALERTES =====
    } else if (text.startsWith("/setalert ")) {
      String alertCmd = text.substring(10);
      if (addAlert(alertCmd)) {
        bot.sendMessage(chat_id, "Alerte ajoutee avec succes", "");
        ledGreen();
        beep(2);
        delay(500);
        ledOff();
      } else {
        bot.sendMessage(chat_id, "Erreur: Format invalide ou limite atteinte (10 max)", "");
        ledRed();
        beepFast(3);
        delay(500);
        ledOff();
      }
      
    } else if (text == "/listalerts") {
      bot.sendMessage(chat_id, listAlerts(), "");
      
    } else if (text == "/clearalerts") {
      clearAlerts();
      bot.sendMessage(chat_id, "Toutes les alertes ont ete supprimees", "");
      ledYellow();
      beepLong();
      delay(500);
      ledOff();
      
    // ===== NOUVELLES COMMANDES HISTORIQUE =====
    } else if (text.startsWith("/history")) {
      int hours = 1;
      if (text.length() > 9) {
        hours = text.substring(9).toInt();
        if (hours <= 0 || hours > 24) hours = 1;
      }
      bot.sendMessage(chat_id, getHistory(hours), "");
      
    } else if (text == "/stats") {
      bot.sendMessage(chat_id, getStats(), "");
      
    } else if (text.startsWith("/graph ")) {
      String type = text.substring(7);
      type.toLowerCase();
      if (type == "temp" || type == "hum" || type == "lux") {
        bot.sendMessage(chat_id, drawASCIIGraph(type), "Markdown");
      } else {
        bot.sendMessage(chat_id, "Types valides: temp, hum, lux", "");
      }
      
    // ===== NOUVELLES COMMANDES PREVISIONS ET AIR =====
    } else if (text == "/previsions") {
      bot.sendMessage(chat_id, getExtendedForecast(), "");
      
    } else if (text == "/air") {
      bot.sendMessage(chat_id, getAirQualityReport(), "");
      
    // ===== COMMANDES GPS ET SYSTEME (EXISTANTES) =====
    } else if (text == "/gps") {
      String gpsMsg = "Informations GPS :\n\n";
      
      if (isGPSAcquired && gps.location.isValid()) {
        // GPS actif avec position valide
        gpsMsg += "Statut: SYNCHRONISE\n";
        gpsMsg += "Latitude: " + String(gps.location.lat(), 6) + "\n";
        gpsMsg += "Longitude: " + String(gps.location.lng(), 6) + "\n";
        gpsMsg += "Altitude: ";
        if (gps.altitude.isValid()) {
          gpsMsg += String(gps.altitude.meters(), 1) + " m\n";
        } else {
          gpsMsg += "Non disponible\n";
        }
        gpsMsg += "Satellites: " + String(gps.satellites.value()) + "\n";
        gpsMsg += "HDOP: " + String(gps.hdop.hdop(), 2) + "\n";
        gpsMsg += "Vitesse: ";
        if (gps.speed.isValid()) {
          gpsMsg += String(gps.speed.kmph(), 1) + " km/h\n";
        } else {
          gpsMsg += "0 km/h\n";
        }
        gpsMsg += "Cap: ";
        if (gps.course.isValid()) {
          gpsMsg += String(gps.course.deg(), 1) + "\n";
        } else {
          gpsMsg += "Non disponible\n";
        }
        if (gps.date.isValid() && gps.time.isValid()) {
          gpsMsg += "Date GPS: " + String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year()) + "\n";
          gpsMsg += "Heure GPS: " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second()) + " UTC\n";
        }
        gpsMsg += "\nCode INSEE: " + dynamicInsee;
      } else if (gpsBackgroundSyncInProgress) {
        // Synchronisation en cours
        gpsMsg += "Statut: SYNCHRONISATION EN COURS\n";
        gpsMsg += "Position actuelle: Bordeaux (defaut)\n";
        gpsMsg += "Latitude: " + WB_LAT + " (defaut)\n";
        gpsMsg += "Longitude: " + WB_LON + " (defaut)\n";
        gpsMsg += "Satellites visibles: ";
        if (gps.satellites.isValid()) {
          gpsMsg += String(gps.satellites.value()) + "\n";
        } else {
          gpsMsg += "0\n";
        }
        if (gps.hdop.isValid()) {
          gpsMsg += "HDOP: " + String(gps.hdop.hdop(), 2) + "\n";
        }
        unsigned long nextTry = (lastGPSBackgroundCheck + GPS_BACKGROUND_RETRY_INTERVAL - millis()) / 1000;
        if (nextTry > 0) {
          gpsMsg += "Prochaine tentative dans: " + String(nextTry) + "s\n";
        } else {
          gpsMsg += "Prochaine tentative: maintenant\n";
        }
        gpsMsg += "\nCode INSEE: " + dynamicInsee + " (defaut)";
      } else {
        // GPS non acquis
        gpsMsg += "Statut: NON SYNCHRONISE\n";
        gpsMsg += "Position: Bordeaux (par defaut)\n";
        gpsMsg += "Latitude: " + WB_LAT + " (defaut)\n";
        gpsMsg += "Longitude: " + WB_LON + " (defaut)\n";
        gpsMsg += "Satellites: 0\n";
        gpsMsg += "\nCode INSEE: " + dynamicInsee + " (defaut)";
      }
      
      bot.sendMessage(chat_id, gpsMsg, "");
      
    } else if (text == "/reboot") {
      rebootRequested = true; // Marquer qu'un reboot est demande
      bot.sendMessage(chat_id, "Redemarrage en cours...", "");
      Serial.println("[Telegram] Commande reboot recue - redemarrage programme...");
      
      // Afficher le message sur l'ecran TFT
      display.fillScreen(COLOR_BLACK);
      display.setTextSize(2);
      display.setTextColor(COLOR_RED);
      
      int16_t x1, y1;
      uint16_t w, h;
      String title = "REBOOT";
      display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 80);
      display.println(title);
      
      display.setTextSize(1);
      display.setTextColor(COLOR_WHITE);
      String subtitle = "Redemarrage...";
      display.getTextBounds(subtitle, 0, 0, &x1, &y1, &w, &h);
      display.setCursor((SCREEN_WIDTH - w) / 2, 120);
      display.println(subtitle);
      
      // Signal LED et sonore
      ledRed();
      beepAlert();
      
      delay(2000); // Delai pour l'envoi du message
      ESP.restart();
      
    } else if (text == "/uptime") {
      unsigned long up = millis() / 1000;
      String s = String(up / 3600) + "h " + String((up % 3600) / 60) + "m " + String(up % 60) + "s";
      bot.sendMessage(chat_id, "Uptime: " + s, "");
      
    } else {
      bot.sendMessage(chat_id, "Commande inconnue. Tape /help pour la liste.", "");
      ledRed();
      beepFast(2);
      delay(500);
      ledOff();
    }
  }
}