# Telemetria de Água com ESP32

Projeto de telemetria embarcada com ESP32 para monitoramento de qualidade da água, publicando leituras no Azure IoT Hub via MQTT/TLS.

## Visão geral

O firmware coleta (ou simula) métricas de sensores, monta um payload JSON e publica periodicamente no tópico de eventos do dispositivo no Azure IoT Hub.

Arquitetura de referência do ecossistema:

`ESP32 -> Azure IoT Hub -> consumidor de eventos -> Azure Cosmos DB`

Detalhes do fluxo em [`docs/arquitetura.md`](docs/arquitetura.md).

## Estrutura do repositório

```text
.
├── include/          # headers e configuração
├── lib/              # bibliotecas locais do projeto
├── src/              # código-fonte do firmware
├── test/             # testes do PlatformIO
├── scripts/          # scripts utilitários (Azure CLI / monitoramento)
├── docs/             # documentação técnica
│   └── assets/
├── platformio.ini
└── README.md
```

## Pré-requisitos

- [PlatformIO Core](https://platformio.org/install)
- ESP32 conectado via USB
- Conta Azure com IoT Hub e dispositivo provisionado
- (Opcional) Azure CLI para geração de SAS Token

## Configuração

1. Crie o arquivo local de credenciais:

   ```bash
   cp include/credenciais.h.example include/credenciais.h
   ```

2. Preencha `include/credenciais.h` com seus dados reais (Wi-Fi e Azure).
3. Ajuste pinos e limites em `include/config.h` conforme o hardware.
4. Em `src/main.cpp`, escolha leitura simulada (`lerMonitor`) ou leitura real (`lerSensores`).

> `include/credenciais.h` é ignorado pelo Git e **não deve ser commitado**.

## Build, upload e monitor

```bash
# Build
pio run

# Upload para a placa
pio run -t upload

# Monitor serial
pio device monitor
```

## Scripts úteis

Em `scripts/Azure IoT Hub/`:

- `gerar_token.sh`: gera SAS Token para o dispositivo no IoT Hub.
- `monitorar.sh`: monitora eventos chegando no IoT Hub.

## Segurança

- Nunca versione tokens, chaves ou credenciais reais.
- Se alguma credencial já foi exposta em commit anterior, faça rotação imediata no Azure (Device Key/SAS Token).
