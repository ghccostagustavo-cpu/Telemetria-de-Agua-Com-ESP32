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

String mqttUser() {
    return String(AZ_HOST) + "/" + String(AZ_ID_DEVICE) + "/?api-version=2021-04-12";
}

bool conectarAzure() {
    if (client.connected())
        return true;
    

    client.disconnect();
    String user = mqttUser();
    if (client.connect(AZ_ID_DEVICE, user.c_str(), AZ_SAS_TOKEN)) {
        Serial.println("\nMQTT Conectado!");
        return true;
    }

    Serial.print("Falha na conexao MQTT, rc = ");
    Serial.println(client.state());
    return false;
}

void setup() {


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

    delay(2000);

    client.setServer(AZ_HOST,8883);
    client.setBufferSize(2048);
    client.setKeepAlive(60);
    client.setSocketTimeout(60);

    espCliente.setInsecure();
    conectarAzure();

    //definicao de portas
    pinMode(SENSOR_CHUVA,INPUT);
    pinMode(SENSOR_TEMPERATURA,INPUT);
    pinMode(SENSOR_TURBIDEZ,INPUT);
    pinMode(SENSOR_VAZAO,INPUT);
    pinMode(SENSOR_PH,INPUT);
    pinMode(SENSOR_OXIGENIO_DISSOLVIDO,INPUT);
    pinMode(SENSOR_CONDUTIVIDADE,INPUT);

    //funcao do sensor de vazao setup
    //setupSensorVazao();
}

void loop() {

    if (!conectarAzure()) {
        delay(5000);
        return;
    }

    client.loop();

    
    //Segue a captura das informacoes. Conforme explicitado no readme, temos a funcao de simular dados com random (monitor_agua_simulador.cpp)
    //Para captura real dos dados dos sensores, a funcao padrao esta construida em leitura.cpp

    //Abaixo, a aquisição dos dados. Para dados simulados (padrao), use a funcao abaixo
    Metricas dados = lerMonitor();//
    //Pra ler os sensores reais, descomente a funcao abaixo e comente a funcao acima
    //Metricas dados = lerSensores();
    


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

    String topic_az = "devices/" + String(AZ_ID_DEVICE) + "/messages/events/";
    if (!client.publish(topic_az.c_str(),jsonString.c_str())) {
        Serial.println("Falha ao publicar mensagem");
    }

    delay(6000);
}
