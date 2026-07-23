//
// Created by gustavo on 22/07/2026.
//

#ifndef PROJ_AGUA_DOIS_MONITOR_AGUA_H
#define PROJ_AGUA_DOIS_MONITOR_AGUA_H

#include "config.h"

struct Metricas {
    float ph;
    float oxi_dissolvido;
    float temperatura;
    float turbidez;
    float condutividade;
    float vazao;
    bool chuva;
    String status;
};

Metricas lerMonitor();

#endif //PROJ_AGUA_DOIS_MONITOR_AGUA_H
