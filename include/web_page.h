#ifndef WEB_PAGE_H
#define WEB_PAGE_H

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Station Météo ESP32-S3</title>
  <style>
    body { font-family: 'Segoe UI', sans-serif; background: #1a1a1a; color: #fff; text-align: center; margin:0; padding:20px;}
    .card { background: #2d2d2d; border-radius: 15px; padding: 20px; margin: 10px auto; max-width: 400px; box-shadow: 0 4px 15px rgba(0,0,0,0.5); }
    h1 { color: #00a8ff; font-size: 1.5rem;}
    h2 { border-bottom: 1px solid #444; padding-bottom: 10px; font-size: 1.2rem; color: #aaa;}
    .value { font-size: 2.5rem; font-weight: bold; }
    .unit { font-size: 1rem; color: #888; }
    .row { display: flex; justify-content: space-around; }
    .status { font-size: 0.9rem; color: #0f0; }
    .alert { color: #f00; font-weight: bold; animation: blink 1s infinite; }
    @keyframes blink { 50% { opacity: 0.5; } }
  </style>
</head>
<body>
  <h1>Météo Station S3</h1>
  
  <div class="card">
    <h2>Intérieur (Capteurs)</h2>
    <div class="row">
        <div><div class="value" id="inTemp">--</div><div class="unit">°C</div></div>
        <div><div class="value" id="inHum">--</div><div class="unit">%</div></div>
    </div>
    <div style="margin-top:15px">Pression: <span id="inPres">--</span> hPa</div>
  </div>

  <div class="card">
    <h2>Extérieur (API/GPS)</h2>
    <div class="value" id="outTemp">--</div><div class="unit">°C (Ressenti)</div>
    <div id="weatherDesc" style="margin-top:10px; color:#fdcb6e">Chargement...</div>
    <div style="margin-top:10px; font-size:0.8rem">Loc: <span id="loc">--</span></div>
  </div>

  <div class="card">
    <h2>Système</h2>
    <div>GPS: <span id="gpsStatus">Recherche...</span></div>
    <div>Lumière: <span id="lux">--</span></div>
    <div id="alertBox" style="display:none" class="alert">ALERTE MÉTÉO !</div>
  </div>

<script>
  function fetchData() {
    fetch('/api/data')
      .then(response => response.json())
      .then(data => {
        document.getElementById('inTemp').innerText = data.sensor.temp.toFixed(1);
        document.getElementById('inHum').innerText = data.sensor.hum.toFixed(0);
        document.getElementById('inPres').innerText = data.sensor.pres.toFixed(0);
        document.getElementById('lux').innerText = data.sensor.lux;
        
        document.getElementById('outTemp').innerText = data.weather.temp.toFixed(1);
        document.getElementById('weatherDesc').innerText = "Code WMO: " + data.weather.code;
        document.getElementById('loc').innerText = data.gps.lat + ", " + data.gps.lon;
        
        document.getElementById('gpsStatus').innerText = data.gps.valid ? "Fix OK ("+data.gps.sats+" sats)" : "Pas de Fix";
        document.getElementById('gpsStatus').style.color = data.gps.valid ? "#0f0" : "#f00";

        if(data.alert) {
            document.getElementById('alertBox').style.display = 'block';
        } else {
            document.getElementById('alertBox').style.display = 'none';
        }
      });
  }
  setInterval(fetchData, 2000);
  fetchData();
</script>
</body>
</html>
)rawliteral";

#endif