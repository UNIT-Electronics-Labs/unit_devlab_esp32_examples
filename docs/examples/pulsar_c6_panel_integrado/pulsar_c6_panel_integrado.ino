/*
  Pulsar C6 - Panel web integrado
  Funciones:
  - Punto de acceso Wi-Fi y servidor web local
  - Entrada touch con interrupción
  - Indicador web gris/azul y contador de pulsos
  - Gestión de archivos en microSD: listar, leer, crear/sobrescribir,
    agregar texto y borrar
  - Control de 26 NeoPixels: color y brillo
  - Lectura del BMI270 por I2C
  - LED de estado por PWM

  Requiere:
  - Arduino-ESP32 Core 3.x
  - SparkFun BMI270 Arduino Library
  - Adafruit NeoPixel

  IMPORTANTE:
  D5 se reserva para el sensor touch. Por lo tanto, el CS de la microSD
  NO puede permanecer en D5. En este ejemplo se mueve a D10.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <Adafruit_NeoPixel.h>
#include <SparkFun_BMI270_Arduino_Library.h>

// ========================= Wi-Fi =========================
const char *AP_SSID = "Pulsar-C6-Lab";
const char *AP_PASSWORD = "pulsarc6";  // mínimo 8 caracteres
WebServer server(80);

// ========================= Pines =========================
// microSD por SPI
constexpr uint8_t SD_SCK  = D13;
constexpr uint8_t SD_MISO = D12;
constexpr uint8_t SD_MOSI = D11;
constexpr uint8_t SD_CS   = D5;  // D5 estaba ocupado por el touch

// Touch y LED de estado
constexpr uint8_t TOUCH_PIN  = D6;
constexpr uint8_t STATUS_LED = D4;

// NeoPixels
constexpr uint8_t NEOPIXEL_PIN = D2;
constexpr uint16_t NUM_PIXELS = 26;

// BMI270
constexpr uint8_t I2C_SDA = A4;
constexpr uint8_t I2C_SCL = A5;
constexpr uint8_t BMI_CS_PIN  = D0;
constexpr uint8_t BMI_SDO_PIN = D1;
constexpr uint8_t BMI_ADDRESS = 0x69;

// PWM del LED de estado
constexpr uint32_t STATUS_PWM_FREQUENCY = 5000;
constexpr uint8_t STATUS_PWM_RESOLUTION = 8;

// ========================= Objetos =========================
Adafruit_NeoPixel pixels(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
BMI270 imu;

// ========================= Estados =========================
volatile bool touchInterruptFlag = false;
volatile uint32_t touchPulseCountISR = 0;

bool touchActive = false;
bool sdReady = false;
bool imuReady = false;

float accelX = 0.0f;
float accelY = 0.0f;
float accelZ = 0.0f;
float gyroX = 0.0f;
float gyroY = 0.0f;
float gyroZ = 0.0f;

uint8_t neoBrightness = 60;
uint8_t statusBrightness = 25;
uint32_t currentNeoColor = 0x0066FF;

unsigned long lastImuRead = 0;
unsigned long lastStatusPulse = 0;
unsigned long lastTouchEvent = 0;
unsigned long lastClientActivity = 0;

constexpr unsigned long TOUCH_HOLD_MS = 220;
constexpr unsigned long IMU_INTERVAL_MS = 100;
constexpr unsigned long CLIENT_ACTIVE_MS = 2500;

// ========================= HTML =========================
const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="es">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Pulsar C6 Lab</title>
<style>
:root{
  --bg:#08111f;
  --card:#101c2f;
  --card2:#14243b;
  --text:#edf6ff;
  --muted:#91a6bd;
  --accent:#1689ff;
  --ok:#31d18b;
  --warn:#ffba49;
  --danger:#ff5d73;
  --border:#263a54;
}
*{box-sizing:border-box}
body{
  margin:0;
  min-height:100vh;
  color:var(--text);
  background:
    radial-gradient(circle at 10% 10%,#12335d 0,transparent 32%),
    radial-gradient(circle at 90% 15%,#123f45 0,transparent 28%),
    var(--bg);
  font-family:Inter,system-ui,-apple-system,Segoe UI,sans-serif;
}
header{
  padding:26px 20px 12px;
  max-width:1200px;
  margin:auto;
}
h1{margin:0;font-size:clamp(1.8rem,4vw,3rem)}
.subtitle{color:var(--muted);margin-top:6px}
.badges{display:flex;gap:8px;flex-wrap:wrap;margin-top:14px}
.badge{
  padding:7px 11px;border:1px solid var(--border);
  border-radius:999px;background:#0b1728;color:var(--muted);
}
main{
  max-width:1200px;margin:auto;padding:10px 20px 35px;
  display:grid;grid-template-columns:repeat(12,1fr);gap:16px;
}
.card{
  background:linear-gradient(145deg,var(--card),var(--card2));
  border:1px solid var(--border);border-radius:18px;padding:18px;
  box-shadow:0 18px 50px #0005;
}
.card h2{font-size:1.05rem;margin:0 0 14px}
.touch-card{grid-column:span 4}
.imu-card{grid-column:span 8}
.neo-card{grid-column:span 6}
.status-card{grid-column:span 6}
.sd-card{grid-column:1/-1}
.touch-wrap{display:flex;align-items:center;gap:22px}
.touch-dot{
  width:92px;height:92px;border-radius:50%;
  background:#596474;border:7px solid #8190a3;
  box-shadow:0 0 0 8px #ffffff0b,inset 0 0 20px #0008;
  transition:.15s ease;
}
.touch-dot.active{
  background:var(--accent);border-color:#75bdff;
  box-shadow:0 0 35px #1689ffcc,0 0 0 8px #1689ff22;
}
.big-number{font-size:2rem;font-weight:750}
.label{font-size:.8rem;color:var(--muted)}
.imu-grid{
  display:grid;grid-template-columns:repeat(6,1fr);gap:10px;
}
.metric{padding:12px;background:#07122199;border:1px solid var(--border);border-radius:12px}
.metric b{display:block;font-size:1.2rem;margin-top:5px}
.controls{display:grid;gap:14px}
.row{display:flex;align-items:center;gap:12px;flex-wrap:wrap}
input[type=color]{width:70px;height:45px;border:0;border-radius:10px;background:none}
input[type=range]{width:min(100%,320px);accent-color:var(--accent)}
input[type=text],textarea,select{
  width:100%;border:1px solid var(--border);background:#071221;
  color:var(--text);border-radius:10px;padding:11px;font:inherit;
}
textarea{min-height:140px;resize:vertical}
button{
  border:0;border-radius:10px;padding:10px 14px;font-weight:700;
  cursor:pointer;background:var(--accent);color:white;
}
button.secondary{background:#35465d}
button.success{background:#168f62}
button.danger{background:#b83d51}
button:active{transform:translateY(1px)}
.sd-layout{display:grid;grid-template-columns:300px 1fr;gap:14px}
.file-list{
  min-height:250px;max-height:430px;overflow:auto;padding:8px;
  background:#071221;border:1px solid var(--border);border-radius:12px;
}
.file{
  width:100%;text-align:left;margin-bottom:7px;background:#172a43;
  display:flex;justify-content:space-between;gap:10px;
}
.file small{color:#bed0e2}
.terminal{
  margin-top:14px;background:#020812;color:#9df5c8;
  border:1px solid #294b40;border-radius:12px;padding:12px;
  min-height:120px;max-height:230px;overflow:auto;
  white-space:pre-wrap;font-family:ui-monospace,SFMono-Regular,Consolas,monospace;
}
.status-line{display:flex;align-items:center;gap:9px;color:var(--muted)}
.status-light{width:10px;height:10px;border-radius:50%;background:var(--danger)}
.status-light.ok{background:var(--ok);box-shadow:0 0 10px var(--ok)}
@media(max-width:850px){
  .touch-card,.imu-card,.neo-card,.status-card{grid-column:1/-1}
  .imu-grid{grid-template-columns:repeat(3,1fr)}
  .sd-layout{grid-template-columns:1fr}
}
@media(max-width:500px){.imu-grid{grid-template-columns:repeat(2,1fr)}}
</style>
</head>
<body>
<header>
  <h1>Pulsar C6 · Panel de integración</h1>
  <div class="subtitle">Touch, BMI270, microSD, NeoPixels y PWM desde una sola interfaz local.</div>
  <div class="badges">
    <span class="badge">IP: <b>192.168.4.1</b></span>
    <span class="badge">Red: <b>Pulsar-C6-Lab</b></span>
    <span class="badge">Tiempo activo: <b id="uptime">0 s</b></span>
  </div>
</header>

<main>
<section class="card touch-card">
  <h2>Entrada touch</h2>
  <div class="touch-wrap">
    <div id="touchDot" class="touch-dot"></div>
    <div>
      <div class="label">Pulsos detectados</div>
      <div id="touchCount" class="big-number">0</div>
      <div id="touchText" class="label">Sin contacto</div>
    </div>
  </div>
</section>

<section class="card imu-card">
  <h2>BMI270 · Acelerómetro y giroscopio</h2>
  <div class="status-line">
    <span id="imuLight" class="status-light"></span>
    <span id="imuStatus">Comprobando sensor…</span>
  </div>
  <div class="imu-grid" style="margin-top:14px">
    <div class="metric"><span class="label">Accel X · g</span><b id="ax">0.000</b></div>
    <div class="metric"><span class="label">Accel Y · g</span><b id="ay">0.000</b></div>
    <div class="metric"><span class="label">Accel Z · g</span><b id="az">0.000</b></div>
    <div class="metric"><span class="label">Gyro X · °/s</span><b id="gx">0.000</b></div>
    <div class="metric"><span class="label">Gyro Y · °/s</span><b id="gy">0.000</b></div>
    <div class="metric"><span class="label">Gyro Z · °/s</span><b id="gz">0.000</b></div>
  </div>
</section>

<section class="card neo-card">
  <h2>Matriz + NeoPixel integrado · 26 LED</h2>
  <div class="controls">
    <div class="row">
      <input id="neoColor" type="color" value="#0066ff">
      <button onclick="applyNeo()">Aplicar color</button>
      <button class="secondary" onclick="neoOff()">Apagar</button>
    </div>
    <label>
      <span class="label">Brillo: <b id="neoBrightnessValue">60</b>/255</span><br>
      <input id="neoBrightness" type="range" min="0" max="255" value="60"
             oninput="neoBrightnessValue.textContent=this.value"
             onchange="applyNeo()">
    </label>
    <div class="row">
      <button style="background:#ff3131" onclick="quickColor('#ff0000')">Rojo</button>
      <button style="background:#21a35b" onclick="quickColor('#00ff55')">Verde</button>
      <button style="background:#166cff" onclick="quickColor('#0066ff')">Azul</button>
      <button style="background:#f5f5f5;color:#222" onclick="quickColor('#ffffff')">Blanco</button>
      <button style="background:#9c4dff" onclick="quickColor('#8d35ff')">Violeta</button>
    </div>
  </div>
</section>

<section class="card status-card">
  <h2>LED de estado · D4</h2>
  <p class="label">
    El LED respira suavemente cuando el sistema está listo, da un destello al detectar
    touch y aumenta su actividad mientras hay una página conectada.
  </p>
  <label>
    <span class="label">Límite de intensidad PWM: <b id="statusBrightnessValue">25</b>/255</span><br>
    <input id="statusBrightness" type="range" min="0" max="255" value="25"
           oninput="statusBrightnessValue.textContent=this.value"
           onchange="setStatusBrightness()">
  </label>
</section>

<section class="card sd-card">
  <h2>Administrador de microSD</h2>
  <div class="status-line">
    <span id="sdLight" class="status-light"></span>
    <span id="sdStatus">Comprobando tarjeta…</span>
  </div>

  <div class="sd-layout" style="margin-top:14px">
    <div>
      <div class="row" style="margin-bottom:8px">
        <button onclick="listFiles()">Leer SD</button>
        <button class="secondary" onclick="clearTerminal()">Limpiar terminal</button>
      </div>
      <div id="fileList" class="file-list">Sin información.</div>
    </div>

    <div>
      <label class="label">Archivo</label>
      <input id="fileName" type="text" value="/registro.txt" placeholder="/archivo.txt">

      <label class="label" style="display:block;margin-top:10px">Contenido</label>
      <textarea id="fileContent" placeholder="Escribe aquí el contenido que deseas guardar…"></textarea>

      <div class="row" style="margin-top:10px">
        <button class="success" onclick="writeFile(false)">Crear / sobrescribir</button>
        <button onclick="writeFile(true)">Agregar texto</button>
        <button class="secondary" onclick="readSelected()">Leer archivo</button>
        <button class="danger" onclick="deleteSelected()">Borrar archivo</button>
      </div>
    </div>
  </div>

  <div id="terminal" class="terminal">[Pulsar C6] Terminal lista.</div>
</section>
</main>

<script>
const $ = id => document.getElementById(id);

function log(message){
  const now = new Date().toLocaleTimeString();
  $('terminal').textContent += `\n[${now}] ${message}`;
  $('terminal').scrollTop = $('terminal').scrollHeight;
}
function clearTerminal(){ $('terminal').textContent='[Pulsar C6] Terminal limpia.'; }

async function api(url, options={}){
  const response = await fetch(url, options);
  const text = await response.text();
  if(!response.ok) throw new Error(text || `HTTP ${response.status}`);
  return text;
}

async function updateState(){
  try{
    const data = await (await fetch('/api/state',{cache:'no-store'})).json();

    $('touchDot').classList.toggle('active',data.touch);
    $('touchCount').textContent=data.touchCount;
    $('touchText').textContent=data.touch?'¡Touch detectado!':'Sin contacto';

    $('ax').textContent=data.ax.toFixed(3);
    $('ay').textContent=data.ay.toFixed(3);
    $('az').textContent=data.az.toFixed(3);
    $('gx').textContent=data.gx.toFixed(3);
    $('gy').textContent=data.gy.toFixed(3);
    $('gz').textContent=data.gz.toFixed(3);

    $('imuStatus').textContent=data.imu?'BMI270 conectado en 0x68':'BMI270 no disponible';
    $('imuLight').classList.toggle('ok',data.imu);

    $('sdStatus').textContent=data.sd
      ? `SD lista · ${data.sdUsedMB.toFixed(2)} MB usados de ${data.sdTotalMB.toFixed(2)} MB`
      : 'MicroSD no disponible';
    $('sdLight').classList.toggle('ok',data.sd);

    $('uptime').textContent=Math.floor(data.uptime/1000)+' s';
  }catch(error){
    $('imuStatus').textContent='Sin comunicación con la Pulsar C6';
  }
}

async function applyNeo(){
  const color=$('neoColor').value.substring(1);
  const brightness=$('neoBrightness').value;
  try{
    await api(`/api/neopixel?color=${color}&brightness=${brightness}`);
    log(`NeoPixels: #${color}, brillo ${brightness}.`);
  }catch(e){log('Error NeoPixel: '+e.message);}
}
function quickColor(color){$('neoColor').value=color;applyNeo();}
function neoOff(){$('neoBrightness').value=0;$('neoBrightnessValue').textContent=0;applyNeo();}

async function setStatusBrightness(){
  const value=$('statusBrightness').value;
  try{
    await api(`/api/status-led?brightness=${value}`);
    log(`Límite PWM del LED de estado: ${value}.`);
  }catch(e){log('Error PWM: '+e.message);}
}

async function listFiles(){
  try{
    const data=await (await fetch('/api/files',{cache:'no-store'})).json();
    const box=$('fileList');
    box.innerHTML='';
    if(!data.files.length){
      box.textContent='La tarjeta está vacía.';
    }else{
      data.files.forEach(file=>{
        const button=document.createElement('button');
        button.className='file';
        button.innerHTML=`<span>${file.name}</span><small>${file.size} B</small>`;
        button.onclick=()=>{
          $('fileName').value=file.name;
          readSelected();
        };
        box.appendChild(button);
      });
    }
    log(`Directorio leído: ${data.files.length} archivo(s).`);
  }catch(e){log('Error al listar: '+e.message);}
}

async function readSelected(){
  const name=$('fileName').value.trim();
  if(!name)return log('Indica un nombre de archivo.');
  try{
    const content=await api('/api/file?name='+encodeURIComponent(name));
    $('fileContent').value=content;
    log(`Archivo leído: ${name} (${content.length} caracteres).`);
  }catch(e){log('Error al leer: '+e.message);}
}

async function writeFile(append){
  const name=$('fileName').value.trim();
  const content=$('fileContent').value;
  if(!name)return log('Indica un nombre de archivo.');
  try{
    await api(`/api/file?name=${encodeURIComponent(name)}&append=${append?1:0}`,{
      method:'POST',
      headers:{'Content-Type':'text/plain;charset=UTF-8'},
      body:content
    });
    log(`${append?'Texto agregado a':'Archivo guardado en'} ${name}.`);
    listFiles();
  }catch(e){log('Error al guardar: '+e.message);}
}

async function deleteSelected(){
  const name=$('fileName').value.trim();
  if(!name)return log('Indica un nombre de archivo.');
  if(!confirm(`¿Borrar ${name}?`))return;
  try{
    await api('/api/file?name='+encodeURIComponent(name),{method:'DELETE'});
    $('fileContent').value='';
    log(`Archivo borrado: ${name}.`);
    listFiles();
  }catch(e){log('Error al borrar: '+e.message);}
}

setInterval(updateState,400);
updateState();
listFiles();
</script>
</body>
</html>
)HTML";

// ========================= Utilidades =========================
void IRAM_ATTR onTouchRise() {
  touchPulseCountISR++;
  touchInterruptFlag = true;
}

String jsonEscape(const String &value) {
  String output;
  output.reserve(value.length() + 8);

  for (size_t i = 0; i < value.length(); i++) {
    const char c = value[i];
    switch (c) {
      case '\\': output += "\\\\"; break;
      case '"':  output += "\\\""; break;
      case '\n': output += "\\n";  break;
      case '\r': output += "\\r";  break;
      case '\t': output += "\\t";  break;
      default:
        if (static_cast<uint8_t>(c) >= 0x20) output += c;
        break;
    }
  }
  return output;
}

String normalizePath(String path) {
  path.trim();

  if (path.length() == 0) return "";
  if (!path.startsWith("/")) path = "/" + path;

  // Evita rutas relativas o caracteres problemáticos.
  if (path.indexOf("..") >= 0 ||
      path.indexOf('\\') >= 0 ||
      path.indexOf(':') >= 0 ||
      path.length() > 63) {
    return "";
  }

  return path;
}

uint32_t parseHexColor(String colorText) {
  colorText.replace("#", "");
  if (colorText.length() != 6) return currentNeoColor;
  return strtoul(colorText.c_str(), nullptr, 16);
}

void applyNeoPixels() {
  pixels.setBrightness(neoBrightness);

  const uint8_t red   = (currentNeoColor >> 16) & 0xFF;
  const uint8_t green = (currentNeoColor >> 8) & 0xFF;
  const uint8_t blue  = currentNeoColor & 0xFF;

  pixels.fill(pixels.Color(red, green, blue), 0, NUM_PIXELS);
  pixels.show();
}

void setStatusLedDuty(uint8_t duty) {
  ledcWrite(STATUS_LED, duty);
}

void updateStatusLed() {
  // Prioridad 1: destello al detectar touch.
  if (millis() - lastTouchEvent < 160) {
    setStatusLedDuty(statusBrightness);
    return;
  }

  // Prioridad 2: parpadeo corto si la SD o el BMI270 fallaron.
  if (!sdReady || !imuReady) {
    const bool on = (millis() / 350) % 2;
    setStatusLedDuty(on ? statusBrightness : 0);
    return;
  }

  // Prioridad 3: cuando hay un navegador consultando, respiración más rápida.
  const bool clientActive = (millis() - lastClientActivity) < CLIENT_ACTIVE_MS;
  const unsigned long period = clientActive ? 1200 : 2600;
  const float phase = static_cast<float>(millis() % period) / period;
  const float triangle = phase < 0.5f ? phase * 2.0f : (1.0f - phase) * 2.0f;

  // Mantiene un mínimo visible del 8% cuando el control no está en cero.
  const float minimum = statusBrightness == 0 ? 0.0f : statusBrightness * 0.08f;
  const uint8_t duty = static_cast<uint8_t>(minimum +
                         triangle * (statusBrightness - minimum));
  setStatusLedDuty(duty);
}

void updateTouch() {
  bool eventDetected = false;

  noInterrupts();
  if (touchInterruptFlag) {
    touchInterruptFlag = false;
    eventDetected = true;
  }
  interrupts();

  if (eventDetected) {
    touchActive = true;
    lastTouchEvent = millis();
    Serial.printf("Touch detectado. Total: %lu\n",
                  static_cast<unsigned long>(touchPulseCountISR));
  }

  if (touchActive && millis() - lastTouchEvent >= TOUCH_HOLD_MS) {
    touchActive = false;
  }
}

void updateImu() {
  if (!imuReady || millis() - lastImuRead < IMU_INTERVAL_MS) return;
  lastImuRead = millis();

  if (imu.getSensorData() != BMI2_OK) return;

  accelX = imu.data.accelX;
  accelY = imu.data.accelY;
  accelZ = imu.data.accelZ;
  gyroX = imu.data.gyroX;
  gyroY = imu.data.gyroY;
  gyroZ = imu.data.gyroZ;
}

// ========================= Inicialización =========================
void initSdCard() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  sdReady = SD.begin(SD_CS, SPI);
  if (!sdReady) {
    Serial.println("Error: no fue posible montar la microSD.");
    return;
  }

  const uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    sdReady = false;
    Serial.println("Error: no hay tarjeta microSD.");
    return;
  }

  Serial.printf("microSD lista. Capacidad: %llu MB\n",
                SD.cardSize() / (1024ULL * 1024ULL));
}

void initImu() {
  pinMode(BMI_CS_PIN, OUTPUT);
  pinMode(BMI_SDO_PIN, OUTPUT);

  digitalWrite(BMI_CS_PIN, HIGH);  // Selecciona interfaz I2C
  digitalWrite(BMI_SDO_PIN, LOW);  // LOW = dirección 0x68

  Wire.begin(I2C_SDA, I2C_SCL);

  imuReady = (imu.beginI2C(BMI_ADDRESS, Wire) == BMI2_OK);
  Serial.println(imuReady
                 ? "BMI270 conectado en 0x68."
                 : "Error: BMI270 no detectado en 0x68.");
}

void initWiFi() {
  WiFi.mode(WIFI_AP);

  if (!WiFi.softAP(AP_SSID, AP_PASSWORD)) {
    Serial.println("Error al crear el punto de acceso.");
    return;
  }

  Serial.println();
  Serial.println("Punto de acceso iniciado.");
  Serial.printf("Red: %s\n", AP_SSID);
  Serial.printf("Clave: %s\n", AP_PASSWORD);
  Serial.printf("Panel: http://%s/\n", WiFi.softAPIP().toString().c_str());
}

// ========================= API web =========================
void handleRoot() {
  lastClientActivity = millis();
  server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void handleState() {
  lastClientActivity = millis();

  uint32_t touchCountCopy;
  noInterrupts();
  touchCountCopy = touchPulseCountISR;
  interrupts();

  const double totalMB = sdReady ? SD.totalBytes() / (1024.0 * 1024.0) : 0.0;
  const double usedMB  = sdReady ? SD.usedBytes() / (1024.0 * 1024.0) : 0.0;

  String json;
  json.reserve(320);
  json += "{";
  json += "\"touch\":" + String(touchActive ? "true" : "false");
  json += ",\"touchCount\":" + String(touchCountCopy);
  json += ",\"imu\":" + String(imuReady ? "true" : "false");
  json += ",\"sd\":" + String(sdReady ? "true" : "false");
  json += ",\"ax\":" + String(accelX, 4);
  json += ",\"ay\":" + String(accelY, 4);
  json += ",\"az\":" + String(accelZ, 4);
  json += ",\"gx\":" + String(gyroX, 4);
  json += ",\"gy\":" + String(gyroY, 4);
  json += ",\"gz\":" + String(gyroZ, 4);
  json += ",\"sdTotalMB\":" + String(totalMB, 2);
  json += ",\"sdUsedMB\":" + String(usedMB, 2);
  json += ",\"uptime\":" + String(millis());
  json += "}";

  server.send(200, "application/json", json);
}

void handleListFiles() {
  lastClientActivity = millis();

  if (!sdReady) {
    server.send(503, "text/plain", "La microSD no está disponible.");
    return;
  }

  File root = SD.open("/");
  if (!root || !root.isDirectory()) {
    server.send(500, "text/plain", "No fue posible abrir el directorio raíz.");
    return;
  }

  String json = "{\"files\":[";
  bool first = true;
  File file = root.openNextFile();

  while (file) {
    if (!file.isDirectory()) {
      if (!first) json += ",";
      first = false;

      String name = file.name();
      if (!name.startsWith("/")) name = "/" + name;

      json += "{\"name\":\"";
      json += jsonEscape(name);
      json += "\",\"size\":";
      json += String(file.size());
      json += "}";
    }

    file.close();
    file = root.openNextFile();
  }

  root.close();
  json += "]}";
  server.send(200, "application/json", json);
}

void handleReadFile() {
  lastClientActivity = millis();

  if (!sdReady) {
    server.send(503, "text/plain", "La microSD no está disponible.");
    return;
  }

  const String path = normalizePath(server.arg("name"));
  if (path.length() == 0 || path == "/") {
    server.send(400, "text/plain", "Nombre de archivo no válido.");
    return;
  }

  File file = SD.open(path, FILE_READ);
  if (!file || file.isDirectory()) {
    server.send(404, "text/plain", "Archivo no encontrado.");
    return;
  }

  // Se envía por streaming para no copiar todo el archivo a RAM.
  server.streamFile(file, "text/plain; charset=utf-8");
  file.close();
}

void handleWriteFile() {
  lastClientActivity = millis();

  if (!sdReady) {
    server.send(503, "text/plain", "La microSD no está disponible.");
    return;
  }

  const String path = normalizePath(server.arg("name"));
  if (path.length() == 0 || path == "/") {
    server.send(400, "text/plain", "Nombre de archivo no válido.");
    return;
  }

  const bool append = server.arg("append") == "1";
  const String content = server.arg("plain");

  // Para sobrescribir, primero se elimina el archivo anterior.
  if (!append && SD.exists(path)) {
    if (!SD.remove(path)) {
      server.send(500, "text/plain", "No fue posible reemplazar el archivo.");
      return;
    }
  }

  File file = SD.open(path, append ? FILE_APPEND : FILE_WRITE);
  if (!file) {
    server.send(500, "text/plain", "No fue posible abrir el archivo para escritura.");
    return;
  }

  const size_t written = file.print(content);
  file.close();

  if (written != content.length()) {
    server.send(500, "text/plain", "La escritura quedó incompleta.");
    return;
  }

  Serial.printf("%s: %s (%u bytes)\n",
                append ? "Texto agregado" : "Archivo guardado",
                path.c_str(),
                static_cast<unsigned>(written));

  server.send(200, "text/plain", "OK");
}

void handleDeleteFile() {
  lastClientActivity = millis();

  if (!sdReady) {
    server.send(503, "text/plain", "La microSD no está disponible.");
    return;
  }

  const String path = normalizePath(server.arg("name"));
  if (path.length() == 0 || path == "/") {
    server.send(400, "text/plain", "Nombre de archivo no válido.");
    return;
  }

  if (!SD.exists(path)) {
    server.send(404, "text/plain", "El archivo no existe.");
    return;
  }

  if (!SD.remove(path)) {
    server.send(500, "text/plain", "No fue posible borrar el archivo.");
    return;
  }

  Serial.printf("Archivo borrado: %s\n", path.c_str());
  server.send(200, "text/plain", "OK");
}

void handleNeoPixel() {
  lastClientActivity = millis();

  if (server.hasArg("color")) {
    currentNeoColor = parseHexColor(server.arg("color"));
  }

  if (server.hasArg("brightness")) {
    neoBrightness = constrain(server.arg("brightness").toInt(), 0, 255);
  }

  applyNeoPixels();
  server.send(200, "text/plain", "OK");
}

void handleStatusLed() {
  lastClientActivity = millis();

  if (!server.hasArg("brightness")) {
    server.send(400, "text/plain", "Falta el valor brightness.");
    return;
  }

  statusBrightness = constrain(server.arg("brightness").toInt(), 0, 255);
  server.send(200, "text/plain", "OK");
}

void handleNotFound() {
  server.send(404, "text/plain", "Ruta no encontrada.");
}

void initWebServer() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/files", HTTP_GET, handleListFiles);
  server.on("/api/file", HTTP_GET, handleReadFile);
  server.on("/api/file", HTTP_POST, handleWriteFile);
  server.on("/api/file", HTTP_DELETE, handleDeleteFile);
  server.on("/api/neopixel", HTTP_GET, handleNeoPixel);
  server.on("/api/status-led", HTTP_GET, handleStatusLed);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("Servidor HTTP iniciado.");
}

// ========================= Arduino =========================
void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.println("=== Pulsar C6 - Panel web integrado ===");

  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(TOUCH_PIN), onTouchRise, RISING);

  if (!ledcAttach(STATUS_LED, STATUS_PWM_FREQUENCY, STATUS_PWM_RESOLUTION)) {
    Serial.println("Error al configurar PWM del LED de estado.");
  }
  setStatusLedDuty(0);

  pixels.begin();
  pixels.clear();
  applyNeoPixels();

  initSdCard();
  initImu();
  initWiFi();
  initWebServer();
}

void loop() {
  server.handleClient();
  updateTouch();
  updateImu();
  updateStatusLed();

  delay(2);
}
