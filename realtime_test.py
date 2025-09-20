"""
Minimal MARS-SENTINEL Test - Debug Version
Tests real-time connection to Arduino and processes the data
"""

import serial
import time
import json

def test_realtime_connection():
    print("🚀 MARS-SENTINEL Real-Time Test")
    print("=" * 40)
    
    try:
        # Connect to Arduino
        print("🔌 Connecting to COM3...")
        ser = serial.Serial('COM3', 9600, timeout=1)
        print("✅ Connected successfully!")
        
        # Wait for Arduino to initialize
        print("⏳ Waiting 2 seconds for Arduino to initialize...")
        time.sleep(2)
        
        print("📡 Reading real-time sensor data...")
        print("Press Ctrl+C to stop")
        print("-" * 40)
        
        event_count = 0
        
        while True:
            try:
                # Read data from Arduino
                line = ser.readline().decode('utf-8').strip()
                if not line:
                    continue
                
                # Parse the data: timestamp,temp,gas,ir,distance
                parts = line.split(',')
                if len(parts) != 5:
                    print(f"⚠️ Invalid data format: {line}")
                    continue
                
                timestamp_ms, temp_str, gas_str, ir_str, dist_str = parts
                
                # Convert to appropriate types
                temperature = float(temp_str) if float(temp_str) > -50 else 20.0  # Use 20°C default if invalid
                gas_level = int(gas_str)
                ir_detection = int(ir_str)
                distance = int(dist_str)
                
                # Determine status based on simple thresholds
                status = "OK"
                alarms = []
                
                if temperature > 45:
                    status = "DANGER"
                    alarms.append("Temperature Critical")
                elif temperature > 35:
                    status = "WARN"
                    alarms.append("Temperature Warning")
                
                if gas_level > 600:
                    status = "DANGER"
                    alarms.append("Gas Contamination Critical")
                elif gas_level > 300:
                    if status != "DANGER":
                        status = "WARN"
                    alarms.append("Gas Contamination Warning")
                
                if distance < 20:
                    status = "DANGER"
                    alarms.append("Obstacle Too Close")
                elif distance < 50:
                    if status != "DANGER":
                        status = "WARN"
                    alarms.append("Obstacle Warning")
                
                if ir_detection >= 1:
                    status = "DANGER"
                    alarms.append("Edge/Fall Risk Detected")
                
                # Create sensor data
                sensor_data = {
                    'event': event_count,
                    'timestamp': int(time.time() * 1000),
                    'temperature': round(temperature, 1),
                    'humidity': 50.0,  # Default humidity
                    'gas_level': gas_level,
                    'ir_detection': ir_detection,
                    'distance': distance,
                    'status': status,
                    'alarms': alarms
                }
                
                # Display formatted data
                status_icon = "🟢" if status == "OK" else "🟡" if status == "WARN" else "🔴"
                print(f"{status_icon} Event #{event_count:3d} | "
                      f"Temp: {temperature:5.1f}°C | "
                      f"Gas: {gas_level:3d} ppm | "
                      f"Dist: {distance:3d} cm | "
                      f"IR: {ir_detection} | "
                      f"Status: {status}")
                
                if alarms:
                    print(f"    🚨 ALARMS: {', '.join(alarms)}")
                
                event_count += 1
                
                # Add a small delay to make it readable
                time.sleep(0.1)
                
            except KeyboardInterrupt:
                print("\n🛑 Stopping real-time test...")
                break
            except Exception as e:
                print(f"❌ Error processing data: {e}")
                print(f"Raw data: {line}")
        
        ser.close()
        print("✅ Test completed successfully!")
        print(f"📊 Total events processed: {event_count}")
        
    except serial.SerialException as e:
        print(f"❌ Serial connection error: {e}")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")

if __name__ == "__main__":
    test_realtime_connection()
