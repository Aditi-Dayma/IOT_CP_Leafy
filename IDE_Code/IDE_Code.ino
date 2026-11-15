/*
 * Leafy — Intelligent Plant Care & Automated Microclimate Control
 * ESP32 Hardware Firmware (USB Web Serial & Autonomous Microclimate Engine)
 *
 * Hardware Configuration:
 *  - Soil Moisture Sensor (Capacitive v1.2): Analog Pin GPIO 34
 *  - 5V Relay Channel 1 (Submersible Pump): Digital Pin GPIO 25 (Active-LOW)
 *  - Serial Baud Rate: 115200 bps
 *
 * Web Dashboard Integration:
 *  - Direct Browser Connection via Web Serial API (Chrome / Edge / Opera)
 *  - Compatible with Leafy Vercel Dashboard
 *  - Bidirectional remote commands: PUMP_ON, PUMP_OFF, THRESHOLD:<val>, SYNC
 */

const int SOIL_SENSOR_PIN = 34; 
const int RELAY_PIN = 25;       

// Calibrated sensor readings
const int DRY_VALUE = 4095;     // Reading in dry air
const int WET_VALUE = 1800;     // Reading in wet soil

// Pump threshold (dynamic, can be updated from dashboard)
int moistureThreshold = 35;     // Default: 35%

// Standard Active-LOW relay logic (wire in NO terminal)
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// State tracking
bool pumpActive = false;
bool manualOverride = false;
unsigned long manualOverrideUntil = 0;
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_INTERVAL = 2000; // Sample every 2 seconds

void setPump(bool state, const char* reason) {
  pumpActive = state;
  digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
  Serial.print("Status: Pump is ");
  Serial.print(state ? "ON" : "OFF");
  Serial.print(" (");
  Serial.print(reason);
  Serial.println(")");
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);
  
  Serial.begin(115200);
  delay(500);
  Serial.println("=========================================");
  Serial.println("🌿 Leafy ESP32 Edge Node Initialized");
  Serial.println("System Ready - Pump initialized to OFF");
  Serial.println("Baud Rate: 115200 | Pin 34: Soil | Pin 25: Relay");
  Serial.println("=========================================");
}

void processIncomingCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  if (cmd.equalsIgnoreCase("PUMP_ON") || cmd.equalsIgnoreCase("PUMP:ON") || cmd.equalsIgnoreCase("WATER")) {
    manualOverride = true;
    manualOverrideUntil = millis() + 5000; // Run pump for 5 seconds
    setPump(true, "Dashboard Manual Watering Override (5s)");
    Serial.println("ACK:PUMP_ON_5S");
  } 
  else if (cmd.equalsIgnoreCase("PUMP_OFF") || cmd.equalsIgnoreCase("PUMP:OFF") || cmd.equalsIgnoreCase("STOP")) {
    manualOverride = false;
    setPump(false, "Dashboard Manual Stop Override");
    Serial.println("ACK:PUMP_OFF");
  } 
  else if (cmd.startsWith("THRESHOLD:") || cmd.startsWith("threshold:")) {
    int newThresh = cmd.substring(10).toInt();
    if (newThresh >= 10 && newThresh <= 90) {
      moistureThreshold = newThresh;
      Serial.print("ACK:THRESHOLD_UPDATED:");
      Serial.println(moistureThreshold);
    }
  } 
  else if (cmd.equalsIgnoreCase("SYNC") || cmd.equalsIgnoreCase("PING")) {
    Serial.println("ACK:PONG:LEAFY_ESP32_OK");
  }
}

void loop() {
  // 1. Check for incoming Serial commands from Web Dashboard
  while (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processIncomingCommand(command);
  }

  // 2. Handle manual override timeout
  if (manualOverride && millis() >= manualOverrideUntil) {
    manualOverride = false;
    setPump(false, "Manual Override Cycle Completed");
  }

  // 3. Periodic sensor sampling & autonomous logic
  unsigned long currentMillis = millis();
  if (currentMillis - lastTelemetryTime >= TELEMETRY_INTERVAL) {
    lastTelemetryTime = currentMillis;

    int rawMoisture = analogRead(SOIL_SENSOR_PIN);
    int moisturePercent = map(rawMoisture, DRY_VALUE, WET_VALUE, 0, 100);
    moisturePercent = constrain(moisturePercent, 0, 100);

    // Standard human-readable log (matches original IDE_Code format)
    Serial.print("Raw Value: ");
    Serial.print(rawMoisture);
    Serial.print(" | Moisture: ");
    Serial.print(moisturePercent);
    Serial.println("%");

    // Autonomous Relay logic if not currently in manual override
    if (!manualOverride) {
      if (moisturePercent < moistureThreshold) {
        if (!pumpActive) {
          setPump(true, "Soil DRY < threshold");
        }
        Serial.print("Status: Soil is DRY (< ");
        Serial.print(moistureThreshold);
        Serial.println("%) -> Pump ON");
      } else {
        if (pumpActive) {
          setPump(false, "Soil WET >= threshold");
        }
        Serial.print("Status: Soil is WET (>= ");
        Serial.print(moistureThreshold);
        Serial.println("%) -> Pump OFF");
      }
    }

    // Structured JSON telemetry packet for browser parser
    Serial.print("JSON:{\"raw\":");
    Serial.print(rawMoisture);
    Serial.print(",\"moisture\":");
    Serial.print(moisturePercent);
    Serial.print(",\"pump\":");
    Serial.print(pumpActive ? 1 : 0);
    Serial.print(",\"threshold\":");
    Serial.print(moistureThreshold);
    Serial.println("}");
  }
}
