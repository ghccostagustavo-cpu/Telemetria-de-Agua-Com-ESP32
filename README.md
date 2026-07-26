# Sistema de Monitoramento de Qualidade da Água com ESP32

Sistema embarcado baseado em ESP32 para monitoramento de parâmetros de qualidade da água, com envio de telemetria em tempo real para o Azure IoT Hub via MQTT.

## Visão geral

O firmware lê ou simula (conforme opção do usuário) as leituras dos sensores, monta um payload JSON com os valores e o status de cada métrica, e publica no tópico de eventos do dispositivo no Azure IoT Hub a cada 6 segundos.

## Hardware

- **Microcontrolador:** ESP32 (framework Arduino, via PlatformIO)

### Sensores

| Sensor | Modelo (exemplo) | Pino |
|---|---|---|
| pH | DFRobot SEN0161 | GPIO 12 |
| Oxigênio Dissolvido | DFRobot SEN0237 | GPIO 14 |
| Turbidez | DFRobot SEN0189 | GPIO 16 |
| Condutividade Elétrica (EC) | DFRobot DFR0300 | GPIO 13 |
| Temperatura (sonda impermeável) | DS18B20 | GPIO 21 |
| Vazão por pulso | YF-S201 (efeito Hall) | GPIO 17 |
| Chuva | Sensor de chuva simples (digital) | GPIO 22 |

### Passivos e acessórios

- Resistores de pull-up/pull-down conforme datasheet de cada sensor
- Divisores de tensão / condicionamento de sinal analógico onde necessário (pH, EC, turbidez, OD)
- Fonte de alimentação regulada para o ESP32 e sensores
- Cabeamento impermeável para os sensores submersos (pH, EC, OD, temperatura)

## Comunicação com o Azure IoT Hub

A comunicação é feita via **MQTT sobre TLS**, usando a biblioteca `PubSubClient`, direto no protocolo nativo do IoT Hub (sem depender do SDK completo da Azure para o transporte).

- **Host:** endpoint do IoT Hub (porta `8883`)
- **Usuário MQTT:** `{host}/{device_id}/?api-version=2021-04-12`
- **Autenticação:** SAS Token (gerado via Azure CLI, ver [Scripts](#scripts-auxiliares))
- **Tópico de publicação:** `devices/{device_id}/messages/events/`
- **Payload:** JSON serializado com `ArduinoJson`, contendo os 6 valores lidos (pH, temperatura, turbidez, oxigênio dissolvido, condutividade, vazão) e um status textual por métrica (ex: `"pH criticamente acido"`, `"Condutividade perigosamente alta"`)

O loop principal reconecta automaticamente ao broker MQTT caso a conexão caia, antes de cada ciclo de publicação.

> Os limites (warning/crítico) de cada métrica ficam centralizados em `include/config.h`.

## Modos de leitura: simulação vs. sensores reais

O projeto separa a lógica de leitura em **dois arquivos `.cpp` independentes**, ambos implementando a mesma interface (`Metricas lerMonitor()` / `Metricas lerSensores()`, declaradas em `include/monitor_agua.h`):

- **`src/monitor_agua_simulador.cpp`** — modo simulação. Gera valores pseudoaleatórios (`random()`) dentro de faixas plausíveis para cada sensor, útil para testar a integração com o Azure IoT Hub sem precisar do hardware físico montado.
- **`src/leitura.cpp`** — modo real. Lê os sensores físicos de fato (ADC do ESP32, `DallasTemperature` para o DS18B20, `DFRobot_PH`/`DFRobot_EC` para pH e condutividade, interrupção por pulso para a vazão).

A escolha de qual modo usar é feita em `src/main.cpp`, comentando/descomentando a chamada correspondente:

```cpp
Metricas dados = lerMonitor();   // simulação
// Metricas dados = lerSensores(); // sensores reais
```

> **Nota:** o conteúdo de `src/leitura.cpp` está atualmente todo comentado no repositório, "descomente" o bloco antes de compilar em modo real.

## Estrutura do projeto

```
proj_agua_dois/
├── include/
│   ├── config.h            # pinos, ID do dispositivo, limites de warning/crítico
│   ├── credenciais.h        # Wi-Fi e credenciais Azure (NÃO versionar com dados reais)
│   ├── monitor_agua.h       # struct Metricas e assinaturas das funções de leitura
│   ├── DFRobot_EC.h / .cpp  # lib do sensor de condutividade
├── src/
│   ├── main.cpp                    # setup, conexão Wi-Fi/MQTT, loop de publicação
│   ├── monitor_agua_simulador.cpp  # leitura simulada
│   └── leitura.cpp                 # leitura real dos sensores físicos
├── scripts/
│   ├── Azure IoT Hub/
│   │   ├── gerar_token.sh   # gera SAS token via Azure CLI
│   │   └── monitorar.sh     # monitora eventos chegando no IoT Hub em tempo real
│   ├── baixar_azurecli_linux.sh
│   └── baixar_azurecli_win.sh
└── platformio.ini
```

## Dependências (PlatformIO)

```ini
lib_deps =
    knolleary/PubSubClient@^2.8
    bblanchon/ArduinoJson@^7.2.2
    azure/Azure SDK for C@^1.1.8
    milesburton/DallasTemperature@^4.0.6
    dfrobot/DFRobot_PH@^1.0.0
    dfrobot/DFRobot_EC10@^1.0.0
```

## Configuração

1. Preencha `include/credenciais.h` com os dados reais (Wi-Fi, host do IoT Hub, ID do dispositivo e SAS token):

```cpp
#define WIFI_SSID "sua_rede"
#define WIFI_SENHA "sua_senha"

#define AZ_ID_DEVICE "id_do_dispositivo"
#define AZ_HOST "seuhub.azure-devices.net"
#define AZ_SAS_TOKEN "SharedAccessSignature sr=..."
```

2. Ajuste os pinos em `include/config.h` conforme sua fiação.
3. Escolha o modo de leitura (simulação ou real) em `src/main.cpp`.
4. Compile e envie para o ESP32:

```bash
pio run -t upload
```

## Scripts auxiliares

Localizados em `scripts/Azure IoT Hub/`, dependem do Azure CLI instalado (scripts de instalação inclusos para Linux e Windows):

- **`gerar_token.sh`** — gera um novo SAS token para o dispositivo autenticar no IoT Hub.
- **`monitorar.sh`** — abre um monitor de eventos em tempo real (`az iot hub monitor-events`), útil para conferir se as mensagens estão chegando durante testes.

## Observação de segurança

`include/credenciais.h` guarda dados sensíveis (Wi-Fi e SAS token). Mantenha esse arquivo fora do controle de versão (`.gitignore`) e nunca commite as credenciais reais do projeto. (Eu cometi esse erro num repositório privado e tive que fazer tudo de novo, rs).


## Sobre o projeto e uso de I.A

Esse é um projeto de "férias de faculdade", também. Ando estudando microcontroladores e tive ideia de fazer isso. Acho que é um projeto muito interessante e genuinamente interessante pro pessoal de microcontroladores. O próximo passo do projeto é lançar as leituras em um banco de dados também no ecossistema Azure.
Enfim, sobre I.A, é um projeto de estudo e interesse pessoal. Não faria o menor sentido vibecodar isso tudo, até porque eu nem aprenderia. Acho importante mencionar que o uso de I.A foi como auxiliar, por exemplo: Formatar o readme (minha stack é Python, C e C++. Não MarkDown), dar algumas luzes quanto a erros que eu não estava achando a solução em documentação ALGUMA (ex: Por que o SAS Token não tá batendo de jeito nenhum? Por que tá dando tal mensagem de erro? Coisas auxiliares) e no código da leitura do sensor de oxigênio dissolvido. Achei muito complicada a documentação dele, admito. No mais, o projeto todo foi feito com uma aba da documentação de cada sensor, de cada recurso do azure, do esp e etc. Espero que alguém que se interesse por isso tudo ache legal (e me fale se alguma coisa tá errada.)
