#include <WiFi.h>
#include <WebServer.h>

// --- НАСТРОЙКИ WI-FI ---
const char* ssid     = "YOUR_WIFI_SSID";     // Имя твоей домашней сети
const char* password = "YOUR_WIFI_PASSWORD"; // Пароль от Wi-Fi

// --- НАСТРОЙКИ ПИНОВ И ШИМ ---
const int coldPin = 13; // Первый мосфет (GPIO13)
const int warmPin = 12; // Второй мосфет (GPIO12 — раскомментируй завтра)

const int freq       = 5000; 
const int resolution = 8;    

// Состояние лампы
bool powerState       = true;  // Включена/Выключена
int currentBrightness = 128;   // Текущая яркость (0-255)

WebServer server(80);

// Применение настроек на железке
void applyLight() {
  int targetValue = powerState ? currentBrightness : 0;
  
  ledcWrite(coldPin, targetValue);
  // ledcWrite(warmPin, targetValue); // Завтра раскомментируешь для второго канала
}

// HTML + JS Веб-интерфейс
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Умная Лампа</title>";
  html += "<style>";
  html += "body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: #121212; color: #fff; text-align: center; padding: 40px 15px; margin: 0; }";
  html += ".card { background: #1e1e1e; max-width: 360px; margin: 0 auto; padding: 30px 20px; border-radius: 20px; box-shadow: 0 10px 30px rgba(0,0,0,0.5); }";
  html += "h1 { margin-top: 0; margin-bottom: 25px; font-size: 24px; color: #00d2ff; }";
  
  // Кнопка Вкл/Выкл
  html += ".btn { width: 100%; padding: 15px; border: none; border-radius: 12px; font-size: 18px; font-weight: bold; cursor: pointer; transition: 0.2s; margin-bottom: 25px; }";
  html += ".btn-on { background: #00d2ff; color: #121212; }";
  html += ".btn-off { background: #333; color: #888; }";
  
  // Слайдер
  html += ".slider-container { margin-top: 10px; }";
  html += ".slider { width: 100%; -webkit-appearance: none; height: 10px; border-radius: 5px; background: #333; outline: none; margin: 15px 0; }";
  html += ".slider::-webkit-slider-thumb { -webkit-appearance: none; width: 26px; height: 26px; border-radius: 50%; background: #00d2ff; cursor: pointer; }";
  html += ".val-text { font-size: 18px; color: #ccc; }";
  html += "</style></head><body>";
  
  html += "<div class='card'>";
  html += "<h1>❄️ Умная Лампа</h1>";
  
  // Кнопка
  String btnClass = powerState ? "btn btn-on" : "btn btn-off";
  String btnText  = powerState ? "ВКЛЮЧЕНО" : "ВЫКЛЮЧЕНО";
  html += "<button id='powerBtn' class='" + btnClass + "' onclick='togglePower()'>" + btnText + "</button>";
  
  // Ползунок
  html += "<div class='slider-container'>";
  html += "<input type='range' min='1' max='255' value='" + String(currentBrightness) + "' class='slider' id='brightnessSlider' oninput='updateUI(this.value)' onchange='sendBrightness(this.value)'>";
  html += "<p class='val-text'>Яркость: <span id='val'>" + String(map(currentBrightness, 0, 255, 0, 100)) + "</span>%</p>";
  html += "</div>";
  
  html += "</div>";
  
  // JS скрипт для динамического обновления без перезагрузки
  html += "<script>";
  html += "let pState = " + String(powerState ? "true" : "false") + ";";
  html += "function togglePower() {";
  html += "  pState = !pState;";
  html += "  fetch('/api/power?state=' + (pState ? 1 : 0)).then(() => location.reload());";
  html += "}";
  html += "function updateUI(val) {";
  html += "  document.getElementById('val').innerText = Math.round((val/255)*100);";
  html += "}";
  html += "function sendBrightness(val) {";
  html += "  fetch('/api/brightness?value=' + val);";
  html += "}";
  html += "</script></body></html>";
  
  return html;
}

// --- ХЕНДЛЕРЫ ВЕБ-СЕРВЕРА ---
void handleRoot() {
  server.send(200, "text/html; charset=utf-8", getHTML());
}

// API: Питание (/api/power?state=1 или /api/power?state=0 или /api/power?toggle=1)
void handleApiPower() {
  if (server.hasArg("state")) {
    powerState = server.arg("state").toInt() > 0;
  } else if (server.hasArg("toggle")) {
    powerState = !powerState;
  }
  applyLight();
  server.send(200, "application/json", "{\"status\":\"ok\",\"power\":" + String(powerState ? "true" : "false") + "}");
}

// API: Яркость (/api/brightness?value=0..255)
void handleApiBrightness() {
  if (server.hasArg("value")) {
    currentBrightness = constrain(server.arg("value").toInt(), 0, 255);
    if (currentBrightness > 0) powerState = true;
    applyLight();
    server.send(200, "application/json", "{\"status\":\"ok\",\"brightness\":" + String(currentBrightness) + "}");
  } else {
    server.send(400, "application/json", "{\"error\":\"Missing value parameter\"}");
  }
}

// API: Статус устройства (/api/status)
void handleApiStatus() {
  String json = "{";
  json += "\"power\":" + String(powerState ? "true" : "false") + ",";
  json += "\"brightness\":" + String(currentBrightness) + ",";
  json += "\"pct\":" + String(map(currentBrightness, 0, 255, 0, 100));
  json += "}";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  // Настройка ШИМ
  ledcAttach(coldPin, freq, resolution);
  // ledcAttach(warmPin, freq, resolution); // Завтра раскомментируешь
  applyLight();

  // Подключение к домашней сети
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nУспешно подключено!");
  Serial.print("IP адрес лампы в домашней сети: ");
  Serial.println(WiFi.localIP());

  // Маршруты страницы и API
  server.on("/", handleRoot);
  server.on("/api/power", handleApiPower);
  server.on("/api/brightness", handleApiBrightness);
  server.on("/api/status", handleApiStatus);
  
  server.begin();
  Serial.println("HTTP сервер запущен!");
}

void loop() {
  server.handleClient();
}