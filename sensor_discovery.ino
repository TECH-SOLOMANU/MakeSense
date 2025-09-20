/*
 * MARS-SENTINEL Sensor Discovery
 * This program scans ALL pins to find what sensors are actually connected
 * Upload this and check Serial Monitor to see your real hardware setup
 */

void setup() {
  Serial.begin(9600);
  Serial.println("=== MARS-SENTINEL SENSOR DISCOVERY ===");
  Serial.println("Scanning ALL pins to find connected sensors...");
  Serial.println("===============================================");
  delay(2000);
}

void loop() {
  Serial.println("\n--- SCANNING ALL PINS ---");
  
  // Scan all ANALOG pins (A0-A5)
  Serial.println("\n🔍 ANALOG PINS SCAN:");
  for (int pin = A0; pin <= A5; pin++) {
    int value = analogRead(pin);
    Serial.print("Pin A");
    Serial.print(pin - A0);
    Serial.print(": ");
    Serial.print(value);
    
    // Analyze what type of sensor this might be
    if (value == 0) {
      Serial.println(" (No sensor / disconnected)");
    } else if (value > 1000) {
      Serial.println(" (Possible floating pin / no sensor)");
    } else if (value > 800) {
      Serial.println(" (⚠️ High reading - possible gas sensor heating up)");
    } else if (value > 500) {
      Serial.println(" (📊 Medium reading - could be gas/light sensor)");
    } else if (value > 100) {
      Serial.println(" (🌡️ Low-medium reading - possible temperature sensor)");
    } else {
      Serial.println(" (🔅 Low reading - possible light sensor or clean gas sensor)");
    }
    delay(100);
  }
  
  // Scan all DIGITAL pins (D2-D13, skipping 0,1 for serial)
  Serial.println("\n🔍 DIGITAL PINS SCAN:");
  for (int pin = 2; pin <= 13; pin++) {
    pinMode(pin, INPUT_PULLUP);
    int value = digitalRead(pin);
    Serial.print("Pin D");
    Serial.print(pin);
    Serial.print(": ");
    Serial.print(value ? "HIGH" : "LOW");
    
    // Test if it might be a sensor
    pinMode(pin, INPUT);
    int value2 = digitalRead(pin);
    if (value != value2) {
      Serial.println(" (🔄 Changes when pullup removed - possible sensor)");
    } else if (value == 0) {
      Serial.println(" (📊 Consistent LOW - might be sensor ground or output)");
    } else {
      Serial.println(" (⚡ Consistent HIGH - might be sensor VCC or floating)");
    }
    delay(100);
  }
  
  // Test for common sensor patterns
  Serial.println("\n🧪 SENSOR TYPE DETECTION:");
  
  // Look for DHT11/DHT22 pattern (usually gives varying readings)
  Serial.println("\nTesting for DHT11/DHT22 sensors on digital pins:");
  for (int pin = 2; pin <= 13; pin++) {
    pinMode(pin, INPUT);
    int reading1 = digitalRead(pin);
    delay(10);
    int reading2 = digitalRead(pin);
    delay(10);
    int reading3 = digitalRead(pin);
    
    if (reading1 != reading2 || reading2 != reading3) {
      Serial.print("Pin D");
      Serial.print(pin);
      Serial.println(" - 🌡️ Possible DHT sensor (varying digital signal)");
    }
  }
  
  // Look for gas sensor pattern (analog, usually 300-800 range)
  Serial.println("\nTesting for Gas sensors on analog pins:");
  for (int pin = A0; pin <= A5; pin++) {
    int value = analogRead(pin);
    if (value > 200 && value < 900) {
      Serial.print("Pin A");
      Serial.print(pin - A0);
      Serial.print(" - ☣️ Possible GAS sensor (reading: ");
      Serial.print(value);
      Serial.println(")");
    }
  }
  
  // Look for light sensor pattern (varies with light)
  Serial.println("\nTesting for Light sensors (cover/uncover to test):");
  for (int pin = A0; pin <= A5; pin++) {
    int value1 = analogRead(pin);
    delay(100);
    int value2 = analogRead(pin);
    int diff = abs(value1 - value2);
    
    if (diff > 50 && (value1 < 800 && value2 < 800)) {
      Serial.print("Pin A");
      Serial.print(pin - A0);
      Serial.println(" - 🔅 Possible LIGHT sensor (reading changes)");
    }
  }
  
  // Test for ultrasonic sensor
  Serial.println("\nTesting for Ultrasonic sensor (trying all pin combinations):");
  for (int trigPin = 2; trigPin <= 12; trigPin++) {
    for (int echoPin = trigPin + 1; echoPin <= 13; echoPin++) {
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
      
      digitalWrite(trigPin, LOW);
      delayMicroseconds(5);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      
      long duration = pulseIn(echoPin, HIGH, 10000); // Short timeout
      
      if (duration > 0) {
        Serial.print("📏 ULTRASONIC found! TRIG=D");
        Serial.print(trigPin);
        Serial.print(", ECHO=D");
        Serial.print(echoPin);
        Serial.print(" (distance: ");
        Serial.print((duration * 0.0343) / 2);
        Serial.println(" cm)");
      }
    }
  }
  
  Serial.println("\n===============================================");
  Serial.println("🔄 Rescanning in 5 seconds...");
  Serial.println("💡 TIP: Move objects, cover sensors to see changes!");
  delay(5000);
}
