/*
 * MARS-SENTINEL Pin Connection Test
 * Upload this to test which sensors are properly connected
 */

#include <DHT.h>

// Pin definitions (same as main firmware)
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

#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

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
  
  Serial.println("=== MARS-SENTINEL PIN CONNECTION TEST ===");
  Serial.println("Testing all sensor connections...");
  Serial.println("==========================================");
  delay(2000);
}

void loop() {
  Serial.println("\n--- SENSOR CONNECTION TEST ---");
  
  // Test LEDs first (visual confirmation)
  Serial.println("1. Testing LEDs...");
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
  delay(500);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
  delay(500);
  digitalWrite(LED_GREEN, LOW);
  Serial.println("   LEDs: RED (Pin 10) and GREEN (Pin 11) should blink");
  
  // Test Buzzer
  Serial.println("2. Testing Buzzer (Pin 9)...");
  tone(BUZZER_PIN, 1000, 200);
  Serial.println("   Buzzer should beep once");
  delay(500);
  
  // Test DHT11 Temperature/Humidity
  Serial.println("3. Testing DHT11 (Pin 4)...");
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("   ❌ DHT11 NOT CONNECTED or FAULTY");
    Serial.println("   Check: VCC->5V, GND->GND, Data->Pin 4");
  } else {
    Serial.print("   ✅ DHT11 WORKING: ");
    Serial.print(temperature);
    Serial.print("°C, ");
    Serial.print(humidity);
    Serial.println("%");
  }
  
  // Test Gas Sensor
  Serial.println("4. Testing Gas Sensor (Pin A1)...");
  int gasValue = analogRead(GAS_PIN);
  Serial.print("   Gas reading: ");
  Serial.print(gasValue);
  if (gasValue > 0 && gasValue < 1023) {
    Serial.println(" ✅ GAS SENSOR WORKING");
  } else if (gasValue == 0) {
    Serial.println(" ❌ GAS SENSOR: No signal (check connections)");
  } else {
    Serial.println(" ⚠️ GAS SENSOR: Unusual reading (check VCC)");
  }
  
  // Test LDR Light Sensor
  Serial.println("5. Testing LDR Light Sensor (Pin A0)...");
  int lightValue = analogRead(LDR_PIN);
  Serial.print("   Light reading: ");
  Serial.print(lightValue);
  if (lightValue > 0 && lightValue < 1023) {
    Serial.println(" ✅ LDR WORKING (try covering/uncovering)");
  } else {
    Serial.println(" ❌ LDR: Check connections (A0 + 10kΩ resistor to GND)");
  }
  
  // Test LM35 Temperature (if using analog)
  Serial.println("6. Testing LM35 Temperature (Pin A2)...");
  int lm35Value = analogRead(TEMP_ANALOG_PIN);
  float lm35Temp = (lm35Value * 5.0 / 1024.0) * 100.0;
  Serial.print("   LM35 reading: ");
  Serial.print(lm35Temp);
  Serial.println("°C");
  if (lm35Temp > 0 && lm35Temp < 80) {
    Serial.println("   ✅ LM35 might be working");
  } else {
    Serial.println("   ❌ LM35: No sensor or not connected to A2");
  }
  
  // Test IR Sensor
  Serial.println("7. Testing IR Proximity Sensor (Pin 5)...");
  int irValue = digitalRead(IR_PIN);
  Serial.print("   IR reading: ");
  Serial.println(irValue ? "DETECTED" : "CLEAR");
  Serial.println("   Try moving object near IR sensor");
  
  // Test Ultrasonic Sensor
  Serial.println("8. Testing Ultrasonic Sensor (Pins 6&7)...");
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000);
  
  if (duration == 0) {
    Serial.println("   ❌ ULTRASONIC: ECHO not connected to Pin 7");
    Serial.println("   Check: TRIG->Pin 6, ECHO->Pin 7, VCC->5V, GND->GND");
  } else {
    float distance = (duration * 0.0343) / 2;
    Serial.print("   ✅ ULTRASONIC WORKING: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  Serial.println("==========================================");
  Serial.println("Repeat test in 5 seconds...");
  delay(5000);
}
