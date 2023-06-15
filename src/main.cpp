#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>
#include "SAFE.hpp"

const char* ssid = "Claro5G";
const char* password = "96556798";

AsyncWebServer server(80);

void handleRequest(AsyncWebServerRequest *request) {
  
  String message;

  if(request->hasParam("test", true)) {
    message = request->getParam("test", true)->value();
  }
  else {
    message = "No message sent";
  }
  request->send(200, "text/plain", "Hello, POST: " + message);
}

volatile bool doorOneTriggered = false;

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
          default:
            break;
        }
      }
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
  // attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_4), handleStateDoorFour, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_5), handleStateDoorFive, CHANGE);
  // attachInterrupt(digitalPinToInterrupt(SENSOR_PORTA_6), handleStateDoorSix, CHANGE);

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

  Serial.println("Estado de cada porta: ");
  Serial.print("Porta 1: ");
  Serial.println(isDoorOneOpened == true ? "aberta" : "fechada");

  Serial.print("Porta 2: ");
  Serial.println(isDoorTwoOpened == true ? "aberta" : "fechada");

  Serial.print("Porta 3: ");
  Serial.println(isDoorThreeOpened == true ? "aberta" : "fechada");

  Serial.println("---");

  isDoorOneOpened = digitalRead(SENSOR_PORTA_1) == HIGH ? false : true;
  isDoorTwoOpened = digitalRead(SENSOR_PORTA_2) == HIGH ? false : true;
  isDoorThreeOpened = digitalRead(SENSOR_PORTA_3) == HIGH ? false : true;

}
