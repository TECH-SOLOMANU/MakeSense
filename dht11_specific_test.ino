/*
 * DHT11 Specific Test - A0 and D10 Configuration
 * Testing your exact wiring setup
 */

#include <DHT.h>

#define DHT_TYPE DHT11

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("🔍 DHT11 Specific Test - Your Wiring");
  Serial.println("Testing A0 and D10 configuration...");
  Serial.println("=====================================");
  
  // Test D10 as data pin (most likely)
  Serial.println("Testing D10 as DATA pin...");
  DHT dht10(10, DHT_TYPE);
  dht10.begin();
  delay(3000); // Extra time for DHT11
  
  float temp10 = dht10.readTemperature();
  float humid10 = dht10.readHumidity();
  
  if (!isnan(temp10) && !isnan(humid10) && temp10 > 0 && temp10 < 80) {
    Serial.print("✅ SUCCESS! DHT11 found with D10 as DATA pin: ");
    Serial.print(temp10);
    Serial.print("°C, ");
    Serial.print(humid10);
    Serial.println("%");
    Serial.println("Correct wiring: VCC→5V, GND→GND, DATA→D10");
  } else {
    Serial.println("❌ D10 as data pin failed");
  }
  
  delay(2000);
  
  // Test A0 as data pin (less likely but possible)
  Serial.println("Testing A0 as DATA pin...");
  DHT dhtA0(A0, DHT_TYPE);
  dhtA0.begin();
  delay(3000);
  
  float tempA0 = dhtA0.readTemperature();
  float humidA0 = dhtA0.readHumidity();
  
  if (!isnan(tempA0) && !isnan(humidA0) && tempA0 > 0 && tempA0 < 80) {
    Serial.print("✅ SUCCESS! DHT11 found with A0 as DATA pin: ");
    Serial.print(tempA0);
    Serial.print("°C, ");
    Serial.print(humidA0);
    Serial.println("%");
    Serial.println("Correct wiring: VCC→5V, GND→GND, DATA→A0");
  } else {
    Serial.println("❌ A0 as data pin failed");
  }
  
  Serial.println("=====================================");
  Serial.println("Test complete!");
  Serial.println("\nMost likely wiring:");
  Serial.println("DHT11 VCC → Arduino 5V");
  Serial.println("DHT11 GND → Arduino GND");
  Serial.println("DHT11 DATA → Arduino D10");
  Serial.println("DHT11 NC → Not connected");
}

void loop() {
  // Nothing here
}
