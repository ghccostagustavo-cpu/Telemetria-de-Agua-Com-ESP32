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
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID,WIFI_SENHA);

    while (WiFi.status()!=WL_CONNECTED) {
        delay(500);
        Serial.print("-.");
    }

    Serial.println("WiFi conectado");
    Serial.println(WiFi.localIP());

    client.setServer(NOME_SERVIDOR,8883);
}

void loop() {

}