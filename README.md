# Projeto-IoT

Monitoramento de temperatura e umidade para o cultivo de **soja**, com ESP32, sensor DHT22, MQTT e Node-RED. O sistema avisa o agricultor por **WhatsApp** quando as condições saem da faixa ideal. Projeto N2 da disciplina de IoT.

## Integrantes

- Giovanna Micher Santana
- Lucas Gargalhone

## Visão geral

Dois dispositivos ESP32, cada um com um sensor **DHT22**, leem as condições do local e publicam os valores em um broker MQTT por WiFi. O Node-RED assina os tópicos, aplica as regras de negócio, envia alertas pelo WhatsApp e grava os dados em um banco em nuvem (InfluxDB), que alimenta os dashboards.

```
ESP32 + DHT22 (temperatura) ─┐
                             ├─► Broker MQTT ─► Node-RED ─┬─► Alerta no WhatsApp
ESP32 + DHT22 (umidade)    ──┘   (HiveMQ)                 └─► InfluxDB ─► Dashboards
```

## Regras de negócio

Valores de referência baseados em recomendações da Embrapa para a cultura da soja.

| Condição | Mensagem enviada |
|---|---|
| Temperatura > 30 | "Temperatura alta! (temp) Isso pode prejudicar a floração da soja" |
| Temperatura < 20 | "Temperatura baixa! (temp) Isso pode inibir o crescimento adequado da soja" |
| Umidade > 18% | "Umidade alta! Pode causar danos à soja" |
| Umidade < 13% | "Umidade baixa! Isso pode causar perda de grãos" |
| 20 < temp < 30 e 13% < umidade < 18% | "Condições ideais de crescimento da soja!!" |

Assim o cultivador pode tomar as devidas precauções, como regar a lavoura quando a umidade está baixa.

## Requisitos do projeto atendidos

- [x] Dois dispositivos com sensores DHT22
- [x] Dois ou mais sensores publicando no broker MQTT para alimentar as regras do Node-RED
- [x] Pelo menos uma API conectada ao Node-RED (WhatsApp)
- [x] Envio dos dados para um banco em nuvem (InfluxDB)
- [x] Dicionário de dados (seção abaixo)
- [x] Dashboards com pelo menos dois painéis cada

## Hardware e tecnologias

- ESP32
- Sensor DHT22 (pino de dados no GPIO 15)
- Protocolo MQTT, broker público `broker.hivemq.com`, porta 1883
- Node-RED
- InfluxDB (nuvem)
- API de mensagens do WhatsApp
- Bibliotecas Arduino: [`DHTesp`](https://github.com/beegee-tokyo/DHTesp) e [`EspMQTTClient`](https://github.com/plapointe6/EspMQTTClient)
- Simulação no [Wokwi](https://wokwi.com/) (rede `Wokwi-GUEST`)

## Tópicos MQTT

O prefixo dos tópicos é `TESTMACK1165744`.

| Tópico | Conteúdo | Exemplo |
|---|---|---|
| `TESTMACK1165744/Temperatura` | Temperatura em °C, com 2 casas decimais | `24.50°C` |
| `TESTMACK1165744/Umidade` | Umidade relativa em %, com 1 casa decimal | `15.2%` |

Os valores são publicados como texto, com a unidade no final. O Node-RED precisa remover a unidade antes de comparar com os limites.

## Dicionário de dados

<!-- Ajuste os nomes conforme o measurement e os campos reais no InfluxDB. -->

| Campo | Tipo | Unidade | Descrição |
|---|---|---|---|
| `temperatura` | float | °C | Temperatura lida pelo DHT22 |
| `umidade` | float | % | Umidade relativa lida pelo DHT22 |
| `time` | timestamp | — | Instante da leitura, gerado pelo InfluxDB |

## Como funciona o código do dispositivo

O código do ESP32 fica em [`dispositivo/dispositivo.ino`](dispositivo/dispositivo.ino).

1. Conecta à rede WiFi e ao broker MQTT (`EspMQTTClient`).
2. Lê temperatura e umidade do DHT22 (`DHTesp`).
3. Publica os valores nos tópicos MQTT.
4. Mostra no monitor serial o estado da conexão WiFi e MQTT.
5. Repete o ciclo a cada 5 segundos.

Para rodar, ajuste no início do código:

```cpp
char SSIDName[] = "Wokwi-GUEST";        // nome da rede WiFi
char SSIDPass[] = "";                   // senha da rede WiFi
char BrokerURL[] = "broker.hivemq.com"; // broker MQTT
String TopicoPrefixo = "TESTMACK1165744"; // prefixo dos tópicos
```

## Node-RED

O fluxo exportado fica em `<!-- caminho do arquivo, ex.: node-red/flow.json -->`. Para usar:

1. Abra o Node-RED e vá em **Menu → Import**.
2. Selecione o arquivo JSON do fluxo.
3. Configure o broker MQTT, as credenciais da API do WhatsApp e a conexão com o InfluxDB.
4. Faça o deploy.

## Dashboards

<!-- Adicione prints dos dashboards, com pelo menos dois painéis cada. -->

## Estrutura do repositório

```
Projeto-IoT/
├── dispositivo/     # código-fonte dos ESP32
├── node-red/        # fluxo (JSON) do Node-RED
├── imagens/         # foto do protótipo e prints dos dashboards
└── README.md
```

## Demonstração

- Vídeo: <!-- link do vídeo de demonstração -->
- Foto do protótipo: <!-- imagem em imagens/ -->

## Referências

- Embrapa. [Exigências climáticas da soja: temperatura](https://www.embrapa.br/agencia-de-informacao-tecnologica/cultivos/soja/pre-producao/caracteristicas-da-especie-e-relacoes-com-o-ambiente/exigencias-climaticas/temperatura)
- Embrapa. [Comunicado Técnico 271](https://ainfo.cnptia.embrapa.br/digital/bitstream/item/79567/1/comunicado-271.pdf): colheita da soja com umidade dos grãos entre 13% e 15%; danos aumentam acima de 18% ou abaixo de 13%.
