//
// Created by gustavo on 22/07/2026.
//

#ifndef PROJ_AGUA_DOIS_MONITOR_AGUA_H
#define PROJ_AGUA_DOIS_MONITOR_AGUA_H

#include "Arduino.h"
#include "config.h"

struct Metricas {
    float ph;
    float oxi_dissolvido;
    float temperatura;
    float turbidez;
    float condutividade;
    float vazao;
    bool chuva;
    //status
    String statuspH;
    String statusChuva;
    String statusVazao;
    String statusOxi_Dissolvido;
    String statusTemperatura;
    String statusTurbidez;
    String statusCondutividade;
};

Metricas lerMonitor();

Metricas lerSensores();

void setupSensorVazao();

#endif //PROJ_AGUA_DOIS_MONITOR_AGUA_H
