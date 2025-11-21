# FAQ

**Q: Mon altitude indique -110m, pourquoi ?**
R: Cela arrive si le BMP280 utilise une pression standard statique (1013.25 hPa) alors que la pression réelle est haute (anticyclone). **Mettez à jour en v1.1.5+** : Nous utilisons maintenant une calibration dynamique via l'API Météo pour corriger cela.

**Q: Le GPS indique "NO FIX" ou "SIMULATION".**
R: Les modules GPS comme le GT-U7 nécessitent une vue dégagée du ciel. En intérieur, ils échouent souvent à capter les satellites. Le système utilise les coordonnées de Bordeaux par défaut tant que le signal n'est pas trouvé.

**Q: Les boutons ne répondent pas instantanément.**
R: Assurez-vous d'utiliser la version **v1.1.1** ou supérieure, qui a implémenté les Interruptions (ISR) pour la gestion des boutons.