#include <Arduino.h>
#include "config.h"
#include "credenciais.h"
#include "monitor_agua.h"
#include "WiFi.h"
#include <PubSubClient.h>
#include <ArduinoJson.h>

WiFiClient espCliente;
PubSubClient client(espCliente);

void setup() {

    //definicao de portas
    pinMode(SENSOR_CHUVA,INPUT);
    pinMode(SENSOR_TEMPERATURA,INPUT);
    pinMode(SENSOR_TURBIDEZ,INPUT);
    pinMode(SENSOR_VAZAO,INPUT);
    pinMode(SENSOR_PH,INPUT);
    pinMode(SENSOR_OXIGENIO_DISSOLVIDO,INPUT);

    //inicializacoes
    Serial.begin(9600);
    WiFi.begin(WIFI_SSID,WIFI_SENHA);

    while (WiFi.status()!=WL_CONNECTED) {
        delay(500);
        Serial.print("-.");
    }
    if (WiFi.status()==WL_CONNECTED) {
        Serial.println("\nWiFi conectado");
        Serial.println(WiFi.localIP());
    }

    client.setServer(AZ_HOST,8883);
}

void loop() {



}