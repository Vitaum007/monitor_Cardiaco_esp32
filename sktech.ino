#include <WiFi.h>
#include <PubSubClient.h>

// ------------------------------
// Definição de pinos
// ------------------------------
#define ECG_PIN     34   // Potenciômetro simulando saída analógica do AD8232
#define LED_PIN     23   // LED de alerta 
#define BUZZER_PIN  26   // Buzzer de alerta

// ------------------------------
// Wi-Fi (Wokwi)
// ------------------------------
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

// ------------------------------
// MQTT (HiveMQ)
// ------------------------------
const char* mqtt_server   = "broker.hivemq.com";
const uint16_t mqtt_port  = 1883;

// Tópicos
const char* topic_ecg     = "oic/ecg";      // Publica dados do "AD8232"
const char* topic_alarme  = "oic/alarme";   // Recebe comandos para LED/Buzzer

WiFiClient    wifiClient;
PubSubClient  mqtt(wifiClient);

// ------------------------------
// Funções de conexão
// ------------------------------
void conectaWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi conectado. IP: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensagem recebida no topico [");
  Serial.print(topic);
  Serial.print("]: ");

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  // Comandos: "1" liga alerta, "0" desliga (controle remoto)
  if (String(topic) == topic_alarme) {
    if (msg == "1" || msg == "ON" || msg == "on") {
      digitalWrite(LED_PIN, HIGH);
      tone(BUZZER_PIN, 2000);  // 2kHz
      Serial.println("Alerta ATIVADO via MQTT");
    } else {
      digitalWrite(LED_PIN, LOW);
      noTone(BUZZER_PIN);
      Serial.println("Alerta DESATIVADO via MQTT");
    }
  }
}

void conectaMQTT() {
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(mqttCallback);

  while (!mqtt.connected()) {
    Serial.print("Conectando ao broker MQTT... ");
    String clientId = "esp32-ecg-";
    clientId += String(random(0xffff), HEX);

    if (mqtt.connect(clientId.c_str())) {
      Serial.println("conectado!");
      // Assina o tópico de comandos
      mqtt.subscribe(topic_alarme);
      Serial.print("Inscrito em ");
      Serial.println(topic_alarme);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqtt.state());
      Serial.println(" - tentando novamente em 2s");
      delay(2000);
    }
  }
}

// ------------------------------
// Setup
// ------------------------------
void setup() {
  Serial.begin(115200);
  delay(2000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  noTone(BUZZER_PIN);

  conectaWiFi();
  conectaMQTT();
}

// ------------------------------
// Loop principal
// ------------------------------
unsigned long ultimaPub = 0;
const unsigned long intervaloPub = 1000; // 1s entre mensagens

void loop() {
  if (!mqtt.connected()) {
    conectaMQTT();
  }
  mqtt.loop();

  unsigned long agora = millis();
  if (agora - ultimaPub >= intervaloPub) {
    ultimaPub = agora;

    // Leitura do "ECG" (AD8232 simulado)
    int raw = analogRead(ECG_PIN);  
    float volts = raw * (3.3 / 4095.0);

    // Regra simples de alerta local (limiar ajustável)
    bool alertaLocal = (raw > 2500);

    // LED automático (alerta local)
    digitalWrite(LED_PIN, alertaLocal ? HIGH : LOW);

    // Buzzer automático (alerta local)
    if (alertaLocal) {
      tone(BUZZER_PIN, 2000);   // toca 2 kHz
    } else {
      noTone(BUZZER_PIN);       // desliga
    }

    // Monta JSON para enviar ao broker
    String payload = String("{\"ecg_raw\":") + raw +
                     ",\"ecg_volts\":" + String(volts, 3) +
                     ",\"alerta_local\":" + (alertaLocal ? "true" : "false") +
                     "}";

    // Publica no broker
    mqtt.publish(topic_ecg, payload.c_str());
    Serial.print("Publicado em ");
    Serial.print(topic_ecg);
    Serial.print(": ");
    Serial.println(payload);
  }
}
