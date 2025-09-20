/*
 * HC-SR04 Multi-Pin Diagnostic Test
 * Tests ultrasonic sensor on different Arduino pins to isolate hardware issues
 */

#define ULTRASONIC_TRIG 6

void setup() {
  Serial.begin(9600);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  
  Serial.println("=== HC-SR04 Multi-Pin Diagnostic Test ===");
  Serial.println("Testing ECHO connection on multiple pins...");
  Serial.println("TRIG Pin: 6 (fixed)");
  Serial.println("==========================================");
  delay(2000);
}

void loop() {
  Serial.println("\n--- Testing Different ECHO Pins ---");
  
  // Test pins 7, 8, 9, 10
  int testPins[] = {7, 8, 9, 10};
  int numPins = 4;
  
  for (int i = 0; i < numPins; i++) {
    int echoPin = testPins[i];
    pinMode(echoPin, INPUT);
    
    Serial.print("Testing ECHO on Pin ");
    Serial.print(echoPin);
    Serial.print(": ");
    
    // Send trigger pulse
    digitalWrite(ULTRASONIC_TRIG, LOW);
    delayMicroseconds(5);
    digitalWrite(ULTRASONIC_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(ULTRASONIC_TRIG, LOW);
    
    // Wait for echo
    unsigned long duration = pulseIn(echoPin, HIGH, 30000);
    
    if (duration == 0) {
      Serial.println("❌ No signal");
    } else {
      float distance = (duration * 0.0343) / 2;
      Serial.print("✅ SUCCESS! Duration: ");
      Serial.print(duration);
      Serial.print(" us, Distance: ");
      Serial.print(distance);
      Serial.println(" cm");
    }
    
    delay(200); // Small delay between tests
  }
  
  Serial.println("\n💡 Instructions:");
  Serial.println("- If Pin 7 shows 'No signal': Check ECHO wire connection to Pin 7");
  Serial.println("- If another pin works: Move ECHO wire to that working pin");
  Serial.println("- If no pins work: Check HC-SR04 power connections (VCC/GND)");
  Serial.println("==========================================");
  
  delay(3000); // Wait 3 seconds before next test cycle
}
