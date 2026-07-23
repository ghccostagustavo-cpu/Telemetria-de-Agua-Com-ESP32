#ifndef PROJ_AGUA_DOIS_CONFIG_H
#define PROJ_AGUA_DOIS_CONFIG_H

#include <Arduino.h>

//pinos
#define SENSOR_CHUVA 22
#define SENSOR_TEMPERATURA 21
#define SENSOR_TURBIDEZ 16
#define SENSOR_VAZAO 17
#define SENSOR_PH 12
#define SENSOR_CONDUTIVIDADE 13
#define SENSOR_OXIGENIO_DISSOLVIDO 14

//sistema
#define ID "ESP32_SENSOR_PAMPULHA_001"
#include <credenciais.h>

//-----limites e definicoes de valores

//ph
#define LIMITE_PH_B_CRIT 9.0f
#define LIMITE_PH_B_WARN 8.5f

#define LIMITE_PH_A_CRIT 5.0f
#define LIMITE_PH_A_WARN 6.0f

//oxigenio dissolvido
#define LIMITE_OXIGENIO_WARN 4.0f
#define LIMITE_OXIGENIO_CRIT 3.0f

//condutividade
#define LIMITE_CONDUT_WARN 500.0f
#define LIMITE_CONDUT_CRIT 1000.0f

//turbidez
#define LIMITE_TURB_WARN 100.0f
#define LIMITE_TURB_CRIT 200.0f

#endif //PROJ_AGUA_DOIS_CONFIG_H
