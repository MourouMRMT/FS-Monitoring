#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebSrv.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

#define ONBOARD_LED  2

//ESP32 IP @: 172.20.10.9
// Replace with your network credentials
const char* ssid = "Mourou";
const char* password = "bimbapbam";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Create a WebSocket object
AsyncWebSocket ws("/ws");

// Json Variable to Hold Sensor Readings
JSONVar readings;

// Timer variables
unsigned short lastTime = 0;
unsigned short timerDelay = 3000;

//rand value for emulated frame
unsigned short r_T;
unsigned short r_TdC;
unsigned short r_ef;
// Get Can frame and return JSON object
String getSensorReadings(){
  readings["temperature"] = String(r_T);
  readings["TauxdeCharge"] =  String(r_TdC);
  readings["etatfeux"] = String(r_ef);
  readings["vitmoteur"]=String(r_TdC);
  String jsonString = JSON.stringify(readings);
  return jsonString;
}

// Initialize SPIFFS
void initSPIFFS() {
  if (!SPIFFS.begin(true)) {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sensorReadings) {
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    //data[len] = 0;
    //String message = (char*)data;
    // Check if the message is "getReadings"
    //if (strcmp((char*)data, "getReadings") == 0) {
      //if it is, send current sensor readings
      String sensorReadings = getSensorReadings();
      Serial.print(sensorReadings);
      notifyClients(sensorReadings);
    //}
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void blink()
{
  delay(800);
  digitalWrite(ONBOARD_LED,HIGH);
  delay(200);
  digitalWrite(ONBOARD_LED,LOW);
}

void setup() {
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initWebSocket();

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();

  pinMode(ONBOARD_LED,OUTPUT);
  //Initialize random number to emulate frame
  randomSeed(analogRead(0));
}

void loop() {
  blink();
  if ((millis() - lastTime) > timerDelay) {
    String sensorReadings = getSensorReadings();
    Serial.print(sensorReadings);
    notifyClients(sensorReadings);

  lastTime = millis();
  Serial.println("tic");
  }
  r_T=random(20,40);
  r_TdC=random(30,90);
  r_ef=random(0,2);
  ws.cleanupClients();
}