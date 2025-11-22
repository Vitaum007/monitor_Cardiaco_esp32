🫀 *Monitor Cardíaco Remoto — Documentação de Hardware*

Este documento descreve todos os componentes físicos utilizados no protótipo, sua função, ligações elétricas e a justificativa de uso.
O objetivo é permitir *total reprodutibilidade* do projeto, conforme exigido na disciplina.

---

# 🟦 1. *Plataforma de Desenvolvimento*

## *ESP32 DevKit V1*

O ESP32 foi escolhido por ser uma plataforma:

* compatível com Wi-Fi nativo
* com ADCs de 12 bits para leitura analógica precisa
* com suporte à IDE Arduino
* disponível gratuitamente no simulador Wokwi
* ideal para aplicações IoT com MQTT

*Parâmetros principais usados no projeto:*

| Recurso     | Uso                                    |
| ----------- | -------------------------------------- |
| GPIO34      | Entrada analógica do “sensor cardíaco” |
| GPIO23      | Saída digital para LED                 |
| GPIO26      | Saída para buzzer                      |
| Wi-Fi       | Conexão TCP/IP com broker MQTT         |
| Alimentação | 3.3V interna                           |

---

# 🟦 2. *Sensor Utilizado*

## *Potenciômetro (Simulação do AD8232)*

No ambiente físico seria usado o módulo AD8232 (sensor de ECG).
No Wokwi, como não existe esse módulo, usamos um *potenciômetro de 10k* para simular o sinal.

Ele é conectado assim:

| Terminal           | Ligação      |
| ------------------ | ------------ |
| GND                | GND do ESP32 |
| VCC                | 3.3V         |
| SIG (pino central) | GPIO34       |

Isso permite gerar valores analógicos entre *0 e 4095*, simulando variações do pulso cardíaco.

---

# 🟦 3. *Atuadores*

## *LED de Alerta (Vermelho)*

Usado para indicar:

* alerta local automático
* alerta remoto via MQTT

*Ligação:*

| LED                  | Conexão                    |
| -------------------- | -------------------------- |
| Anodo (perna longa)  | GPIO23 (via resistor 220Ω) |
| Catodo (perna curta) | GND                        |

*Resistor:*
Um resistor de *220Ω* é obrigatório para limitar a corrente.

---

## *Buzzer Ativo*

O buzzer é utilizado para emitir alerta sonoro e funciona tanto:

* automaticamente (quando o sensor passa do limite)
* quanto remotamente via MQTT

*Ligação:*

| Buzzer            | Conexão |
| ----------------- | ------- |
| Pino positivo (+) | GPIO26  |
| Pino negativo (–) | GND     |

---

# 🟦 4. *Alimentação*

O ESP32 é alimentado pelo próprio Wokwi ou pela USB.
Os periféricos são alimentados internamente:

* 3.3V para potenciômetro
* GPIOs como saída para LED e buzzer

Não foi necessária fonte externa.

---

# 🟦 5. *Esquema de Ligações*

### 🧩 Ligações completas:


ESP32
│
├── GPIO34  →  Sinal do Potenciômetro (AD8232 simulado)
│
├── GPIO23  →  Resistor 220Ω → Anodo LED
│                 Catodo LED → GND
│
├── GPIO26  →  Buzzer (+)
│                 Buzzer (–) → GND
│
└── 3.3V → VCC do Potenciômetro
           GND → GND do Potenciômetro


---

# 🟦 6. *Diagrama Visual (Wokwi)*

Insira aqui o print do seu circuito do Wokwi:


hardware/esquema_wokwi.png


---

# 🟦 7. *Lista Completa de Componentes*

| Quantidade             | Componente        | Função                   |
| ---------------------- | ----------------- | ------------------------ |
| 1                      | ESP32 DevKit V1   | Microcontrolador + Wi-Fi |
| 1                      | Potenciômetro 10k | Simular sensor AD8232    |
| 1                      | LED vermelho      | Alerta visual            |
| 1                      | Resistor 220Ω     | Limitar corrente do LED  |
| 1                      | Buzzer ativo      | Alerta sonoro            |
| Cabos virtuais (Wokwi) | Interligações     |                          |

---

# 🟦 8. *Justificativa de Escolha dos Componentes*

* O *ESP32* já possui Wi-Fi, ideal para MQTT.
* O *potenciômetro* permite testar o comportamento do ADC como no módulo AD8232.
* O *LED* e o *buzzer* são atuadores simples que demonstram o alerta com clareza.
* Tudo é *suportado no Wokwi*, facilitando testes e replicação.