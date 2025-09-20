"""
Quick Arduino Serial Test
Tests if the Arduino is sending data on COM3
"""

import serial
import time

def test_arduino_connection():
    print("🔍 Testing Arduino connection on COM3...")
    
    try:
        # Open serial connection
        ser = serial.Serial('COM3', 9600, timeout=2)
        print("✓ Serial port opened successfully")
        
        # Wait a moment for Arduino to initialize
        print("⏳ Waiting for Arduino to initialize...")
        time.sleep(2)
        
        # Try to read some data
        print("📡 Listening for data (10 seconds)...")
        start_time = time.time()
        data_received = False
        
        while time.time() - start_time < 10:
            if ser.in_waiting > 0:
                try:
                    line = ser.readline().decode('utf-8').strip()
                    if line:
                        print(f"📥 Received: {line}")
                        data_received = True
                except:
                    print("📥 Received non-text data")
                    data_received = True
            time.sleep(0.1)
        
        ser.close()
        
        if data_received:
            print("✅ Arduino is sending data!")
            print("💡 You can now start the MARS-SENTINEL server")
        else:
            print("❌ No data received from Arduino")
            print("💡 Please check:")
            print("   1. Arduino firmware is uploaded")
            print("   2. Sensors are connected properly")
            print("   3. Arduino is powered and reset")
            
    except serial.SerialException as e:
        print(f"❌ Serial connection failed: {e}")
        print("💡 Please check:")
        print("   1. Arduino is connected to COM3")
        print("   2. No other software is using the port")
        print("   3. Arduino drivers are installed")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")

if __name__ == "__main__":
    test_arduino_connection()
