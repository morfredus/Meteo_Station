#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Meteo Station S3 - Dashboard</title>
  <style>
    :root { --bg: #121212; --card: #1e1e1e; --text: #e0e0e0; --accent: #00a8ff; --warn: #ff9f43; --danger: #ff5252; --success: #1dd1a1; }
    body { font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif; background: var(--bg); color: var(--text); margin: 0; padding: 20px; }
    .container { max-width: 800px; margin: 0 auto; }
    h1 { text-align: center; color: var(--accent); margin-bottom: 5px; }
    .subtitle { text-align: center; color: #888; font-size: 0.9rem; margin-bottom: 30px; }
    
    .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 15px; }
    .card { background: var(--card); padding: 20px; border-radius: 12px; box-shadow: 0 4px 6px rgba(0,0,0,0.3); text-align: center; }
    .card h3 { margin: 0 0 10px 0; font-size: 1rem; color: #aaa; text-transform: uppercase; letter-spacing: 1px; }
    .value { font-size: 2.5rem; font-weight: 700; margin: 10px 0; }
    .unit { font-size: 1rem; color: #666; font-weight: normal; }
    .sub-value { font-size: 0.9rem; color: #888; margin-top: 5px; }
    
    /* Indicateurs spécifiques */
    .aqi-badge { padding: 5px 10px; border-radius: 20px; font-weight: bold; color: #000; display: inline-block; margin-top: 5px;}
    
    .alert-box { background: rgba(255, 82, 82, 0.1); border: 1px solid var(--danger); color: var(--danger); padding: 15px; border-radius: 8px; margin-bottom: 20px; display: none; text-align: center; animation: pulse 2s infinite; }
    
    .footer { margin-top: 40px; text-align: center; font-size: 0.8rem; color: #555; border-top: 1px solid #333; padding-top: 20px; }
    
    @keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.7; } 100% { opacity: 1; } }
  </style>
</head>
<body>

<div class="container">
  <h1>Station Météo S3</h1>
  <div class="subtitle">Dashboard Temps Réel • <span id="uptime">--</span></div>

  <div id="alertPanel" class="alert-box">
    <strong>⚠️ ALERTE MÉTÉO ACTIVE</strong><br>
    <span id="alertMsg">--</span>
  </div>

  <div class="grid">
    <div class="card">
      <h3>Intérieur</h3>
      <div class="value"><span id="inTemp">--</span><span class="unit">°C</span></div>
      <div class="sub-value">Hum: <span id="inHum">--</span>%</div>
    </div>
    
    <div class="card">
      <h3>Atmosphère</h3>
      <div class="value"><span id="inPres">--</span><span class="unit">hPa</span></div>
      <div class="sub-value">Tendance: <span id="trend">--</span></div>
    </div>

    <div class="card">
      <h3>Ambiance</h3>
      <div class="value"><span id="inLux">--</span><span class="unit">lux</span></div>
      <div class="sub-value">Luminosité</div>
    </div>
  </div>

  <h2 style="margin-top:30px; font-size:1.2rem; border-bottom:1px solid #333; padding-bottom:10px;">Extérieur & Prévisions</h2>
  
  <div class="grid">
    <div class="card" style="border-top: 3px solid var(--accent);">
      <h3>Extérieur</h3>
      <div class="value"><span id="outTemp">--</span><span class="unit">°C</span></div>
      <div class="sub-value" id="weatherDesc">--</div>
    </div>

    <div class="card">
      <h3>Qualité Air</h3>
      <div class="value" id="aqiVal">--</div>
      <div id="aqiText" class="aqi-badge" style="background:#555;">--</div>
    </div>

    <div class="card">
      <h3>GPS / Réseau</h3>
      <div style="font-size:0.9rem; text-align:left; line-height:1.6;">
        Lat: <span id="lat" style="color:#fff">--</span><br>
        Lon: <span id="lon" style="color:#fff">--</span><br>
        Sats: <span id="sats" style="color:var(--success)">--</span><br>
        Source: <span id="provider" style="color:var(--warn)">--</span>
      </div>
    </div>
  </div>

  <div class="grid" style="margin-top:15px;">
    <div class="card">
        <h3>Demain</h3>
        <div style="font-size:1.5rem"><span id="f1max" style="color:var(--danger)">--</span>° / <span id="f1min" style="color:var(--accent)">--</span>°</div>
    </div>
    <div class="card">
        <h3>J+2</h3>
        <div style="font-size:1.5rem"><span id="f2max" style="color:var(--danger)">--</span>° / <span id="f2min" style="color:var(--accent)">--</span>°</div>
    </div>
    <div class="card">
        <h3>J+3</h3>
        <div style="font-size:1.5rem"><span id="f3max" style="color:var(--danger)">--</span>° / <span id="f3min" style="color:var(--accent)">--</span>°</div>
    </div>
  </div>

    <div class="footer">
    Système v)rawliteral" PROJECT_VERSION R"rawliteral( • ESP32-S3 • <a href="/api/data" target="_blank" style="color:#666">Voir JSON Brut</a>
  </div>

<script>
  function getAQIColor(aqi) {
    if(aqi == 1) return ['#1dd1a1', 'Bon'];
    if(aqi == 2) return ['#feca57', 'Moyen'];
    if(aqi == 3) return ['#ff9f43', 'Dégradé'];
    if(aqi == 4) return ['#ff6b6b', 'Mauvais'];
    return ['#5f27cd', 'Dangereux'];
  }

  function updateData() {
    fetch('/api/data')
      .then(response => response.json())
      .then(data => {
        // Sensor
        document.getElementById('inTemp').innerText = data.sensor.temp.toFixed(1);
        document.getElementById('inHum').innerText = data.sensor.hum.toFixed(0);
        document.getElementById('inPres').innerText = data.sensor.pres.toFixed(0);
        document.getElementById('inLux').innerText = data.sensor.lux;
        document.getElementById('trend').innerText = data.sensor.trend;

        // Weather
        document.getElementById('outTemp').innerText = data.weather.temp.toFixed(1);
        document.getElementById('weatherDesc').innerText = "Code WMO: " + data.weather.code;
        document.getElementById('provider').innerText = data.weather.provider;

        // AQI
        const aqi = data.weather.aqi;
        document.getElementById('aqiVal').innerText = aqi;
        if(aqi > 0) {
            const info = getAQIColor(aqi);
            const el = document.getElementById('aqiText');
            el.innerText = info[1];
            el.style.background = info[0];
        }

        // GPS
        document.getElementById('lat').innerText = data.gps.lat.toFixed(4);
        document.getElementById('lon').innerText = data.gps.lon.toFixed(4);
        document.getElementById('sats').innerText = data.gps.sats;

        // Forecast
        const f = data.forecast; // Assuming array provided in JSON
        if(f) {
            document.getElementById('f1max').innerText = f[0].max.toFixed(0); document.getElementById('f1min').innerText = f[0].min.toFixed(0);
            document.getElementById('f2max').innerText = f[1].max.toFixed(0); document.getElementById('f2min').innerText = f[1].min.toFixed(0);
            document.getElementById('f3max').innerText = f[2].max.toFixed(0); document.getElementById('f3min').innerText = f[2].min.toFixed(0);
        }

        // System
        document.getElementById('uptime').innerText = data.sys.uptime;

        // Alert
        const alertBox = document.getElementById('alertPanel');
        if(data.alert) {
            alertBox.style.display = 'block';
            document.getElementById('alertMsg').innerText = "Température critique détectée !";
        } else {
            alertBox.style.display = 'none';
        }
      })
      .catch(err => console.error(err));
  }

  setInterval(updateData, 2000);
  updateData();
</script>
</body>
</html>
)rawliteral";

#endif