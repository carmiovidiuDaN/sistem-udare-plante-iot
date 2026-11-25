#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

// Configurare Wi-Fi (Wokwi)
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Configurare Broker MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* device_id = "ESP32_OVIDIU_01";

// Topicuri MQTT (personalizati prefixul pentru a evita coliziunile)
const char* topic_senzori = "irigare/OVIDIU/senzori";
const char* topic_comanda = "irigare/OVIDIU/comanda";
const char* topic_status  = "irigare/OVIDIU/status";

// Pini hardware
#define PIN_SOIL   34
#define PIN_DHT    15
#define PIN_RELAY  26
#define PIN_LED    27
#define DHTTYPE    DHT22

DHT dht(PIN_DHT, DHTTYPE);
WiFiClient espClient;
PubSubClient client(espClient);

// Variabile de stare
bool modAutomat = true;
bool pompaActiva = false;
int pragUmiditate = 35;       // Porneste udarea sub 35%
const int histerezis = 15;    // Opreste udarea peste 50%
unsigned long ultimaPublicare = 0;
const long intervalPublicare = 5000;

void seteazaPompa(bool stare) {
  pompaActiva = stare;
  digitalWrite(PIN_RELAY, stare ? HIGH : LOW);
  digitalWrite(PIN_LED, stare ? HIGH : LOW);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String mesaj = "";
  for (unsigned int i = 0; i < length; i++) {
    mesaj += (char)payload[i];
  }
  mesaj.trim();
  Serial.println("[MQTT] Comanda primita: " + mesaj);

  if (mesaj == "ON") {
    modAutomat = false;
    seteazaPompa(true);
  } else if (mesaj == "OFF") {
    modAutomat = false;
    seteazaPompa(false);
  } else if (mesaj == "AUTO") {
    modAutomat = true;
  } else if (mesaj.startsWith("PRAG:")) {
    int nouPrag = mesaj.substring(5).toInt();
    if (nouPrag >= 10 && nouPrag <= 80) {
      pragUmiditate = nouPrag;
      Serial.println("[CONFIG] Prag nou setat la: " + String(pragUmiditate) + "%");
    }
  }
}

void connectWiFi() {
  Serial.print("Conectare la Wi-Fi");
  WiFi.begin(ssid, password, 6);
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectat! IP: " + WiFi.localIP().toString());
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Conectare la Broker MQTT...");
    if (client.connect(device_id, topic_status, 1, true, "OFFLINE")) {
      Serial.println(" Conectat!");
      client.publish(topic_status, "ONLINE", true);
      client.subscribe(topic_comanda);
    } else {
      Serial.print(" Eroare rc=");
      Serial.print(client.state());
      Serial.println(". Reincercare in 3 secunde...");
      delay(3000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  seteazaPompa(false);

  dht.begin();
  connectWiFi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  unsigned long acum = millis();
  if (acum - ultimaPublicare >= intervalPublicare) {
    ultimaPublicare = acum;

    // Citire senzori
    int rawSoil = analogRead(PIN_SOIL);
    int umiditateSol = map(rawSoil, 0, 4095, 0, 100);
    float temperatura = dht.readTemperature();
    float umiditateAer = dht.readHumidity();

    // Control automat cu histerezis
    if (modAutomat) {
      if (!pompaActiva && umiditateSol < pragUmiditate) {
        seteazaPompa(true);
      } else if (pompaActiva && umiditateSol >= (pragUmiditate + histerezis)) {
        seteazaPompa(false);
      }
    }

    // Construire payload JSON
    StaticJsonDocument<256> doc;
    doc["id_dispozitiv"] = device_id;
    doc["umiditate_sol"] = umiditateSol;
    doc["temperatura"] = isnan(temperatura) ? 0.0 : temperatura;
    doc["umiditate_aer"] = isnan(umiditateAer) ? 0.0 : umiditateAer;
    doc["pompa"] = pompaActiva ? "ON" : "OFF";
    doc["mod"] = modAutomat ? "AUTO" : "MANUAL";
    doc["prag"] = pragUmiditate;

    char buffer[256];
    serializeJson(doc, buffer);
    client.publish(topic_senzori, buffer);

    Serial.println("[MQTT] Publicat: " + String(buffer));
  }
}
