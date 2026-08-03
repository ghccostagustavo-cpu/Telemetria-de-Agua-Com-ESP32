# Arquitetura do sistema

## Componentes

- **ESP32 (PlatformIO/Arduino):** coleta/simula métricas de qualidade da água e publica telemetria.
- **Azure IoT Hub:** broker de ingestão MQTT/TLS para dispositivos.
- **Consumidor de eventos (cloud/backend):** processa mensagens do IoT Hub.
- **Azure Cosmos DB:** persistência histórica de telemetria e consultas analíticas.

## Fluxo de dados

1. O ESP32 lê os sensores (ou gera dados simulados).
2. O firmware monta payload JSON com métricas e status.
3. O dispositivo autentica com SAS Token e publica em:
   - `devices/{device_id}/messages/events/`
4. O IoT Hub disponibiliza os eventos para consumidores.
5. O consumidor transforma/enriquece os dados e grava no Cosmos DB.

## Observações técnicas

- Transporte: **MQTT sobre TLS (porta 8883)**.
- Reautenticação/reconexão MQTT ocorre no loop quando necessário.
- Limites de alerta por métrica ficam centralizados em `include/config.h`.

## Diagrama simplificado

```text
[Sensores] -> [ESP32 Firmware] -> [Azure IoT Hub] -> [Consumer/Function] -> [Cosmos DB]
```

## Artefatos de apoio

- Esquemático elétrico: `docs/assets/esquematico.svg`
