/*
 * HC-SR04 Ultrasonic Sensor Diagnostic Test
 * This will help diagnose ultrasonic sensor connection issues
 */

#define ULTRASONIC_TRIG 6
#define ULTRASONIC_ECHO 7

void setup() {
  Serial.begin(9600);
  Serial.println("HC-SR04 Ultrasonic Sensor Diagnostic Test");
  Serial.println("==========================================");
  Serial.println("Wiring Check:");
  Serial.println("HC-SR04 VCC  -> Arduino 5V");
  Serial.println("HC-SR04 GND  -> Arduino GND");
  Serial.println("HC-SR04 TRIG -> Arduino Pin 6");
  Serial.println("HC-SR04 ECHO -> Arduino Pin 7");
  Serial.println("==========================================");
  
  // Configure pins
  pinMode(ULTRASONIC_TRIG, OUTPUT);
  pinMode(ULTRASONIC_ECHO, INPUT);
  
  delay(2000); // Startup delay
}

void loop() {
  // Test the ultrasonic sensor
  long distance = measureDistance();
  long rawDuration = getRawDuration();
  
  Serial.print("Time: ");
  Serial.print(millis());
  Serial.print(" ms | ");
  
  if (rawDuration == 0) {
    Serial.println("❌ NO ECHO RECEIVED!");
    Serial.println("Possible issues:");
    Serial.println("1. ECHO pin not connected to Pin 7");
    Serial.println("2. TRIG pin not connected to Pin 6");
    Serial.println("3. HC-SR04 power not connected (VCC to 5V, GND to GND)");
    Serial.println("4. HC-SR04 sensor is faulty");
    Serial.println("5. No object within 2-400cm range");
    Serial.println("---");
  } else {
    Serial.print("✅ Distance: ");
    Serial.print(distance);
    Serial.print(" cm | Raw Duration: ");
    Serial.print(rawDuration);
    Serial.print(" μs");
    
    if (distance >= 400) {
      Serial.println(" (MAX RANGE - move object closer)");
    } else if (distance <= 2) {
      Serial.println(" (TOO CLOSE - move object further)");
    } else {
      Serial.println(" (GOOD READING)");
    }
  }
  
  delay(500); // Half second between readings
}

long getRawDuration() {
  // Clear trigger pin
  digitalWrite(ULTRASONIC_TRIG, LOW);
  delayMicroseconds(5);
  
  // Send 10us pulse
  digitalWrite(ULTRASONIC_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG, LOW);
  
  // Read echo duration with timeout
  return pulseIn(ULTRASONIC_ECHO, HIGH, 60000); // 60ms timeout
}

long measureDistance() {
  long duration = getRawDuration();
  
  if (duration == 0) {
    return 400; // Timeout - return max range
  }
  
  // Calculate distance (speed of sound = 343 m/s)
  long distance = (duration * 0.0343) / 2;
  
  // Constrain to valid range
  if (distance < 2) return 2;
  if (distance > 400) return 400;
  
  return distance;
}
