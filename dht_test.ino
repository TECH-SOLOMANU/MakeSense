/*
 * DHT11 Diagnostic Test
 * This will help diagnose DHT11 connection issues
 */

#include <DHT.h>

#define DHT_PIN 4
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  Serial.println("DHT11 Diagnostic Test Starting...");
  Serial.println("Wiring Check:");
  Serial.println("DHT11 VCC -> Arduino 5V (or 3.3V)");
  Serial.println("DHT11 GND -> Arduino GND");
  Serial.println("DHT11 DATA -> Arduino Pin 4");
  Serial.println("DHT11 might need 10kΩ pull-up resistor between DATA and VCC");
  Serial.println("=====================================");
  
  dht.begin();
  delay(2000);  // Initial startup delay
}

void loop() {
  // Read temperature and humidity
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  Serial.print("Timestamp: ");
  Serial.print(millis());
  Serial.print(" ms | ");
  
  // Check if readings are valid
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("❌ DHT11 reading failed!");
    Serial.println("Possible issues:");
    Serial.println("1. DHT11 not connected to pin 4");
    Serial.println("2. DHT11 power not connected (VCC to 5V, GND to GND)");
    Serial.println("3. DHT11 needs 10kΩ pull-up resistor on DATA line");
    Serial.println("4. DHT11 sensor might be faulty");
    Serial.println("5. Try different DHT11 sensor");
  } else {
    Serial.print("✅ Temperature: ");
    Serial.print(temperature);
    Serial.print("°C | Humidity: ");
    Serial.print(humidity);
    Serial.println("%");
  }
  
  delay(2000);  // DHT11 needs at least 1 second between readings
}
