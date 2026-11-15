/*
 * Leafy — Intelligent Plant Care & Automated Microclimate Control
 * ESP32 Hardware Firmware (3-Hour Sampling Cycle & Smart Weather-Aware Engine)
 *
 * Hardware Configuration:
 *  - Soil Moisture Sensor (Capacitive v1.2): Analog Pin GPIO 34
 *  - 5V Relay Channel 1 (Submersible Pump): Digital Pin GPIO 25 (Active-LOW)
 *  - Serial Baud Rate: 115200 bps
 *
 * Smart Irrigation Logic:
 *  1. 3-Hour Sampling Cycle: Samples soil moisture every 3 hours (or on-demand).
 *  2. Real-Time Threshold Check: If moisture < threshold (default 35%), checks rain lockout.
 *  3. Weather-Aware Rain Lockout: If rain is predicted in next 3 hours, holds pump OFF.
 *  4. Missed-Rain Auto-Recovery: If it does not rain, runs pump until soil moisture
 *     reaches half of the maximum seasonal moisture for the selected crop.
 *  5. Web Serial Interface: Bi-directional control with Leafy Vercel Dashboard.
 */

const int SOIL_SENSOR_PIN = 34; 
const int RELAY_PIN = 25;       

// Calibration readings (calibrated to your physical sensor)
const int DRY_VALUE = 4095;     // Reading in dry air
const int WET_VALUE = 1800;     // Reading in wet soil

// Pump thresholds & targets (dynamic from dashboard)
int moistureThreshold = 35;     // Default threshold: 35%
int targetRecoveryMoisture = 35;// Half of crop seasonal max moisture (default: 35%)

// Standard Active-LOW relay logic (wire in NO terminal)
const int RELAY_ON = LOW;
const int RELAY_OFF = HIGH;

// Timing: 3 Hours sampling interval (default: 3 * 3600 * 1000 ms)
const unsigned long INTERVAL_3_HOURS_MS = 3UL * 3600UL * 1000UL; // 10,800,000 ms
unsigned long currentIntervalMs = INTERVAL_3_HOURS_MS;
unsigned long lastSampleTime = 0;

// State flags
bool pumpActive = false;
bool manualOverride = false;
unsigned long manualOverrideUntil = 0;

bool rainLockout = false;          // True when weather API predicts rain in next 3h
bool recoveringTargetMoist = false;// True when pumping until target recovery moisture

void setRelay(bool state, const char* reason) {
  pumpActive = state;
  digitalWrite(RELAY_PIN, state ? RELAY_ON : RELAY_OFF);
  
  Serial.print("Status: Pump is ");
  Serial.print(state ? "ON" : "OFF");
  Serial.print(" | Reason: ");
  Serial.println(reason);
}

int readSoilMoisturePercent() {
  int raw = analogRead(SOIL_SENSOR_PIN);
  int pct = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

void executeIrrigationEvaluation(const char* triggerSource) {
  int rawMoisture = analogRead(SOIL_SENSOR_PIN);
  int moisturePercent = map(rawMoisture, DRY_VALUE, WET_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.println("\n-----------------------------------------");
  Serial.print(">> [3H Agro Sample Triggered via ");
  Serial.print(triggerSource);
  Serial.println("]");
  Serial.print("Raw Value: ");
  Serial.print(rawMoisture);
  Serial.print(" | Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");
  Serial.print("Current Threshold: ");
  Serial.print(moistureThreshold);
  Serial.print("% | Rain Lockout: ");
  Serial.println(rainLockout ? "ACTIVE (Rain in next 3h)" : "INACTIVE");

  // Core Decision Engine
  if (moisturePercent < moistureThreshold) {
    if (rainLockout) {
      // Rule: Rain predicted in next 3 hours -> Keep pump OFF
      setRelay(false, "Rain Lockout: Rain expected in next 3 hours (Pump Held OFF)");
      Serial.println("Decision: Soil is DRY, BUT rain predicted in next 3h -> Pump kept OFF to conserve water");
    } else {
      // Rule: Soil is dry & no rain predicted -> Turn pump ON
      setRelay(true, "Autonomous Soil Deficit Trigger (< threshold)");
      Serial.println("Decision: Soil is DRY & No Rain Predicted -> Pump ON");
    }
  } else {
    setRelay(false, "Soil moisture nominal (>= threshold)");
    Serial.println("Decision: Soil is WET / Nominal -> Pump OFF");
  }

  // Structured JSON packet for Vercel Dashboard
  Serial.print("JSON:{\"raw\":");
  Serial.print(rawMoisture);
  Serial.print(",\"moisture\":");
  Serial.print(moisturePercent);
  Serial.print(",\"pump\":");
  Serial.print(pumpActive ? 1 : 0);
  Serial.print(",\"threshold\":");
  Serial.print(moistureThreshold);
  Serial.print(",\"rainLockout\":");
  Serial.print(rainLockout ? 1 : 0);
  Serial.print(",\"interval\":");
  Serial.print(currentIntervalMs / 1000);
  Serial.println("}");
  Serial.println("-----------------------------------------");
}

void setup() {
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF);
  
  Serial.begin(115200);
  delay(500);
  
  Serial.println("==================================================");
  Serial.println("🌿 Leafy ESP32 Smart Agro-Meteorological Node");
  Serial.println("Cycle: 3 Hours Sampling | Soil: Pin 34 | Relay: Pin 25");
  Serial.println("Weather-Aware Rain Lockout & Recovery Engine Active");
  Serial.println("==================================================");

  // Initial power-up sample
  lastSampleTime = millis();
  executeIrrigationEvaluation("Power-up Initialization");
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  // 1. On-Demand Fetch
  if (cmd.equalsIgnoreCase("FETCH_NOW") || cmd.equalsIgnoreCase("SAMPLE_NOW")) {
    lastSampleTime = millis();
    executeIrrigationEvaluation("Dashboard Demand Request");
  }
  // 2. Weather Rain Lockout controls
  else if (cmd.equalsIgnoreCase("RAIN_LOCKOUT:1") || cmd.equalsIgnoreCase("RAIN:1")) {
    rainLockout = true;
    if (pumpActive && !manualOverride) {
      setRelay(false, "Rain Lockout Engaged by Weather Forecast");
    }
    Serial.println("ACK:RAIN_LOCKOUT_ENGAGED");
  }
  else if (cmd.equalsIgnoreCase("RAIN_LOCKOUT:0") || cmd.equalsIgnoreCase("RAIN:0")) {
    rainLockout = false;
    Serial.println("ACK:RAIN_LOCKOUT_DISENGAGED");
  }
  // 3. Missed-Rain Recovery: Pump until target moisture (half of max seasonal moisture)
  else if (cmd.startsWith("PUMP_TO_TARGET:") || cmd.startsWith("TARGET_MOIST:")) {
    int colonIdx = cmd.indexOf(':');
    targetRecoveryMoisture = cmd.substring(colonIdx + 1).toInt();
    if (targetRecoveryMoisture < 15) targetRecoveryMoisture = 35;
    
    recoveringTargetMoist = true;
    rainLockout = false; // Override rain lockout since rain didn't occur
    setRelay(true, "Missed Rain Auto-Recovery to 50% Crop Max Moisture");
    Serial.print("ACK:PUMPING_TO_SEASONAL_TARGET:");
    Serial.print(targetRecoveryMoisture);
    Serial.println("%");
  }
  // 4. Manual pump triggers
  else if (cmd.equalsIgnoreCase("PUMP_ON") || cmd.equalsIgnoreCase("PUMP:ON") || cmd.equalsIgnoreCase("WATER")) {
    manualOverride = true;
    manualOverrideUntil = millis() + 5000; // 5-second pulse
    setRelay(true, "Manual Dashboard Override (5s)");
    Serial.println("ACK:MANUAL_PUMP_ON_5S");
  }
  else if (cmd.equalsIgnoreCase("PUMP_OFF") || cmd.equalsIgnoreCase("PUMP:OFF") || cmd.equalsIgnoreCase("STOP")) {
    manualOverride = false;
    recoveringTargetMoist = false;
    setRelay(false, "Manual Stop Override");
    Serial.println("ACK:MANUAL_PUMP_OFF");
  }
  // 5. Configurable Threshold
  else if (cmd.startsWith("THRESHOLD:")) {
    int val = cmd.substring(10).toInt();
    if (val >= 10 && val <= 90) {
      moistureThreshold = val;
      Serial.print("ACK:THRESHOLD_SET:");
      Serial.println(moistureThreshold);
    }
  }
  // 6. Configurable Interval (for Lab Demo / 3 Hours)
  else if (cmd.startsWith("INTERVAL:")) {
    long sec = cmd.substring(9).toInt();
    if (sec > 0) {
      currentIntervalMs = (unsigned long)sec * 1000UL;
      Serial.print("ACK:INTERVAL_SET_SECONDS:");
      Serial.println(sec);
    }
  }
  else if (cmd.equalsIgnoreCase("STATUS") || cmd.equalsIgnoreCase("PING")) {
    int curMoist = readSoilMoisturePercent();
    Serial.print("PONG:LEAFY_OK|Moisture:");
    Serial.print(curMoist);
    Serial.print("%|Pump:");
    Serial.print(pumpActive ? "ON" : "OFF");
    Serial.print("|RainLockout:");
    Serial.print(rainLockout ? "1" : "0");
    Serial.print("|NextSampleIn:");
    unsigned long elapsed = millis() - lastSampleTime;
    long remaining = (currentIntervalMs > elapsed) ? (currentIntervalMs - elapsed) / 1000 : 0;
    Serial.print(remaining);
    Serial.println("s");
  }
}

void loop() {
  // Check for incoming serial commands from Dashboard
  while (Serial.available() > 0) {
    String cmd = Serial.readStringUntil('\n');
    processCommand(cmd);
  }

  // Handle manual 5-second pulse timeout
  if (manualOverride && millis() >= manualOverrideUntil) {
    manualOverride = false;
    setRelay(false, "Manual Pulse Completed");
  }

  // Handle Missed-Rain Recovery: Monitor moisture until target is reached
  if (recoveringTargetMoist) {
    static unsigned long lastTargetCheck = 0;
    if (millis() - lastTargetCheck >= 1000) {
      lastTargetCheck = millis();
      int currentMoisture = readSoilMoisturePercent();
      Serial.print(">> [Recovery Pumping] Current: ");
      Serial.print(currentMoisture);
      Serial.print("% | Target (50% Max): ");
      Serial.print(targetRecoveryMoisture);
      Serial.println("%");

      if (currentMoisture >= targetRecoveryMoisture) {
        recoveringTargetMoist = false;
        setRelay(false, "Target Seasonal Moisture Reached (Pump OFF)");
        Serial.println(">> Target Reached! Pump Auto-Shutoff Engaged.");
      }
    }
  }

  // 3-Hour Periodic Evaluation Cycle
  unsigned long currentMillis = millis();
  if (currentMillis - lastSampleTime >= currentIntervalMs) {
    lastSampleTime = currentMillis;
    executeIrrigationEvaluation("3-Hour Scheduled Timer");
  }
}
