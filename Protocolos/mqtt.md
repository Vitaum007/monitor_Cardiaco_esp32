Arquitetura e Funcionamento da Comunicação MQTT

Este documento descreve toda a lógica e o fluxo de comunicação MQTT implementada no projeto Monitor Cardíaco Remoto com ESP32, apresentando tópicos utilizados, formato das mensagens, bibliotecas empregadas, fluxo bidirecional e integração com o simulador Wokwi e o broker HiveMQ.

 1. Visão Geral da Comunicação MQTT

O sistema utiliza o protocolo MQTT (Message Queuing Telemetry Transport) para troca de mensagens leves entre o ESP32 e um servidor em nuvem (broker).

O ESP32:

publica dados do sensor → nuvem

assina comandos remotos → atuadores

A comunicação é bidirecional, permitindo monitoramento contínuo e ação remota instantânea.

O broker escolhido foi:

broker.hivemq.com — Porta 1883 (MQTT TCP)


No navegador (para testes via WebSocket), o cliente HiveMQ usa:

wss://broker.hivemq.com:8884/mqtt

 2. Bibliotecas Utilizadas

O firmware utiliza:

#include <WiFi.h>          // conexão Wi-Fi
#include <PubSubClient.h>  // implementação MQTT


Essas bibliotecas permitem:

Conectar o ESP32 ao Wi-Fi

Abrir sessão MQTT com o broker

Publicar mensagens

Receber comandos

 3. Tópicos Utilizados (Publish / Subscribe)

O sistema utiliza dois tópicos MQTT, um para envio de dados e outro para controle remoto:

Tópico	Direção	Função
oic/ecg	ESP32 → Broker	Publicação dos dados do sensor cardíaco
oic/alarme	Broker → ESP32	Receber comandos para ligar/desligar LED e buzzer
 4. Publicação dos Dados (oic/ecg)

A cada 1 segundo, o ESP32 envia ao broker um JSON contendo:

valor bruto lido do sensor (0–4095)

tensão equivalente (0–3,3 V)

estado do alerta automático

Exemplo real de mensagem:
{
  "ecg_raw": 3100,
  "ecg_volts": 2.50,
  "alerta_local": true
}


Isso permite monitoramento contínuo e registro de anomalias cardíacas.

 5. Assinatura de Comandos (oic/alarme)

O ESP32 fica inscrito neste tópico:

mqtt.subscribe("oic/alarme");


Ao receber comandos, o callback trata os valores:

 Comando “1”

Ativa atuadores:

LED acende

Buzzer toca em 2 kHz

 Comando “0”

Desativa atuadores:

LED apaga

Buzzer silencia

O controle remoto é imediato, demonstrando o uso correto da arquitetura MQTT.

 6. Fluxo Completo da Comunicação (Bidirecional)
[SENSOR] → leitura analógica GPIO34
      ↓
[ESP32] → processa → detecta alerta local
      ↓
JSON publicado a cada 1s
      ↓
[BROKER MQTT HiveMQ]
      ↓
[CLIENTE WEB] → HiveMQ Websocket Client exibe dados
      ↓
Comando enviado (1 ou 0)
      ↓
[BROKER MQTT]
      ↓
[ESP32] → callback → LED/Buzzer

 7. Conexão ao Broker (Wi-Fi + MQTT)
Conexão Wi-Fi:
WiFi.begin("Wokwi-GUEST", "");

Conexão MQTT:
mqtt.setServer("broker.hivemq.com", 1883);
mqtt.connect(clientId);

Reconexão automática:
if (!mqtt.connected()) {
    conectaMQTT();
}
mqtt.loop();


Isso garante que o sistema continue funcionando mesmo se houver queda de rede.

 8. Intervalo de Publicação (QoS 0)

O envio dos dados ocorre a cada:

const unsigned long intervaloPub = 1000; // 1s


MQTT QoS 0 foi escolhido por ser:

leve

rápido

ideal para monitoramento contínuo

 9. Tempo de Resposta Medido
Fluxo	Média Real
Sensor → MQTT	5,0 s
MQTT → Atuador	2,5 s

Os resultados foram compatíveis com:

taxas de publicação (1 segundo)

latência do Wokwi

latência do HiveMQ

processamento do ESP32