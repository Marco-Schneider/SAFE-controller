#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "SAFE.hpp"
#include "soc/rtc_wdt.h"

const char* ssid = "Claro5G";
const char* password = "96556798";

const char* safeStatus = "";

bool isSafeClosed;
bool shouldUpdateSafeStatus;

AsyncWebServer server(80);

/* Funções para voltar a trava para a posição de descanso */
void handleClosingLockOne() {
  digitalWrite(PORTA_1, HIGH);
  digitalWrite(LED_PORTA_1, LOW);
}


void handleClosingLockTwo() {
  digitalWrite(PORTA_2, HIGH);
  digitalWrite(LED_PORTA_2, LOW);
}

void handleClosingLockThree() {
  digitalWrite(PORTA_3, HIGH);
  digitalWrite(LED_PORTA_3, LOW);
}

void handleClosingLockFour() {
  digitalWrite(PORTA_4, HIGH);
  digitalWrite(LED_PORTA_4, LOW);
}

void handleClosingLockFive() {
  digitalWrite(PORTA_5, HIGH);
  digitalWrite(LED_PORTA_5, LOW);
}

void handleClosingLockSix() {
  digitalWrite(PORTA_6, HIGH);
  digitalWrite(LED_PORTA_6, LOW);
}

void updateSafeStatus(const String& status) {

  HTTPClient http;

  StaticJsonDocument<256> doc;
  JsonObject fields = doc.createNestedObject("fields");
  JsonObject statusField = fields.createNestedObject("status");
  statusField["stringValue"] = status;

  String payload;
  serializeJson(doc, payload);

  const char* url = "https://firestore.googleapis.com/v1/projects/safe-auth-v2/databases/(default)/documents/safe/state";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.PATCH(payload);
  http.end();

  if (httpResponseCode == HTTP_CODE_OK) {
    Serial.println("Firestore document updated successfully");
  } else {
    Serial.print("Failed to update Firestore document. Error code: ");
    Serial.println(httpResponseCode);
  }

}

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi network...");
  }

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
    if(request->url() == "/control/retrieve") {

      StaticJsonDocument<256> doc;
      deserializeJson(doc, data);
      JsonArray objects = doc["objects"];

      for(int i = 0; i < objects.size(); i++) {
        int door = objects[i]["door"].as<int>();
        int state = objects[i]["state"].as<int>();
        switch (door) {
          case 1:
            digitalWrite(PORTA_1, LOW);
            digitalWrite(LED_PORTA_1, HIGH);
            break;
          case 2:
            digitalWrite(PORTA_2, LOW);
            digitalWrite(LED_PORTA_2, HIGH);
            break;
          case 3:
            digitalWrite(PORTA_3, LOW);
            digitalWrite(LED_PORTA_3, HIGH);
            break;
          case 4:
            digitalWrite(PORTA_4, LOW);
            digitalWrite(LED_PORTA_4, HIGH);
            break;
          case 5:
            digitalWrite(PORTA_5, LOW);
            digitalWrite(LED_PORTA_5, HIGH);
            break;
          case 6:
            digitalWrite(PORTA_6, LOW);
            digitalWrite(LED_PORTA_6, HIGH);
            break;
          default:
            break;
        }
      }
      safeStatus = "itemRetrieval";
       shouldUpdateSafeStatus = true;
    }
  });

  server.begin();

  // Configurando os eventos de interrupção para alterar o estado da trava
  // Porta fechada -> sensor ativo
  // Porta é aberta -> borda de descida no sensor
  // Abrir a trava
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_1), handleClosingLockOne, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_2), handleClosingLockTwo, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_3), handleClosingLockThree, FALLING);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_4), handleClosingLockFour, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_5), handleClosingLockFive, CHANGE);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_6), handleClosingLockSix, CHANGE);

  // Setup dos pinos
  pinMode(PORTA_1, OUTPUT);
  pinMode(PORTA_2, OUTPUT);
  pinMode(PORTA_3, OUTPUT);
  pinMode(PORTA_4, OUTPUT);
  pinMode(PORTA_5, OUTPUT);
  pinMode(PORTA_6, OUTPUT);

  pinMode(SENSOR_PORTA_1, INPUT);
  pinMode(SENSOR_PORTA_2, INPUT);
  pinMode(SENSOR_PORTA_3, INPUT);
  pinMode(SENSOR_PORTA_4, INPUT);
  pinMode(SENSOR_PORTA_5, INPUT);
  pinMode(SENSOR_PORTA_6, INPUT);

  pinMode(LED_PORTA_1, OUTPUT);
  pinMode(LED_PORTA_2, OUTPUT);
  pinMode(LED_PORTA_3, OUTPUT);
  pinMode(LED_PORTA_4, OUTPUT);
  pinMode(LED_PORTA_5, OUTPUT);
  pinMode(LED_PORTA_6, OUTPUT);

  // Configurando os estados inciais das travas
  digitalWrite(PORTA_1, HIGH);
  digitalWrite(PORTA_2, HIGH);
  digitalWrite(PORTA_3, HIGH);
  digitalWrite(PORTA_4, HIGH);
  digitalWrite(PORTA_5, HIGH);
  digitalWrite(PORTA_6, HIGH);

  // Configurando estado inicial dos leds
  digitalWrite(LED_PORTA_1, LOW);
  digitalWrite(LED_PORTA_2, LOW);
  digitalWrite(LED_PORTA_3, LOW);
  digitalWrite(LED_PORTA_4, LOW);
  digitalWrite(LED_PORTA_5, LOW);
  digitalWrite(LED_PORTA_6, LOW);

}

void loop() {

  if(shouldUpdateSafeStatus) {
    updateSafeStatus(safeStatus);
    shouldUpdateSafeStatus = false;
  }

  if(safeStatus == "itemRetrieval") {

    isSafeClosed = digitalRead(PORTA_1) && digitalRead(PORTA_2) && digitalRead(PORTA_3) &&
                   digitalRead(PORTA_4) && digitalRead(PORTA_5) && digitalRead(PORTA_6) &&
                   digitalRead(SENSOR_PORTA_1) && digitalRead(SENSOR_PORTA_2) && digitalRead(SENSOR_PORTA_3) &&
                   digitalRead(SENSOR_PORTA_4) && digitalRead(SENSOR_PORTA_5) && digitalRead(SENSOR_PORTA_6);

    if(isSafeClosed) {
      Serial.println("THE RETRIEVAL HAS FINISHED!!!");
      safeStatus = "Closed";
      updateSafeStatus(safeStatus);
      delay(5000);
    }
  }

  Serial.print("Current status: ");
  Serial.println(safeStatus);

}
