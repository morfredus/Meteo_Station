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
    <div style="margin-bottom:8px;">Système <strong>v</strong>: <span id="version">--</span> • ESP32-S3</div>
    <div style="font-size:0.85rem; color:#777;">
      <a href="/api/data" target="_blank" style="color:#aaa">/api/data</a> (JSON capteurs) •
      <a href="/api/debug" target="_blank" style="color:#aaa">/api/debug</a> (Diagnostics)
    </div>
  </div>

  <h2 style="margin-top:30px; font-size:1.1rem; border-bottom:1px solid #333; padding-bottom:10px;">Diagnostics & Tests API</h2>

  <div class="grid">
    <div class="card">
      <h3>Diagnostics Rapides</h3>
      <div style="text-align:left; font-size:0.9rem; line-height:1.4;">
        <strong>Version:</strong> <span id="dbg_version">--</span><br>
        <strong>AHT20:</strong> <span id="dbg_aht">--</span><br>
        <strong>BMP280:</strong> <span id="dbg_bmp">--</span><br>
        <strong>I2C devices:</strong> <span id="dbg_i2c">aucun</span>
      </div>
      <div style="margin-top:10px;"><button id="btnDebug" style="padding:8px 12px; border-radius:6px; background:var(--accent); color:#000; border:none; cursor:pointer;">Rafraîchir diagnostics</button></div>
    </div>

    <div class="card">
      <h3>Tests & Endpoints</h3>
      <div style="text-align:left; font-size:0.85rem; color:#bbb;">
        <div><strong>GET</strong> <a href="/api/data" target="_blank">/api/data</a> — JSON capteurs & météo</div>
        <div style="margin-top:6px;"><strong>GET</strong> <a href="/api/debug" target="_blank">/api/debug</a> — état capteurs, I2C</div>
        <div style="margin-top:6px;"><strong>POST</strong> /api/scan — scan I2C manuel (voir bouton ci-contre)</div>
        <div style="margin-top:8px;">Exemples (terminal):</div>
        <pre style="background:#111; color:#9f9; padding:8px; border-radius:6px; font-size:0.85rem;">curl http://DEVICE_IP/api/data
curl http://DEVICE_IP/api/debug
curl -X POST http://DEVICE_IP/api/scan</pre>
      </div>
    </div>

    <div class="card">
      <h3>Tester I2C (local)</h3>
      <div style="font-size:0.85rem; text-align:left; color:#bbb; line-height:1.4;">
        Vous pouvez afficher la liste des devices I2C détectés et leurs adresses hex.
        <div style="margin-top:8px;"><button id="btnShowI2C" style="padding:8px 12px; border-radius:6px; background:#444; color:#fff; border:none; cursor:pointer;">Afficher I2C</button></div>
        <pre id="i2cList" style="background:#0f0f0f; color:#ddd; padding:8px; border-radius:6px; margin-top:8px;">--</pre>
      </div>
    </div>
  </div>

</div>

<script>
  function getAQIColor(aqi) {
    if(aqi == 1) return ['#1dd1a1', 'Bon'];
    if(aqi == 2) return ['#feca57', 'Moyen'];
    if(aqi == 3) return ['#ff9f43', 'Dégradé'];
    if(aqi == 4) return ['#ff6b6b', 'Mauvais'];
    return ['#5f27cd', 'Dangereux'];
  }

  function safeFixed(v, digits) {
    if (v === null || v === undefined || isNaN(v)) return '--';
    return Number(v).toFixed(digits);
  }

  function updateData() {
    fetch('/api/data')
      .then(response => response.json())
      .then(data => {
        document.getElementById('inTemp').innerText = safeFixed(data.sensor.temp,1);
        document.getElementById('inHum').innerText = safeFixed(data.sensor.hum,0);
        document.getElementById('inPres').innerText = safeFixed(data.sensor.pres,0);
        document.getElementById('inLux').innerText = data.sensor.lux || '--';
        document.getElementById('trend').innerText = data.sensor.trend || '--';
        document.getElementById('outTemp').innerText = safeFixed(data.weather.temp,1);
        document.getElementById('weatherDesc').innerText = 'Code WMO: ' + (data.weather.code || '--');
        document.getElementById('provider').innerText = data.weather.provider || '--';
        const aqi = data.weather.aqi || 0;
        document.getElementById('aqiVal').innerText = aqi;
        if(aqi > 0) {
            const info = getAQIColor(aqi);
            const el = document.getElementById('aqiText');
            el.innerText = info[1];
            el.style.background = info[0];
        }
        document.getElementById('lat').innerText = safeFixed(data.gps.lat,4);
        document.getElementById('lon').innerText = safeFixed(data.gps.lon,4);
        document.getElementById('sats').innerText = data.gps.sats || '--';
        const f = data.forecast;
        if(f && f.length >= 3) {
            document.getElementById('f1max').innerText = safeFixed(f[0].max,0); document.getElementById('f1min').innerText = safeFixed(f[0].min,0);
            document.getElementById('f2max').innerText = safeFixed(f[1].max,0); document.getElementById('f2min').innerText = safeFixed(f[1].min,0);
            document.getElementById('f3max').innerText = safeFixed(f[2].max,0); document.getElementById('f3min').innerText = safeFixed(f[2].min,0);
        }
        document.getElementById('uptime').innerText = data.sys ? (data.sys.uptime || '--') : '--';
        
        // Gestion des alertes
        const alertPanel = document.getElementById('alertPanel');
        const alertMsg = document.getElementById('alertMsg');
        if (data.alert && data.alert.active) {
          alertPanel.style.display = 'block';
          alertMsg.innerText = data.alert.message || 'Alerte active';
        } else {
          alertPanel.style.display = 'none';
        }
      })
      .catch(err => console.error('updateData error', err));
  }

  function updateDebug() {
    fetch('/api/debug')
      .then(r => { if (!r.ok) throw new Error('debug endpoint non disponible'); return r.json(); })
      .then(d => {
        document.getElementById('dbg_version').innerText = d.version || '--';
        document.getElementById('version').innerText = d.version || '--';
        document.getElementById('dbg_aht').innerText = d.ahtAvailable ? 'OK' : 'ABSENT';
        document.getElementById('dbg_bmp').innerText = d.bmpAvailable ? 'OK' : 'ABSENT';
        if (d.i2c_count && Array.isArray(d.i2c_devices)) {
          const hexes = d.i2c_devices.map(a => '0x' + (Number(a).toString(16).toUpperCase().padStart(2,'0'))).join(', ');
          document.getElementById('dbg_i2c').innerText = hexes || 'aucun';
          document.getElementById('i2cList').innerText = hexes || 'aucun';
        } else {
          document.getElementById('dbg_i2c').innerText = 'aucun';
          document.getElementById('i2cList').innerText = 'aucun';
        }
      })
      .catch(err => {
        document.getElementById('dbg_version').innerText = '--';
        document.getElementById('dbg_aht').innerText = 'N/A';
        document.getElementById('dbg_bmp').innerText = 'N/A';
        document.getElementById('dbg_i2c').innerText = 'N/A';
        document.getElementById('i2cList').innerText = 'Endpoint /api/debug non disponible';
        console.warn('updateDebug error', err);
      });
  }

  document.addEventListener('DOMContentLoaded', function(){
    const btnDebug = document.getElementById('btnDebug');
    if(btnDebug) btnDebug.addEventListener('click', updateDebug);
    const btnShowI2C = document.getElementById('btnShowI2C');
    if(btnShowI2C) btnShowI2C.addEventListener('click', updateDebug);
    const tryScan = document.createElement('button');
    tryScan.textContent = 'Lancer I2C scan (si supporté)';
    tryScan.style = 'padding:8px 12px; margin-left:8px; border-radius:6px; background:#222; color:#fff; border:none; cursor:pointer;';
    tryScan.onclick = function(){
      fetch('/api/scan', { method: 'POST' })
        .then(r => r.json())
        .then(j => { document.getElementById('i2cList').innerText = JSON.stringify(j); updateDebug(); })
        .catch(e => { document.getElementById('i2cList').innerText = 'Endpoint /api/scan non disponible'; console.warn(e); });
    };
    if (btnShowI2C && btnShowI2C.parentNode) btnShowI2C.parentNode.appendChild(tryScan);
    updateData(); updateDebug();
    setInterval(updateData, 2000);
    setInterval(updateDebug, 10000);
  });
</script>
</body>
</html>
)rawliteral";

#endif
