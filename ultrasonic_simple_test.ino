/*
 * Simple HC-SR04 Ultrasonic Sensor Test
 * This will help verify if the sensor hardware is working
 */

#define TRIG_PIN 6
#define ECHO_PIN 7

void setup() {
  Serial.begin(9600);
  Serial.println("=== HC-SR04 Hardware Test ===");
  Serial.println("Checking connections...");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  
  // Test if ECHO pin is properly connected
  Serial.print("ECHO pin (Pin 7) state: ");
  Serial.println(digitalRead(ECHO_PIN) ? "HIGH" : "LOW");
  
  delay(1000);
}

void loop() {
  Serial.println("\n--- Testing HC-SR04 ---");
  
  // Clear trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(5);
  
  // Send trigger pulse
  Serial.println("1. Sending TRIG pulse...");
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Check ECHO pin state immediately after trigger
  Serial.print("2. ECHO pin state after trigger: ");
  Serial.println(digitalRead(ECHO_PIN) ? "HIGH" : "LOW");
  
  // Wait for echo with timeout
  Serial.println("3. Waiting for ECHO response...");
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  if (duration == 0) {
    Serial.println("❌ NO ECHO RECEIVED!");
    Serial.println("PROBLEM: ECHO pin not connected or sensor faulty");
    Serial.println("");
    Serial.println("Check:");
    Serial.println("- ECHO wire connected to Pin 7");
    Serial.println("- VCC connected to 5V");
    Serial.println("- GND connected to GND");
    Serial.println("- Try different jumper wires");
  } else {
    long distance = (duration * 0.0343) / 2;
    Serial.print("✅ ECHO received! Duration: ");
    Serial.print(duration);
    Serial.print(" us, Distance: ");
    Serial.print(distance);
    Serial.println(" cm");
  }
  
  delay(2000); // Wait 2 seconds before next test
}
