/*
 * MARS-SENTINEL Simplified Firmware
 * Only uses CONFIRMED working sensors + simulated data for missing ones
 * 
 * CONFIRMED HARDWARE:
 * - Gas Sensor: A1 (WORKING - shows 0-800+ ppm)
 * 
 * SIMULATED (not physically connected):
 * - Temperature/Humidity (realistic simulation)
 * - Distance (fixed at 400cm since ultrasonic not connected)
 * - IR sensor (simulated)
 */

// Pin definitions - ONLY for confirmed working sensors
#define GAS_PIN A1
#define LED_PIN 13  // Built-in LED for status

// Timing
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 200; // 200ms

// Simulation variables for missing sensors
float simTemp = 22.5;      // Start at room temperature
float simHumidity = 45.0;  // Start at comfortable humidity
unsigned long lastSimUpdate = 0;

// Thresholds
struct Thresholds {
  float tempHigh = 45.0;
  float tempWarn = 35.0;
  float humidHigh = 85.0;
  float humidWarn = 70.0;
  int gasHigh = 600;
  int gasWarn = 300;
  int distanceWarn = 50;
  int distanceDanger = 20;
  int irDanger = 1;
} thresholds;

// System state
enum SystemStatus { STATUS_OK, STATUS_WARN, STATUS_DANGER };
SystemStatus currentStatus = STATUS_OK;

void setup() {
  Serial.begin(9600);
  
  // Configure pins
  pinMode(GAS_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // Initial state
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("MARS-SENTINEL System Initialized (Simplified Mode)");
  Serial.println("Using: Real Gas Sensor + Simulated Temperature/Humidity");
  Serial.println("Format: timestamp,temp,humidity,gas,ir,distance");
  
  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read sensors at specified interval
  if (currentTime - lastSensorRead >= SENSOR_INTERVAL) {
    readAndProcessSensors();
    lastSensorRead = currentTime;
  }
  
  // Check for incoming serial commands
  if (Serial.available()) {
    processSerialCommand();
  }
}

void readAndProcessSensors() {
  // Read REAL gas sensor (confirmed working)
  int gasLevel = analogRead(GAS_PIN);
  
  // Generate realistic simulated temperature and humidity
  updateSimulatedEnvironment();
  
  // Fixed values for missing sensors
  int irDetection = 0;      // No IR sensor
  long distance = 400;      // No ultrasonic sensor
  
  // Determine system status based on real gas sensor
  SystemStatus newStatus = evaluateStatus(simTemp, simHumidity, gasLevel, irDetection, distance);
  
  // Update status LED
  updateStatusLED(newStatus);
  currentStatus = newStatus;
  
  // Send data via serial in CSV format (same as original)
  Serial.print(millis());
  Serial.print(",");
  Serial.print(simTemp, 2);
  Serial.print(",");
  Serial.print(simHumidity, 2);
  Serial.print(",");
  Serial.print(gasLevel);
  Serial.print(",");
  Serial.print(irDetection);
  Serial.print(",");
  Serial.println(distance);
}

void updateSimulatedEnvironment() {
  unsigned long currentTime = millis();
  
  // Update simulated values every 10 seconds with realistic variations
  if (currentTime - lastSimUpdate > 10000) {
    // Temperature: gentle variations around room temperature
    simTemp += (random(-100, 101) / 100.0); // ±1°C random walk
    simTemp = constrain(simTemp, 18.0, 28.0); // Keep in realistic range
    
    // Humidity: gentle variations around comfortable level
    simHumidity += (random(-200, 201) / 100.0); // ±2% random walk
    simHumidity = constrain(simHumidity, 35.0, 65.0); // Keep in comfortable range
    
    lastSimUpdate = currentTime;
  }
  
  // Add small real-time variations
  float tempVariation = sin(currentTime / 30000.0) * 0.5; // Slow sine wave ±0.5°C
  float humidVariation = cos(currentTime / 45000.0) * 2.0; // Slow cosine ±2%
  
  simTemp += tempVariation;
  simHumidity += humidVariation;
}

SystemStatus evaluateStatus(float temp, float humidity, int gas, int ir, long distance) {
  SystemStatus status = STATUS_OK;
  
  // Temperature checks
  if (temp > thresholds.tempHigh) {
    status = STATUS_DANGER;
  } else if (temp > thresholds.tempWarn && status != STATUS_DANGER) {
    status = STATUS_WARN;
  }
  
  // Humidity checks
  if (humidity > thresholds.humidHigh) {
    status = STATUS_DANGER;
  } else if (humidity > thresholds.humidWarn && status != STATUS_DANGER) {
    status = STATUS_WARN;
  }
  
  // Gas level checks (REAL SENSOR)
  if (gas > thresholds.gasHigh) {
    status = STATUS_DANGER;
  } else if (gas > thresholds.gasWarn && status != STATUS_DANGER) {
    status = STATUS_WARN;
  }
  
  return status;
}

void updateStatusLED(SystemStatus status) {
  switch (status) {
    case STATUS_OK:
      digitalWrite(LED_PIN, HIGH); // Solid on = OK
      break;
      
    case STATUS_WARN:
      // Slow blink for warning
      digitalWrite(LED_PIN, (millis() / 1000) % 2);
      break;
      
    case STATUS_DANGER:
      // Fast blink for danger
      digitalWrite(LED_PIN, (millis() / 250) % 2);
      break;
  }
}

void processSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command.startsWith("SET ")) {
    // Parse threshold update commands
    int spaceIndex = command.indexOf(' ', 4);
    if (spaceIndex > 0) {
      String parameter = command.substring(4, spaceIndex);
      float value = command.substring(spaceIndex + 1).toFloat();
      
      if (parameter == "TEMP_HIGH") {
        thresholds.tempHigh = value;
        Serial.println("OK: TEMP_HIGH updated to " + String(value));
      } else if (parameter == "GAS_HIGH") {
        thresholds.gasHigh = (int)value;
        Serial.println("OK: GAS_HIGH updated to " + String((int)value));
      } else if (parameter == "GAS_WARN") {
        thresholds.gasWarn = (int)value;
        Serial.println("OK: GAS_WARN updated to " + String((int)value));
      } else {
        Serial.println("ERROR: Unknown parameter " + parameter);
      }
    }
  } else if (command == "STATUS") {
    Serial.println("System Status: " + String(currentStatus == STATUS_OK ? "OK" : 
                                              currentStatus == STATUS_WARN ? "WARN" : "DANGER"));
    Serial.println("Real Gas Sensor: " + String(analogRead(GAS_PIN)) + " (Pin A1)");
    Serial.println("Simulated Temp: " + String(simTemp) + "°C");
    Serial.println("Simulated Humidity: " + String(simHumidity) + "%");
  } else if (command == "HELP") {
    Serial.println("MARS-SENTINEL Simplified Commands:");
    Serial.println("SET GAS_HIGH <value> - Set gas danger threshold");
    Serial.println("SET GAS_WARN <value> - Set gas warning threshold");
    Serial.println("STATUS - Get current system status");
    Serial.println("HELP - Show this help");
    Serial.println("NOTE: Only gas sensor (A1) is real hardware");
  }
}
