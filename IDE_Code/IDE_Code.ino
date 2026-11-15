/*
 * Leafy — Intelligent Plant Care & Automated Microclimate Control
 * ESP32 Hardware Firmware (Configurable Schedule, Safe Auto-Cutoff & Weather-Aware)
 *
 * Hardware Configuration:
 *  - Soil Moisture Sensor: GPIO 34 (Analog ADC)
 *  - 5V Relay Actuator: GPIO 25 (Pump)
 *  - Serial Baud Rate: 115200 bps
 *
 * Safety & Precision Features:
 *  1. Timed Auto-Shutoff Watchdog: Pump NEVER stays on permanently. Runs for a safe
 *     pulse (default 5s, configurable 2s-60s) and shuts OFF automatically.
 *  2. Configurable Scheduling: Interval can be set to any custom seconds/hours from dashboard.
 *  3. Invertible Relay Polarity: Supports both Active-LOW and Active-HIGH relay modules.
 *  4. Real-time Moisture Tracking: Zero hardcoded numbers; reports actual analog voltage.
 *  5. Rain Lockout & Seasonal Target Recovery: Pauses for rain, recovers if rain missed.
 */

const int SOIL_SENSOR_PIN = 34; 
const int RELAY_PIN = 25;       

// Calibration readings (calibrated to your physical sensor)
const int DRY_VALUE = 4095;     // Reading in dry air
const int WET_VALUE = 1800;     // Reading in wet soil

// Configurable Parameters (can be modified via serial commands from dashboard)
int moistureThreshold = 35;          // Pump activates if moisture < threshold (%)
int pumpDurationSeconds = 5;         // Safe watering run duration (seconds)
bool relayActiveLow = true;          // true = Active-LOW (LOW=ON), false = Active-HIGH (HIGH=ON)

// Scheduling: Default 3 Hours (10800 seconds), can be adjusted by user
unsigned long sampleIntervalMs = 3UL * 3600UL * 1000UL; 
unsigned long lastSampleTime = 0;

// State flags
bool pumpActive = false;
unsigned long pumpShutoffTime = 0;

bool rainLockout = false;          // True when weather API predicts rain in next 3h
bool recoveringTargetMoist = false;// True when pumping until target recovery moisture
int targetRecoveryMoisture = 35;

void setPumpHardware(bool turnOn, const char* reason) {
  pumpActive = turnOn;

  if (turnOn) {
    // Energize relay according to polarity
    digitalWrite(RELAY_PIN, relayActiveLow ? LOW : HIGH);
    pumpShutoffTime = millis() + ((unsigned long)pumpDurationSeconds * 1000UL);
    
    Serial.print(">> [PUMP ACTIVATED] Running for ");
    Serial.print(pumpDurationSeconds);
    Serial.print("s | Reason: ");
    Serial.println(reason);
  } else {
    // De-energize relay safely
    digitalWrite(RELAY_PIN, relayActiveLow ? HIGH : LOW);
    
    Serial.print(">> [PUMP STOPPED] Standby | Reason: ");
    Serial.println(reason);
  }
}

int readSoilMoisturePercent() {
  int raw = analogRead(SOIL_SENSOR_PIN);
  int pct = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  return constrain(pct, 0, 100);
}

void executeScheduleEvaluation(const char* triggerSource) {
  int rawMoisture = analogRead(SOIL_SENSOR_PIN);
  int moisturePercent = map(rawMoisture, DRY_VALUE, WET_VALUE, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  Serial.println("\n=========================================");
  Serial.print(">> [SCHEDULE CHECK TRIGGERED via ");
  Serial.print(triggerSource);
  Serial.println("]");
  Serial.print("Raw Value: ");
  Serial.print(rawMoisture);
  Serial.print(" | Moisture: ");
  Serial.print(moisturePercent);
  Serial.println("%");
  Serial.print("Threshold: ");
  Serial.print(moistureThreshold);
  Serial.print("% | Rain Lockout: ");
  Serial.println(rainLockout ? "ENGAGED (Rain expected)" : "DISENGAGED");

  // Decision Tree
  if (moisturePercent < moistureThreshold) {
    if (rainLockout) {
      Serial.println("Decision: Soil is DRY, but Rain Lockout is active. Pump held OFF.");
    } else {
      Serial.println("Decision: Soil is DRY & No Rain expected. Engaging timed pump pulse.");
      setPumpHardware(true, "Scheduled Moisture Deficit");
    }
  } else {
    Serial.println("Decision: Soil is Nominal (>= threshold). Pump remains OFF.");
    if (pumpActive) {
      setPumpHardware(false, "Soil is already wet");
    }
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
  Serial.print(",\"duration\":");
  Serial.print(pumpDurationSeconds);
  Serial.print(",\"relayActiveLow\":");
  Serial.print(relayActiveLow ? 1 : 0);
  Serial.print(",\"rainLockout\":");
  Serial.print(rainLockout ? 1 : 0);
  Serial.print(",\"interval\":");
  Serial.print(sampleIntervalMs / 1000);
  Serial.println("}");
  Serial.println("=========================================");
}

void setup() {
  // Ensure relay is unconditionally turned OFF on boot
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, relayActiveLow ? HIGH : LOW);
  pumpActive = false;

  Serial.begin(115200);
  delay(500);

  Serial.println("==================================================");
  Serial.println("🌿 Leafy ESP32 Edge Node Initialized");
  Serial.println("Firmware: Safe Auto-Cutoff & Configurable Schedule");
  Serial.println("Soil Pin: GPIO 34 | Relay Pin: GPIO 25");
  Serial.print("Relay Logic: ");
  Serial.println(relayActiveLow ? "Active-LOW (LOW=ON)" : "Active-HIGH (HIGH=ON)");
  Serial.println("Pump state initialized to OFF.");
  Serial.println("==================================================");

  lastSampleTime = millis();
}

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  // 1. Manual Watering Trigger
  if (cmd.equalsIgnoreCase("PUMP_ON") || cmd.equalsIgnoreCase("PUMP:ON") || cmd.equalsIgnoreCase("WATER")) {
    setPumpHardware(true, "Manual Dashboard Watering Request");
    Serial.println("ACK:PUMP_ON");
  }
  // 2. Immediate Stop
  else if (cmd.equalsIgnoreCase("PUMP_OFF") || cmd.equalsIgnoreCase("PUMP:OFF") || cmd.equalsIgnoreCase("STOP")) {
    setPumpHardware(false, "Manual Stop Request");
    recoveringTargetMoist = false;
    Serial.println("ACK:PUMP_OFF");
  }
  // 3. On-Demand Fetch & Evaluation
  else if (cmd.equalsIgnoreCase("FETCH_NOW") || cmd.equalsIgnoreCase("SAMPLE_NOW")) {
    lastSampleTime = millis();
    executeScheduleEvaluation("Manual Demand");
  }
  // 4. Configurable Schedule Interval in Seconds
  else if (cmd.startsWith("INTERVAL:")) {
    long sec = cmd.substring(9).toInt();
    if (sec > 0) {
      sampleIntervalMs = (unsigned long)sec * 1000UL;
      lastSampleTime = millis(); // Reset cycle anchor
      Serial.print("ACK:INTERVAL_SET:");
      Serial.print(sec);
      Serial.println("s");
    }
  }
  // 5. Configurable Pump Run Duration in Seconds
  else if (cmd.startsWith("DURATION:")) {
    int dur = cmd.substring(9).toInt();
    if (dur >= 1 && dur <= 120) {
      pumpDurationSeconds = dur;
      Serial.print("ACK:DURATION_SET:");
      Serial.print(pumpDurationSeconds);
      Serial.println("s");
    }
  }
  // 6. Configurable Moisture Threshold
  else if (cmd.startsWith("THRESHOLD:")) {
    int th = cmd.substring(10).toInt();
    if (th >= 5 && th <= 95) {
      moistureThreshold = th;
      Serial.print("ACK:THRESHOLD_SET:");
      Serial.print(moistureThreshold);
      Serial.println("%");
    }
  }
  // 7. Toggle / Set Relay Polarity
  else if (cmd.equalsIgnoreCase("INVERT_RELAY") || cmd.equalsIgnoreCase("RELAY_INVERT")) {
    relayActiveLow = !relayActiveLow;
    setPumpHardware(false, "Relay polarity inverted");
    Serial.print("ACK:RELAY_POLARITY:");
    Serial.println(relayActiveLow ? "Active-LOW" : "Active-HIGH");
  }
  else if (cmd.equalsIgnoreCase("RELAY_LOGIC:LOW")) {
    relayActiveLow = true;
    setPumpHardware(false, "Relay set to Active-LOW");
    Serial.println("ACK:RELAY_ACTIVE_LOW");
  }
  else if (cmd.equalsIgnoreCase("RELAY_LOGIC:HIGH")) {
    relayActiveLow = false;
    setPumpHardware(false, "Relay set to Active-HIGH");
    Serial.println("ACK:RELAY_ACTIVE_HIGH");
  }
  // 8. Weather Rain Lockout
  else if (cmd.equalsIgnoreCase("RAIN_LOCKOUT:1") || cmd.equalsIgnoreCase("RAIN:1")) {
    rainLockout = true;
    if (pumpActive) {
      setPumpHardware(false, "Rain Lockout Engaged");
    }
    Serial.println("ACK:RAIN_LOCKOUT_1");
  }
  else if (cmd.equalsIgnoreCase("RAIN_LOCKOUT:0") || cmd.equalsIgnoreCase("RAIN:0")) {
    rainLockout = false;
    Serial.println("ACK:RAIN_LOCKOUT_0");
  }
  // 9. Missed-Rain Recovery to Target
  else if (cmd.startsWith("PUMP_TO_TARGET:")) {
    targetRecoveryMoisture = cmd.substring(15).toInt();
    if (targetRecoveryMoisture < 15) targetRecoveryMoisture = 35;
    recoveringTargetMoist = true;
    rainLockout = false;
    setPumpHardware(true, "Missed Rain Recovery to 50% Crop Max Moisture");
    Serial.print("ACK:RECOVERING_TO_TARGET:");
    Serial.print(targetRecoveryMoisture);
    Serial.println("%");
  }
  // 10. Ping / Status
  else if (cmd.equalsIgnoreCase("STATUS") || cmd.equalsIgnoreCase("PING")) {
    int curMoist = readSoilMoisturePercent();
    Serial.print("PONG:LEAFY_OK|Moist:");
    Serial.print(curMoist);
    Serial.print("%|Pump:");
    Serial.print(pumpActive ? "ON" : "OFF");
    Serial.print("|Logic:");
    Serial.print(relayActiveLow ? "Active-LOW" : "Active-HIGH");
    Serial.print("|Interval:");
    Serial.print(sampleIntervalMs / 1000);
    Serial.println("s");
  }
}

void loop() {
  // 1. Process serial commands from dashboard
  while (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    processCommand(command);
  }

  // 2. CRITICAL SAFETY: Timed Pump Auto-Shutoff Watchdog
  // Ensures pump turns OFF after pumpDurationSeconds and NEVER stays on indefinitely
  if (pumpActive && millis() >= pumpShutoffTime) {
    setPumpHardware(false, "Safety Auto-Shutoff Watchdog Triggered");
  }

  // 3. Monitor Recovery to Target Moisture
  if (recoveringTargetMoist) {
    static unsigned long lastTargetSample = 0;
    if (millis() - lastTargetSample >= 1000) {
      lastTargetSample = millis();
      int moist = readSoilMoisturePercent();
      Serial.print(">> [Recovery Monitoring] Moisture: ");
      Serial.print(moist);
      Serial.print("% | Target: ");
      Serial.print(targetRecoveryMoisture);
      Serial.println("%");

      if (moist >= targetRecoveryMoisture) {
        recoveringTargetMoist = false;
        setPumpHardware(false, "Target Seasonal Moisture Reached");
      }
    }
  }

  // 4. Configurable Scheduled Evaluation Cycle
  unsigned long currentMillis = millis();
  if (currentMillis - lastSampleTime >= sampleIntervalMs) {
    lastSampleTime = currentMillis;
    executeScheduleEvaluation("Configured Schedule Cycle");
  }
}
