/*
 * DHT11 Advanced Troubleshooting
 * Tests different power levels and pin combinations
 */

#include <DHT.h>

#define DHT_TYPE DHT11

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("🔧 DHT11 Advanced Troubleshooting");
  Serial.println("Testing different configurations...");
  Serial.println("=====================================");
  
  // Enable internal pull-up on test pins
  pinMode(A0, INPUT_PULLUP);
  pinMode(10, INPUT_PULLUP);
  
  Serial.println("Phase 1: Testing with internal pull-ups...");
  
  // Test D10 with pull-up
  Serial.println("Testing D10 with internal pull-up...");
  DHT dht10(10, DHT_TYPE);
  dht10.begin();
  delay(4000); // Longer delay
  
  float temp10 = dht10.readTemperature();
  float humid10 = dht10.readHumidity();
  
  Serial.print("D10 readings: Temp=");
  Serial.print(temp10);
  Serial.print("°C, Humidity=");
  Serial.print(humid10);
  Serial.print("% (Valid: ");
  Serial.print(!isnan(temp10) && !isnan(humid10));
  Serial.println(")");
  
  if (!isnan(temp10) && !isnan(humid10) && temp10 > 0 && temp10 < 80) {
    Serial.println("✅ SUCCESS with D10!");
  } else {
    Serial.println("❌ D10 still failed");
  }
  
  delay(2000);
  
  // Test A0 with pull-up
  Serial.println("Testing A0 with internal pull-up...");
  DHT dhtA0(A0, DHT_TYPE);
  dhtA0.begin();
  delay(4000);
  
  float tempA0 = dhtA0.readTemperature();
  float humidA0 = dhtA0.readHumidity();
  
  Serial.print("A0 readings: Temp=");
  Serial.print(tempA0);
  Serial.print("°C, Humidity=");
  Serial.print(humidA0);
  Serial.print("% (Valid: ");
  Serial.print(!isnan(tempA0) && !isnan(humidA0));
  Serial.println(")");
  
  if (!isnan(tempA0) && !isnan(humidA0) && tempA0 > 0 && tempA0 < 80) {
    Serial.println("✅ SUCCESS with A0!");
  } else {
    Serial.println("❌ A0 still failed");
  }
  
  Serial.println("\n=====================================");
  Serial.println("Diagnostic complete!");
  Serial.println("\nIf both still fail, check:");
  Serial.println("1. DHT11 power: Is VCC connected to 5V or 3.3V?");
  Serial.println("2. DHT11 ground: Is GND connected to Arduino GND?");
  Serial.println("3. DHT11 model: Some need external 4.7kΩ pull-up resistor");
  Serial.println("4. Wiring: Are connections secure?");
  Serial.println("5. DHT11 health: Try a different DHT11 sensor");
  
  Serial.println("\nCurrent setup assumption:");
  Serial.println("- One wire (A0 or D10) = DATA");
  Serial.println("- Need VCC → 5V and GND → GND somewhere");
}

void loop() {
  // Nothing here
}
