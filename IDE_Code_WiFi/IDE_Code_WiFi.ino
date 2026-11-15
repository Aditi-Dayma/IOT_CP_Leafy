/*
 * Leafy — Intelligent Plant Care & Automated Microclimate Control
 * ESP32 Hardware Firmware (Wi-Fi + MQTT Wireless IoT Edition)
 *
 * Hardware Configuration:
 *  - Soil Moisture Sensor: GPIO 34
 *  - 5V Relay Pump Actuator: GPIO 25 (Active-LOW)
 *
 * MQTT Topics:
 *  - Telemetry Out: leafy/telemetry
 *  - Pump Control In: leafy/pump/control
 *  - Threshold Config In: leafy/threshold/set
 */

#include <WiFi.h>
#include <PubSubClient.h>

// Wi-Fi Credentials (replace with your local network)
const char* WIFI_SSID = "YOUR_WIFI_SSID";
const char* WIFI_PASS = "YOUR_WIFI_PASSWORD";

// Public MQTT Broker (Works directly with WebSockets in web dashboard)
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "leafy_esp32_greenhouse_01";

// Topics
const char* TOPIC_TELEMETRY = "leafy/telemetry";
const char* TOPIC_PUMP_CMD = "leafy/pump/control";
const char* TOPIC_THRESHOLD = "leafy/threshold/set";

// Hardware Pins
const int SOIL_SENSOR_PIN = 34;
const int RELAY_PIN = 25;

const int DRY_VALUE = 4095;
const int WET_VALUE = 1800;
int moistureThreshold = 35;

const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

bool pumpActive = false;
bool manualOverride = false;
unsigned long manualOverrideUntil = 0;
unsigned long lastPublish = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setPump(bool state) {
  pumpActive = state;
  digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
  
  // Publish pump state event
  char buf[64];
  snprintf(buf, sizeof(buf), "{\"pump\":%d,\"reason\":\"%s\"}", state ? 1 : 0, manualOverride ? "manual" : "auto");
  client.publish("leafy/pump/status", buf);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  message.trim();

  Serial.print("MQTT Received [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  if (String(topic) == TOPIC_PUMP_CMD) {
    if (message == "ON" || message == "1") {
      manualOverride = true;
      manualOverrideUntil = millis() + 5000;
      setPump(true);
    } else if (message == "OFF" || message == "0") {
      manualOverride = false;
      setPump(false);
    }
  } else if (String(topic) == TOPIC_THRESHOLD) {
    int val = message.toInt();
    if (val >= 10 && val <= 90) {
      moistureThreshold = val;
      Serial.print("New threshold: ");
      Serial.println(moistureThreshold);
    }
  }
}

void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print(MQTT_BROKER);
    Serial.print("...");
    if (client.connect(MQTT_CLIENT_ID)) {
      Serial.println(" Connected!");
      client.subscribe(TOPIC_PUMP_CMD);
      client.subscribe(TOPIC_THRESHOLD);
    } else {
      Serial.print(" Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);

  Serial.begin(115200);
  delay(500);

  Serial.println("\nConnecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Connected! IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT();
  }
  client.loop();

  if (manualOverride && millis() >= manualOverrideUntil) {
    manualOverride = false;
    setPump(false);
  }

  unsigned long now = millis();
  if (now - lastPublish > 2000) {
    lastPublish = now;

    int rawMoisture = analogRead(SOIL_SENSOR_PIN);
    int moisturePercent = map(rawMoisture, DRY_VALUE, WET_VALUE, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    if (!manualOverride) {
      if (moisturePercent < moistureThreshold) {
        if (!pumpActive) setPump(true);
      } else {
        if (pumpActive) setPump(false);
      }
    }

    char payload[128];
    snprintf(payload, sizeof(payload), 
      "{\"raw\":%d,\"moisture\":%d,\"pump\":%d,\"threshold\":%d,\"uptime\":%lu}",
      rawMoisture, moisturePercent, pumpActive ? 1 : 0, moistureThreshold, millis() / 1000
    );

    client.publish(TOPIC_TELEMETRY, payload);
    Serial.println(payload);
  }
}
