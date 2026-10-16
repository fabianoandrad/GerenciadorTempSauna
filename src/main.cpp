#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include <Preferences.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LED_PIN 2       // Definição do pino para o LED do proprio ESP32
#define ONE_WIRE_BUS 4  // Definição do pino pro sensor DS18B20
#define FLAME_DO_PIN 27 // Definição do pino para o sensor de chama digital
#define FLAME_AO_PIN 34 // Definição do pino para o sensor de chama analógico
#define DHT_PIN 33      // Definição do pino para o sensor DHT22

#define DHT_TYPE DHT22 // Definição do tipo do sensor DHT22


// =============================
// Sensores
// Define uma instancia do oneWire para comunicacao com o sensor
// =============================
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorDS;
DHT dht(DHT_PIN, DHT_TYPE);

// =============================
// WebServer
// =============================
WebServer server(80);

// =============================
// LocalStorage do esp32
// =============================
Preferences preferences;

// =============================
// WiFi
// =============================
const char *ssid = "AFFA_2.4G";
const char *password = "Affa@2307";

// =============================
// Variáveis globais
// =============================
bool ledState = false;
float tempSauna = 0.0; // Variável para armazenar a temperatura da sauna
float tempPool = 0.0; // Variavel para armazenar a temperatura da piscina
float flameLevel = 0.0; // Variável para armazenar o nível de chama
float tempRoom = 0.0; // Variável para armazenar a temperatura ambiente
float humidityRoom = 0.0; // Variável para armazenar a umidade ambiente
int flameRaw = 0; // Variável para armazenar o valor bruto do sensor de chama
bool flameDetected = false; // Variável para armazenar se a chama foi detectada

float maxTempSauna = 0; 
float minTempSauna = 0;
float maxTempPool = 0;
float minTempPool = 0;
float maxTempRoom = 0;
float minTempRoom = 0;
float maxHumidityRoom = 0;
float minHumidityRoom = 0;

// Declarar as funções antes de usá-las
void postLedState();
void getStatus();
void handleRoot();
void handleCSS();
void handleJS();
void readFlameSensor();

// =============================
// HTML
// Arquivos estáticos
// =============================
void handleRoot()
{
  File file = LittleFS.open("/index.html", "r");

  if (!file)
  {
    server.send(404, "text/plain", "index.html nao encontrado");
    return;
  }

  server.streamFile(file, "text/html");
  file.close();
}

// =============================.
// CSS
// =============================
void handleCSS()
{
  File file = LittleFS.open("/style.css", "r");

  if (!file)
  {
    server.send(404, "text/plain", "style.css nao encontrado");
    return;
  }

  server.streamFile(file, "text/css");
  file.close();
}

// =============================
// JavaScript
// =============================
void handleJS()
{
  File file = LittleFS.open("/script.js", "r");

  if (!file)
  {
    server.send(404, "text/plain", "script.js nao encontrado");
    return;
  }

  server.streamFile(file, "application/javascript");
  file.close();
}

// =============================
// Favicon
// =============================
void handleFavicon()
{
  File file = LittleFS.open("/favicon.svg", "r");

  if (!file)
  {
    server.send(404, "text/plain", "Favicon nao encontrado");
    return;
  }

  server.streamFile(file, "image/svg+xml");

  file.close();
}

// =============================
// API STATUS
// =============================
void getStatus()
{
    JsonDocument doc;

    doc["status"] = "online";
    doc["led"] = ledState ? "ligado" : "desligado";
    doc["tempSauna"] = tempSauna;
    doc["maxTempSauna"] = maxTempSauna;
    doc["minTempSauna"] = minTempSauna;
    doc["tempPool"] = tempPool;
    doc["maxTempPool"] = maxTempPool;
    doc["minTempPool"] = minTempPool;
    doc["tempRoom"] = tempRoom;
    doc["maxTempRoom"] = maxTempRoom;
    doc["minTempRoom"] = minTempRoom;
    doc["humidityRoom"] = humidityRoom;
    doc["maxHumidityRoom"] = maxHumidityRoom;
    doc["minHumidityRoom"] = minHumidityRoom;
    doc["flameLevel"] = flameLevel;
    doc["flameDetected"] = flameDetected;

    String json;
    serializeJson(doc, json);

  server.send(200, "application/json", json);
}

// =============================
// FUNÇÕES AUXILIARES
// =============================
void postLedState()
{
  String body = server.arg("plain");
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error)
  {
    server.send(400, "application/json", "{\"erro\":\"JSON invalido\"}");
    return;
  }

  ledState = doc["state"];

  digitalWrite(LED_PIN, ledState ? HIGH : LOW);

  Serial.print(ledState ? "Led Ligado" : "Led Desligado");

  // IMPORTANTE: responder ao navegador
  String json = "{";
  json += "\"status\":\"ok\",";
  json += "\"led\":";
  json += ledState ? "\"ligado\"" : "\"desligado\"";
  json += "}";

  server.send(200, "application/json", json);
}

// ===========================================
// FUNÇÃO PARA LEITURA DO SENSOR DE CHAMA
// ===========================================
float calculateFlameLevel(int raw)
{
  float level;

  if (raw >= 3500)
  {
    level = 0.0;
  }
  else if (raw >= 2800)
  {
    level = 25.0 - ((raw - 2800) * 25.0 / (3500 - 2800));
  }
  else if (raw >= 1800)
  {
    level = 50.0 - ((raw - 1800) * 25.0 / (2800 - 1800));
  }
  else if (raw >= 900)
  {
    level = 100.0 - ((raw - 900) * 50.0 / (1800 - 900));
  }
  else
  {
    level = 100.0;
  }

  return level;
}

void readFlameSensor()
{
  flameRaw = analogRead(FLAME_AO_PIN);

  flameLevel = calculateFlameLevel(flameRaw);

  flameDetected = (digitalRead(FLAME_DO_PIN) == HIGH);
}

void readTemperatureAmbient()
{
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (!isnan(temperature))
  {
    tempRoom = temperature;
  }

  if (!isnan(humidity))
  {
    humidityRoom = humidity;
  }
}

void setup()
{
  Serial.begin(115200); // Inicializa a comunicação serial
  sensors.begin();      // Inicializa a biblioteca DallasTemperature
  dht.begin();
  preferences.begin("sauna-data", false);

  maxTempSauna = preferences.getFloat("maxTempSaunaNVS", 0); 
  minTempSauna = preferences.getFloat("minTempSaunaNVS", 0); 
  maxTempPool = preferences.getFloat("maxTempPoolNVS", 0);
  minTempPool = preferences.getFloat("minTempPoolNVS", 0);
  maxTempRoom = preferences.getFloat("maxTempRoomNVS", 0);
  minTempRoom = preferences.getFloat("minTemRoomNVS", 0);
  maxHumidityRoom = preferences.getFloat("maxHumidityRoomNVS", 0);
  minHumidityRoom = preferences.getFloat("minHumidityRoomNVS", 0);

  // Configura os pinos
  pinMode(LED_PIN, OUTPUT);
  pinMode(FLAME_AO_PIN, INPUT);
  pinMode(FLAME_DO_PIN, INPUT);

  analogReadResolution(12); // Configura a resolução da leitura analógica para 12 bits (0-4095)

  // Obtem endereço do sensor
  if (!sensors.getAddress(sensorDS, 0))
  {
    Serial.println("Erro ao obter endereço do sensor!");
  }

  // LittleFS
  if (!LittleFS.begin(true))
  {
    Serial.println("Erro ao inicializar LittleFS!");
    return;
  }

  // Wi-Fi
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.print("Conectando ao Wi-Fi");

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < 15000)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nFalha ao conectar ao Wi-Fi");
  }
  else
  {
    Serial.println("\nWi-Fi conectado!");
  }

  // while(true){
  //   float Sensor_DS18B20 = sensors.getTempC(sensorDS);
  //   delay(500);
  //   Serial.print("Temp: "); Serial.print(Sensor_DS18B20); Serial.print("°C");
  // }

  Serial.println("Wi-Fi conectado!");

  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  // Rotas da página
  server.on("/", HTTP_GET, handleRoot);
  server.on("/style.css", HTTP_GET, handleCSS);
  server.on("/script.js", HTTP_GET, handleJS);

  // Rotas da API
  server.on("/api/status", HTTP_GET, getStatus);
  server.on("/api/ledState", HTTP_POST, postLedState);
  server.on("/favicon.svg", HTTP_GET, handleFavicon);

  server.onNotFound([]()
                    { server.send(
                          404,
                          "application/json",
                          "{\"erro\":\"rota nao encontrada\"}"); });

  server.begin();

  Serial.println("Servidor HTTP iniciado!");
}

void loop()
{
  readFlameSensor();        // Leitura do sensor de chama
  readTemperatureAmbient(); // Leitura do sensor de temperatura ambiente

  server.handleClient(); // Processar requisições HTTP

  static unsigned long lastTemp = 0;

  if (millis() - lastTemp > 2000)
  {
    lastTemp = millis();

    sensors.requestTemperatures(); // Leitura da temperatura da sauna

    float temp = sensors.getTempCByIndex(0);

    if (temp != DEVICE_DISCONNECTED_C)
    {
      tempSauna = temp;
    }
  }

  // =================================================
  // Armazenamos na NVS do esp32 os dados
  // =================================================
  if(tempSauna > maxTempSauna)
  {
    maxTempSauna = tempSauna;
    preferences.putFloat("maxTempSaunaNVS", maxTempSauna);
  }
  if(tempSauna < minTempSauna || minTempSauna == 0.00)
  {
    minTempSauna = tempSauna;
    preferences.putFloat("minTempSaunaNVS", minTempSauna);
  }

  if(tempPool > maxTempPool)
  {
    maxTempPool = tempPool;
    preferences.putFloat("maxTempPoolNVS", maxTempPool);
  }
  if(tempPool < minTempPool || minTempPool == 0.00)
  {
    minTempPool = tempPool;
    preferences.putFloat("minTempPoolNVS", minTempPool);
  }

  if(tempRoom > maxTempRoom)
  {
    maxTempRoom = tempRoom;
    preferences.putFloat("maxTempRoomNVS", maxTempRoom);
  }
  if(tempRoom < minTempRoom || minTempRoom == 0.00)
  {
    minTempRoom = tempRoom;
    preferences.putFloat("minTempRoomNVS", minTempRoom);
  }

  if(humidityRoom > maxHumidityRoom)
  {
    maxHumidityRoom = humidityRoom;
    preferences.putFloat("maxHumidityRoomNVS", maxHumidityRoom);
  }
  if(humidityRoom < minHumidityRoom || minHumidityRoom == 0.00)
  {
    minHumidityRoom = humidityRoom;
    preferences.putFloat("minHumidityRoomNVS", minHumidityRoom);
  }

  // =================================================
  // Exibir a temperatura da sauna no monitor serial
  // =================================================
  Serial.print("Temperatura da sauna: ");
  Serial.print(tempSauna);
  Serial.println(" °C");
  
  // =================================
  // Leitura da temperatura ambiente
  // =================================
  Serial.print("Temp Ambiente: ");
  Serial.print(tempRoom, 1);
  Serial.print(" °C | Umidade: ");
  Serial.print(humidityRoom, 1);
  Serial.println(" %");

  // ===========================================
  // Exibir o nível de chama no monitor serial
  // ===========================================
  Serial.print("RAW: ");
  Serial.print(flameRaw);
  Serial.print(" | Flame Level: ");
  Serial.print(flameLevel, 2); // Exibir com duas casas decimais
  Serial.print("% | Detectado: ");

  if (flameDetected)
    Serial.println("SIM");
  else
    Serial.println("NAO");


  Serial.println("-----------------------------");
  delay(1000);
}
