#include <Arduino.h>
#include "config.h"
#include "monitor_agua.h"
#include <OneWire.h>
#include <Wire.h>
#include <DallasTemperature.h>
#include "DFRobot_PH.h"
#include "DFRobot_EC.h"

//--temperatura
OneWire temp_sensor(SENSOR_TEMPERATURA);
DallasTemperature sensors(&temp_sensor);

float lerTemperatura(){
    sensors.begin();
    sensors.requestTemperatures();
    return sensors.getTempCByIndex(0);
}

//--ph
DFRobot_PH ph_sensor;
float lerPH(float temperatura){
    ph_sensor.begin();
    float voltagem = analogReadMilliVolts(SENSOR_PH);
    return ph_sensor.readPH(voltagem, temperatura);
}

//--condutividade
DFRobot_EC ce;
float lerCondutividade(float temperatura){
    float voltagem_cond = analogRead(SENSOR_CONDUTIVIDADE) / 1024.0 * 5000;
    return ce.readEC(voltagem_cond, temperatura);
}
//--oxigenio dissolvido

// calibração padrão DFRobot para o SEN0237
#define CAL1_V 1600
#define CAL1_T 25
#define CAL2_V 1300
#define CAL2_T 15

const uint16_t DO_Table[41] = {
    14460, 14220, 13820, 13440, 13090, 12740, 12420, 12110, 11810, 11530,
    11260, 11010, 10770, 10530, 10300, 10080, 9860, 9660, 9460, 9270,
    9080, 8900, 8730, 8570, 8410, 8250, 8110, 7960, 7820, 7690,
    7560, 7430, 7300, 7180, 7070, 6950, 6840, 6730, 6630, 6530, 6410};

float lerOxigenioDissolvido(float temperatura){
    float voltagem_oxi = analogRead(SENSOR_OXIGENIO_DISSOLVIDO) / 1024.0 * 5000;
    uint16_t v_saturation = (uint32_t)CAL1_V + (uint32_t)35 * temperatura - (uint32_t)CAL1_T * 35;
    return voltagem_oxi * DO_Table[(int)temperatura] / v_saturation;
}

//vazao
volatile int contador_pulsos = 0;

void IRAM_ATTR Flow() {
    contador_pulsos++;
}

void setupSensorVazao(){
    pinMode(SENSOR_VAZAO, INPUT);
    attachInterrupt(digitalPinToInterrupt(SENSOR_VAZAO), Flow, RISING);
}

double lerVazao(){
   contador_pulsos = 0;
   interrupts();
   delay(1000);
   noInterrupts();
    double vazao = contador_pulsos * 2.25;
    vazao  *= 60/1000;// L/min
    return vazao;
}

Metricas lerSensores(){
    Metricas s;
    s.chuva = digitalRead(SENSOR_CHUVA);
    s.temperatura = lerTemperatura();
    s.ph = lerPH(s.temperatura);
    s.condutividade = lerCondutividade(s.temperatura);
    s.oxi_dissolvido = lerOxigenioDissolvido(s.temperatura);
    s.turbidez=analogRead(SENSOR_TURBIDEZ) /(5.0/1024.0);
    s.vazao=lerVazao();

        //----status
    
    //ph
    if(s.ph>=LIMITE_PH_B_WARN && s.ph<LIMITE_PH_B_CRIT)
        s.statuspH = "pH perigosamente basico";
    else if(s.ph>=LIMITE_PH_B_CRIT)
        s.statuspH = "pH criticamente basico";
    else if(s.ph<=LIMITE_PH_A_WARN && s.ph>LIMITE_PH_A_CRIT)
        s.statuspH = "pH perigosamente acido";
    else if(s.ph<=LIMITE_PH_A_CRIT)
        s.statuspH = "pH criticamente acido";
    
    //chuva
    if(s.chuva==0)
        s.statusChuva = "Sem chuva";
    else if(s.chuva==1)
        s.statusChuva = "Chovendo";

    //condutividade
    if(s.condutividade>=LIMITE_CONDUT_WARN && s.condutividade<LIMITE_CONDUT_CRIT)
        s.statusCondutividade = "Condutividade perigosamente alta";
    else if(s.condutividade>=LIMITE_CONDUT_CRIT)
        s.statusCondutividade = "Condutividade criticamente alta";

if (s.oxi_dissolvido <= LIMITE_OXIGENIO_CRIT)
        s.statusOxi_Dissolvido = "Oxigenio dissolvido criticamente baixo";
else if (s.oxi_dissolvido <= LIMITE_OXIGENIO_WARN)
    s.statusOxi_Dissolvido = "Oxigenio dissolvido perigosamente baixo";

    //turbidez
    if(s.turbidez>=LIMITE_TURB_WARN && s.turbidez<LIMITE_TURB_CRIT)
        s.statusTurbidez = "Turbidez perigosamente alta";
    else if(s.turbidez>=LIMITE_TURB_CRIT)
        s.statusTurbidez = "Turbidez criticamente alta";


    return s;
}
