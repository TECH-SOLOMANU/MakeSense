/*
 * MARS-SENTINEL Firmware
 * Portable Astronaut Safety & Habitat Monitoring System
 * 
 * Hardware Setup:
 * - LDR: A0 (with 10kΩ resistor to GND)
 * - Gas Sensor (MQ-2/MQ-135): A1
 * - Temperature Sensor (LM35): A2 (or DHT11 on D4)
 * - IR Proximity Sensor: D5
 * - Ultrasonic Trig: D6
 * - Ultrasonic Echo: D7
 * - Buzzer: D9
 * - LED Red: D10
 * - LED Green: D11
 */

#include <DHT.h>

// Pin definitions
#define LDR_PIN A0
#define GAS_PIN A1
#define TEMP_ANALOG_PIN A2
#define DHT_PIN 4
#define IR_PIN 5
#define ULTRASONIC_TRIG 6
#define ULTRASONIC_ECHO 7
#define BUZZER_PIN 9
#define LED_RED 10
#define LED_GREEN 11

// Sensor configuration
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Timing
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 200; // 200ms as per PRD

// Thresholds (can be updated via serial commands)
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
  
  // Initialize DHT sensor
  dht.begin();
  
  // Configure pins
  pinMode(LDR_PIN, INPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(TEMP_ANALOG_PIN, INPUT);
  pinMode(IR_PIN, INPUT);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  
  // Initial state - green LED on
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_RED, LOW);
  
  Serial.println("MARS-SENTINEL System Initialized");
  Serial.println("Format: timestamp,temp,humidity,gas,ir,distance");
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
  // Read all sensors
  float temperature = readTemperature();
  float humidity = readHumidity();
  int gasLevel = readGasLevel();
  int irDetection = digitalRead(IR_PIN);
  long distance = readUltrasonicDistance();
  int lightLevel = readLightLevel();
  
  // Determine system status
  SystemStatus newStatus = evaluateStatus(temperature, humidity, gasLevel, irDetection, distance);
  
  // Update LEDs and buzzer based on status
  updateAlarms(newStatus);
  currentStatus = newStatus;
  
  // Send data via serial in CSV format
  Serial.print(millis());
  Serial.print(",");
  Serial.print(temperature, 2);
  Serial.print(",");
  Serial.print(humidity, 2);
  Serial.print(",");
  Serial.print(gasLevel);
  Serial.print(",");
  Serial.print(irDetection);
  Serial.print(",");
  Serial.println(distance);
}

float readTemperature() {
  // Try DHT11 first
  float temp = dht.readTemperature();
  
  // If DHT11 fails, use LM35 analog sensor
  if (isnan(temp)) {
    int analogValue = analogRead(TEMP_ANALOG_PIN);
    temp = (analogValue * 5.0 / 1024.0) * 100.0; // LM35: 10mV/°C
  }
  
  return temp;
}

float readHumidity() {
  float humidity = dht.readHumidity();
  return isnan(humidity) ? 0.0 : humidity;
}

int readGasLevel() {
  return analogRead(GAS_PIN);
}

int readLightLevel() {
  int rawValue = analogRead(LDR_PIN);
  // Convert to percentage (0-100%)
  return map(rawValue, 0, 1023, 0, 100);
}

long readUltrasonicDistance() {
  // Clear trigger pin
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(5);
  
  // Send 10us pulse
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  // Read echo duration with longer timeout
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 60000); // 60ms timeout for better range
  
  // DEBUG: Print raw duration to help diagnose
  if (millis() % 1000 < 200) { // Only print debug every ~1 second
    Serial.print("DEBUG: Raw duration = ");
    Serial.print(duration);
    Serial.print(" us, ");
  }
  
  // If no echo received (timeout), return max range
  if (duration == 0) {
    if (millis() % 1000 < 200) {
      Serial.println("NO ECHO (timeout)");
    }
    return 400; // Max range for timeout
  }
  
  // Calculate distance (speed of sound = 343 m/s at 20°C)
  // Distance = (duration in microseconds * 0.0343) / 2
  long distance = (duration * 0.0343) / 2;
  
  if (millis() % 1000 < 200) {
    Serial.print("Calculated distance = ");
    Serial.println(distance);
  }
  
  // Constrain to valid HC-SR04 range (2-400cm)
  if (distance < 2) {
    return 2; // Minimum detection range
  } else if (distance > 400) {
    return 400; // Maximum range
  }
  
  return distance;
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
  
  // Gas level checks
  if (gas > thresholds.gasHigh) {
    status = STATUS_DANGER;
  } else if (gas > thresholds.gasWarn && status != STATUS_DANGER) {
    status = STATUS_WARN;
  }
  
  // Distance checks (obstacle detection)
  if (distance < thresholds.distanceDanger) {
    status = STATUS_DANGER;
  } else if (distance < thresholds.distanceWarn && status != STATUS_DANGER) {
    status = STATUS_WARN;
  }
  
  // IR edge detection
  if (ir >= thresholds.irDanger) {
    status = STATUS_DANGER;
  }
  
  return status;
}

void updateAlarms(SystemStatus status) {
  switch (status) {
    case STATUS_OK:
      digitalWrite(LED_GREEN, HIGH);
      digitalWrite(LED_RED, LOW);
      noTone(BUZZER_PIN);
      break;
      
    case STATUS_WARN:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      noTone(BUZZER_PIN);
      break;
      
    case STATUS_DANGER:
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_RED, HIGH);
      // Fast beep pattern for danger
      tone(BUZZER_PIN, 1000, 100);
      break;
  }
}

void processSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command.startsWith("SET ")) {
    // Parse threshold update commands
    // Format: SET TEMP_HIGH 45
    int spaceIndex = command.indexOf(' ', 4);
    if (spaceIndex > 0) {
      String parameter = command.substring(4, spaceIndex);
      float value = command.substring(spaceIndex + 1).toFloat();
      
      if (parameter == "TEMP_HIGH") {
        thresholds.tempHigh = value;
        Serial.println("OK: TEMP_HIGH updated to " + String(value));
      } else if (parameter == "TEMP_WARN") {
        thresholds.tempWarn = value;
        Serial.println("OK: TEMP_WARN updated to " + String(value));
      } else if (parameter == "HUMID_HIGH") {
        thresholds.humidHigh = value;
        Serial.println("OK: HUMID_HIGH updated to " + String(value));
      } else if (parameter == "HUMID_WARN") {
        thresholds.humidWarn = value;
        Serial.println("OK: HUMID_WARN updated to " + String(value));
      } else if (parameter == "GAS_HIGH") {
        thresholds.gasHigh = (int)value;
        Serial.println("OK: GAS_HIGH updated to " + String((int)value));
      } else if (parameter == "GAS_WARN") {
        thresholds.gasWarn = (int)value;
        Serial.println("OK: GAS_WARN updated to " + String((int)value));
      } else if (parameter == "DIST_WARN") {
        thresholds.distanceWarn = (int)value;
        Serial.println("OK: DIST_WARN updated to " + String((int)value));
      } else if (parameter == "DIST_DANGER") {
        thresholds.distanceDanger = (int)value;
        Serial.println("OK: DIST_DANGER updated to " + String((int)value));
      } else {
        Serial.println("ERROR: Unknown parameter " + parameter);
      }
    }
  } else if (command == "STATUS") {
    // Return current system status
    Serial.println("System Status: " + String(currentStatus == STATUS_OK ? "OK" : 
                                              currentStatus == STATUS_WARN ? "WARN" : "DANGER"));
  } else if (command == "THRESHOLDS") {
    // Return current thresholds
    Serial.println("Current Thresholds:");
    Serial.println("TEMP_HIGH: " + String(thresholds.tempHigh));
    Serial.println("TEMP_WARN: " + String(thresholds.tempWarn));
    Serial.println("HUMID_HIGH: " + String(thresholds.humidHigh));
    Serial.println("HUMID_WARN: " + String(thresholds.humidWarn));
    Serial.println("GAS_HIGH: " + String(thresholds.gasHigh));
    Serial.println("GAS_WARN: " + String(thresholds.gasWarn));
    Serial.println("DIST_WARN: " + String(thresholds.distanceWarn));
    Serial.println("DIST_DANGER: " + String(thresholds.distanceDanger));
  } else if (command == "HELP") {
    Serial.println("MARS-SENTINEL Commands:");
    Serial.println("SET TEMP_HIGH <value> - Set temperature danger threshold");
    Serial.println("SET TEMP_WARN <value> - Set temperature warning threshold");
    Serial.println("SET GAS_HIGH <value> - Set gas danger threshold");
    Serial.println("STATUS - Get current system status");
    Serial.println("THRESHOLDS - Show all thresholds");
    Serial.println("HELP - Show this help");
  }
}
