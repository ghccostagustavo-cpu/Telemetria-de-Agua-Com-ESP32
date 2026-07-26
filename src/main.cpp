#include <Arduino.h>
#include "config.h"
#include "credenciais.h"
#include "monitor_agua.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <az_iot_hub_client.h>
#include <az_core.h>
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>


WiFiClientSecure espCliente;
PubSubClient client(espCliente);

void setup() {

    //inicializacoes
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID,WIFI_SENHA);

    while (WiFi.status()!=WL_CONNECTED) {
        delay(500);
        Serial.print("-.");
    }
    if (WiFi.status()==WL_CONNECTED) {
        Serial.println("\nWiFi conectado");
        Serial.println(WiFi.localIP());
    }


    //azure - inicializando cliente
    String user = String(AZ_HOST) + "/" + String(AZ_ID_DEVICE) + "/?api-version=2021-04-12";
    client.setServer(AZ_HOST,8883);
    espCliente.setInsecure();
    client.connect(
        AZ_ID_DEVICE,
        user.c_str(),
        AZ_SAS_TOKEN
        );
    if (client.connect(AZ_ID_DEVICE, user.c_str(), AZ_SAS_TOKEN))
        Serial.println("\nMQTT Conectado!");
    else {
        Serial.print("Falha na conexao MQTT, rc = ");
        Serial.println(client.state());
    }

    //definicao de portas
    pinMode(SENSOR_CHUVA,INPUT);
    pinMode(SENSOR_TEMPERATURA,INPUT);
    pinMode(SENSOR_TURBIDEZ,INPUT);
    pinMode(SENSOR_VAZAO,INPUT);
    pinMode(SENSOR_PH,INPUT);
    pinMode(SENSOR_OXIGENIO_DISSOLVIDO,INPUT);

}

void loop() {

    if (!client.connected()) {
        String user = String(AZ_HOST) + "/" + String(AZ_ID_DEVICE) + "/?api-version=2021-04-12";
        client.connect(AZ_ID_DEVICE, user.c_str(), AZ_SAS_TOKEN);
    }

    client.loop();


    Metricas dados = lerMonitor();
    //criando documento json para publicar no azure
    JsonDocument doc;
    doc["ph"]= dados.ph;
    doc["temperatura"] = dados.temperatura;
    doc["turbidez"]=dados.turbidez;
    doc["oxigenio_dissolvido"]=dados.oxi_dissolvido;
    doc["condutividade"]=dados.condutividade;
    doc["vazao"]=dados.vazao;
    //status
    doc["statuspH"]=dados.statuspH;
    doc["statusTemperatura"]=dados.statusTemperatura;
    doc["statusCondutividade"]=dados.statusCondutividade;
    doc["statusOxiDissolvido"]=dados.statusOxi_Dissolvido;
    doc["statusTurbidez"]=dados.statusTurbidez;
    doc["statusVazao"]=dados.statusVazao;

    String jsonString;
    serializeJson(doc,jsonString);

    //publicando no azure
    String topic_az = "devices/" + String(AZ_ID_DEVICE) + "/messages/events/";
    client.publish(topic_az.c_str(),jsonString.c_str());

    delay(6000);
}