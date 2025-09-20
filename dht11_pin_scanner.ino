/*
 * DHT11 Pin Scanner
 * Tests all digital pins to find where your DHT11 is connected
 */

#include <DHT.h>

#define DHT_TYPE DHT11

// Test pins (ALL possible pins including analog)
int testPins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A3, A4, A5};
int numPins = sizeof(testPins) / sizeof(testPins[0]);

void setup() {
  Serial.begin(9600);
  delay(2000);
  
  Serial.println("🔍 DHT11 Pin Scanner - EXTENDED SEARCH");
  Serial.println("Scanning ALL pins (Digital + Analog) for DHT11...");
  Serial.println("Testing pins: D0,D1,D2-D13,A0,A3,A4,A5");
  Serial.println("====================================================");
  
  for (int i = 0; i < numPins; i++) {
    int pin = testPins[i];
    Serial.print("Testing Pin ");
    if (pin >= A0) {
      Serial.print("A");
      Serial.print(pin - A0);
    } else {
      Serial.print("D");
      Serial.print(pin);
    }
    Serial.print("... ");
    
    DHT dht(pin, DHT_TYPE);
    dht.begin();
    delay(2500); // DHT needs time
    
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();
    
    if (!isnan(temp) && !isnan(humid) && temp > 0 && temp < 80 && humid > 0 && humid < 100) {
      Serial.print("✅ FOUND! ");
      Serial.print(temp);
      Serial.print("°C, ");
      Serial.print(humid);
      Serial.println("%");
    } else {
      Serial.println("❌ No DHT11");
    }
    
    delay(1000);
  }
  
  Serial.println("=====================================");
  Serial.println("Scan complete!");
  Serial.println("\nIf no DHT11 found, check:");
  Serial.println("1. Wiring: VCC→5V, GND→GND, Data→Digital Pin");
  Serial.println("2. DHT11 library installed");
  Serial.println("3. DHT11 is working (try different sensor)");
}

void loop() {
  // Nothing here
}
