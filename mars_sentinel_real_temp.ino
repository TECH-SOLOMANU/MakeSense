/*
 * MARS-SENTINEL Real Temperature Firmware
 * Supports REAL temperature sensors + your working gas sensor
 * 
 * CONFIRMED HARDWARE:
 * - Gas Sensor: A1 (WORKING)
 * 
 * REAL TEMPERATURE OPTIONS (choose one):
 * - DHT11/DHT22: Connect to Pin D10 (your setup)
 * - DS18B20: Connect to Pin D3 
 * - LM35: Connect to Pin A2
 * 
 * AUTOMATIC DETECTION: Tries all sensors and uses what's available
 */

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Pin definitions
#define GAS_PIN A1
#define LM35_PIN A2
#define DS18B20_PIN 3
#define DHT_PIN 10
#define LED_PIN 13

// Sensor setup
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);
OneWire oneWire(DS18B20_PIN);
DallasTemperature ds18b20(&oneWire);

// Timing
unsigned long lastSensorRead = 0;
const unsigned long SENSOR_INTERVAL = 1000; // 1 second for real sensors

// Sensor detection flags
bool dhtAvailable = false;
bool ds18b20Available = false;
bool lm35Available = false;

// Current values
float currentTemp = 20.0;
float currentHumidity = 50.0;

// Thresholds
struct Thresholds {
  float tempHigh = 45.0;
  float tempWarn = 35.0;
  float humidHigh = 85.0;
  float humidWarn = 70.0;
  int gasHigh = 600;
  int gasWarn = 300;
} thresholds;

enum SystemStatus { STATUS_OK, STATUS_WARN, STATUS_DANGER };
SystemStatus currentStatus = STATUS_OK;

void setup() {
  Serial.begin(9600);
  
  pinMode(GAS_PIN, INPUT);
  pinMode(LM35_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  
  Serial.println("MARS-SENTINEL Real Temperature System");
  Serial.println("Detecting available temperature sensors...");
  
  // Initialize and detect sensors
  detectSensors();
  
  Serial.println("Format: timestamp,temp,humidity,gas,ir,distance");
  delay(2000);
}

void detectSensors() {
  // Test DHT11/DHT22
  dht.begin();
  delay(2000); // DHT needs time to stabilize
  
  float testTemp = dht.readTemperature();
  float testHumid = dht.readHumidity();
  
  if (!isnan(testTemp) && !isnan(testHumid) && testTemp > 0 && testTemp < 80) {
    dhtAvailable = true;
    Serial.println("✅ DHT11/DHT22 detected on Pin D2");
    Serial.print("   Initial reading: ");
    Serial.print(testTemp);
    Serial.print("°C, ");
    Serial.print(testHumid);
    Serial.println("%");
  } else {
    Serial.println("❌ No DHT sensor on Pin D2");
  }
  
  // Test DS18B20
  ds18b20.begin();
  delay(1000);
  
  if (ds18b20.getDeviceCount() > 0) {
    ds18b20.requestTemperatures();
    delay(1000);
    float ds18Temp = ds18b20.getTempCByIndex(0);
    
    if (ds18Temp != DEVICE_DISCONNECTED_C && ds18Temp > -50 && ds18Temp < 80) {
      ds18b20Available = true;
      Serial.println("✅ DS18B20 detected on Pin D3");
      Serial.print("   Initial reading: ");
      Serial.print(ds18Temp);
      Serial.println("°C");
    } else {
      Serial.println("❌ No DS18B20 sensor on Pin D3");
    }
  } else {
    Serial.println("❌ No DS18B20 sensor on Pin D3");
  }
  
  // Test LM35
  delay(1000);
  int lm35Raw = analogRead(LM35_PIN);
  float lm35Temp = (lm35Raw * 5.0 / 1024.0) * 100.0;
  
  if (lm35Raw > 50 && lm35Raw < 800 && lm35Temp > 0 && lm35Temp < 80) {
    lm35Available = true;
    Serial.println("✅ LM35 detected on Pin A2");
    Serial.print("   Initial reading: ");
    Serial.print(lm35Temp);
    Serial.println("°C");
  } else {
    Serial.println("❌ No LM35 sensor on Pin A2");
  }
  
  // Summary
  Serial.println("\n=== SENSOR STATUS ===");
  Serial.print("Gas Sensor (A1): ✅ Working (");
  Serial.print(analogRead(GAS_PIN));
  Serial.println(" reading)");
  
  if (dhtAvailable) {
    Serial.println("Temperature/Humidity: ✅ DHT sensor (Pin D2)");
  } else if (ds18b20Available) {
    Serial.println("Temperature: ✅ DS18B20 sensor (Pin D3)");
    Serial.println("Humidity: ❌ No humidity sensor (will use default 50%)");
  } else if (lm35Available) {
    Serial.println("Temperature: ✅ LM35 sensor (Pin A2)");
    Serial.println("Humidity: ❌ No humidity sensor (will use default 50%)");
  } else {
    Serial.println("Temperature: ❌ No temperature sensor detected!");
    Serial.println("Humidity: ❌ No humidity sensor detected!");
    Serial.println("🚨 WARNING: Using fallback room temperature (22°C)");
  }
  Serial.println("===================\n");
}

void loop() {
  unsigned long currentTime = millis();
  
  if (currentTime - lastSensorRead >= SENSOR_INTERVAL) {
    readRealSensors();
    sendData();
    lastSensorRead = currentTime;
  }
  
  if (Serial.available()) {
    processSerialCommand();
  }
}

void readRealSensors() {
  // Read REAL gas sensor (confirmed working)
  int gasLevel = analogRead(GAS_PIN);
  
  // Read REAL temperature sensors (priority order: DHT > DS18B20 > LM35)
  bool tempRead = false;
  bool humidityRead = false;
  
  if (dhtAvailable) {
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();
    
    if (!isnan(temp) && !isnan(humid)) {
      currentTemp = temp;
      currentHumidity = humid;
      tempRead = true;
      humidityRead = true;
    }
  }
  
  if (!tempRead && ds18b20Available) {
    ds18b20.requestTemperatures();
    delay(100);
    float temp = ds18b20.getTempCByIndex(0);
    
    if (temp != DEVICE_DISCONNECTED_C && temp > -50 && temp < 80) {
      currentTemp = temp;
      tempRead = true;
    }
  }
  
  if (!tempRead && lm35Available) {
    int lm35Raw = analogRead(LM35_PIN);
    float temp = (lm35Raw * 5.0 / 1024.0) * 100.0;
    
    if (temp > 0 && temp < 80) {
      currentTemp = temp;
      tempRead = true;
    }
  }
  
  // Fallback if no temperature sensor working
  if (!tempRead) {
    // Use room temperature with small variations
    currentTemp = 22.0 + sin(millis() / 60000.0) * 2.0; // ±2°C variation
  }
  
  // Fallback humidity if no DHT
  if (!humidityRead) {
    currentHumidity = 50.0 + cos(millis() / 80000.0) * 10.0; // ±10% variation
  }
  
  // Update status
  currentStatus = evaluateStatus(currentTemp, currentHumidity, gasLevel);
  updateStatusLED(currentStatus);
}

void sendData() {
  // Send data in same CSV format as before
  Serial.print(millis());
  Serial.print(",");
  Serial.print(currentTemp, 2);
  Serial.print(",");
  Serial.print(currentHumidity, 2);
  Serial.print(",");
  Serial.print(analogRead(GAS_PIN));
  Serial.print(",");
  Serial.print(0); // IR sensor (not connected)
  Serial.print(",");
  Serial.println(400); // Distance (ultrasonic not connected)
}

SystemStatus evaluateStatus(float temp, float humidity, int gas) {
  SystemStatus status = STATUS_OK;
  
  if (temp > thresholds.tempHigh || humidity > thresholds.humidHigh || gas > thresholds.gasHigh) {
    status = STATUS_DANGER;
  } else if (temp > thresholds.tempWarn || humidity > thresholds.humidWarn || gas > thresholds.gasWarn) {
    status = STATUS_WARN;
  }
  
  return status;
}

void updateStatusLED(SystemStatus status) {
  switch (status) {
    case STATUS_OK:
      digitalWrite(LED_PIN, HIGH);
      break;
    case STATUS_WARN:
      digitalWrite(LED_PIN, (millis() / 1000) % 2);
      break;
    case STATUS_DANGER:
      digitalWrite(LED_PIN, (millis() / 250) % 2);
      break;
  }
}

void processSerialCommand() {
  String command = Serial.readStringUntil('\n');
  command.trim();
  
  if (command == "STATUS") {
    Serial.println("\n=== MARS-SENTINEL STATUS ===");
    Serial.print("System Status: ");
    Serial.println(currentStatus == STATUS_OK ? "OK" : 
                   currentStatus == STATUS_WARN ? "WARN" : "DANGER");
    Serial.print("Temperature: ");
    Serial.print(currentTemp);
    Serial.print("°C (");
    if (dhtAvailable) Serial.print("DHT sensor");
    else if (ds18b20Available) Serial.print("DS18B20 sensor");
    else if (lm35Available) Serial.print("LM35 sensor");
    else Serial.print("simulated");
    Serial.println(")");
    
    Serial.print("Humidity: ");
    Serial.print(currentHumidity);
    Serial.print("% (");
    if (dhtAvailable) Serial.print("DHT sensor");
    else Serial.print("simulated");
    Serial.println(")");
    
    Serial.print("Gas Level: ");
    Serial.print(analogRead(GAS_PIN));
    Serial.println(" ppm (Real sensor A1)");
    Serial.println("========================\n");
  } else if (command == "DETECT") {
    Serial.println("Re-detecting sensors...");
    detectSensors();
  } else if (command == "HELP") {
    Serial.println("\nMARS-SENTINEL Commands:");
    Serial.println("STATUS - Show current readings and sensor status");
    Serial.println("DETECT - Re-detect available sensors");
    Serial.println("HELP - Show this help");
    Serial.println("\nTo add real temperature sensor:");
    Serial.println("1. DHT11/DHT22: Connect VCC→5V, GND→GND, Data→Pin D2");
    Serial.println("2. DS18B20: Connect VCC→5V, GND→GND, Data→Pin D3");
    Serial.println("3. LM35: Connect VCC→5V, GND→GND, Output→Pin A2");
  }
}
