/*
 * HC-SR04 Ultrasonic Sensor Hardware Diagnostic Test
 * 
 * This test helps identify hardware wiring issues
 * Upload this to Arduino and open Serial Monitor (9600 baud)
 */

#define ULTRASONIC_TRIG 6
#define ULTRASONIC_ECHO 7

void setup() {
  Serial.begin(9600);
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  Serial.println("=== HC-SR04 Hardware Diagnostic Test ===");
  Serial.println("TRIG Pin: 6");
  Serial.println("ECHO Pin: 7");
  Serial.println("Expected: VCC->5V, GND->GND, TRIG->Pin6, ECHO->Pin7");
  Serial.println("==========================================");
  delay(2000);
}

void loop() {
  Serial.println("\n--- Test Cycle ---");
  
  // Step 1: Check ECHO pin initial state
  int echoState = digitalRead(ULTRASONIC_ECHO);
  Serial.print("1. ECHO Pin 7 initial state: ");
  Serial.println(echoState == HIGH ? "HIGH" : "LOW");
  
  // Step 2: Send trigger pulse
  Serial.println("2. Sending 10us trigger pulse...");
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  // Step 3: Wait and check for ECHO response
  Serial.println("3. Waiting for ECHO response...");
  unsigned long startTime = micros();
  unsigned long duration = pulseIn(ULTRASONIC_ECHO, HIGH, 30000); // 30ms timeout
  unsigned long endTime = micros();
  
  Serial.print("4. ECHO duration: ");
  Serial.print(duration);
  Serial.println(" microseconds");
  
  if (duration == 0) {
    Serial.println("❌ ERROR: No ECHO signal received!");
    Serial.println("   Possible causes:");
    Serial.println("   - ECHO pin not connected to Pin 7");
    Serial.println("   - Loose jumper wire");
    Serial.println("   - Damaged HC-SR04 sensor");
    Serial.println("   - Wrong pin connection");
  } else {
    float distance = (duration * 0.0343) / 2;
    Serial.print("✅ SUCCESS: Distance = ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  // Step 4: Check ECHO pin final state
  echoState = digitalRead(ULTRASONIC_ECHO);
  Serial.print("5. ECHO Pin 7 final state: ");
  Serial.println(echoState == HIGH ? "HIGH" : "LOW");
  
  Serial.println("------------------");
  delay(2000); // Wait 2 seconds between tests
}
