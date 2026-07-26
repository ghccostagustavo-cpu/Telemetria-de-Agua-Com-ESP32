#include <Arduino.h>
#include "monitor_agua.h"



Metricas lerMonitor(){
    Metricas m;
    m.ph = random(100,1500)/100.0f;
    m.chuva = random(0,2);
    m.condutividade = random(0,10000)/10.0f;
    m.oxi_dissolvido = random(0,100)/10.0f;
    m.temperatura=random(150,380)/10.0f;
    m.turbidez=random(0,10000)/10.0f;
    m.vazao=random(50,1000)/10.f;
    
    //----status
    
    //ph
    if(m.ph>=LIMITE_PH_B_WARN && m.ph<LIMITE_PH_B_CRIT)
        m.statuspH = "pH perigosamente basico";
    else if(m.ph>=LIMITE_PH_B_CRIT)
        m.statuspH = "pH criticamente basico";
    else if(m.ph<=LIMITE_PH_A_WARN && m.ph>LIMITE_PH_A_CRIT)
        m.statuspH = "pH perigosamente acido";
    else if(m.ph<=LIMITE_PH_A_CRIT)
        m.statuspH = "pH criticamente acido";
    
    //chuva
    if(m.chuva==0)
        m.statusChuva = "Sem chuva";
    else if(m.chuva==1)
        m.statusChuva = "Chovendo";

    //condutividade
    if(m.condutividade>=LIMITE_CONDUT_WARN && m.condutividade<LIMITE_CONDUT_CRIT)
        m.statusCondutividade = "Condutividade perigosamente alta";
    else if(m.condutividade>=LIMITE_CONDUT_CRIT)
        m.statusCondutividade = "Condutividade criticamente alta";

if (m.oxi_dissolvido <= LIMITE_OXIGENIO_CRIT)
        m.statusOxi_Dissolvido = "Oxigenio dissolvido criticamente baixo";
else if (m.oxi_dissolvido <= LIMITE_OXIGENIO_WARN)
    m.statusOxi_Dissolvido = "Oxigenio dissolvido perigosamente baixo";

    //turbidez
    if(m.turbidez>=LIMITE_TURB_WARN && m.turbidez<LIMITE_TURB_CRIT)
        m.statusTurbidez = "Turbidez perigosamente alta";
    else if(m.turbidez>=LIMITE_TURB_CRIT)
        m.statusTurbidez = "Turbidez criticamente alta";

    
    return m;

}