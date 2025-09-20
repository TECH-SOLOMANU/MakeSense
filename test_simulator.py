"""
MARS-SENTINEL Test Simulator
Simulates Arduino sensor data for testing the dashboard without hardware
"""

import serial
import time
import random
from serial.tools import list_ports

def find_virtual_port():
    """Find available virtual/loopback ports"""
    ports = list_ports.comports()
    for port in ports:
        if 'Virtual' in port.description or 'loopback' in port.description.lower():
            return port.device
    return None

def simulate_sensor_data():
    """Generate realistic sensor data for testing"""
    # Base values
    temp = 22.5 + random.uniform(-2, 8)  # 20.5-30.5°C
    humidity = 45 + random.uniform(-10, 25)  # 35-70%
    gas = random.randint(200, 400)  # Normal air quality
    ir = random.choice([0, 0, 0, 1])  # Mostly clear, occasionally detect edge
    distance = random.randint(30, 100)  # 30-100cm distance
    
    # Occasionally simulate danger conditions
    if random.random() < 0.05:  # 5% chance of danger
        danger_type = random.choice(['temp', 'gas', 'distance'])
        if danger_type == 'temp':
            temp = 47 + random.uniform(0, 8)  # Dangerous temperature
        elif danger_type == 'gas':
            gas = random.randint(650, 900)  # High gas levels
        elif danger_type == 'distance':
            distance = random.randint(5, 18)  # Too close to obstacle
    
    timestamp = int(time.time() * 1000)
    return f"{timestamp},{temp:.2f},{humidity:.2f},{gas},{ir},{distance}"

def main():
    print("🧪 MARS-SENTINEL Test Simulator")
    print("This simulates Arduino serial output for testing purposes")
    print()
    
    # Try to use virtual port, otherwise prompt for manual setup
    virtual_port = find_virtual_port()
    if virtual_port:
        print(f"Found virtual port: {virtual_port}")
        port = virtual_port
    else:
        print("No virtual COM port found.")
        print("For testing without Arduino:")
        print("1. Install com0com (Windows) or socat (Linux/Mac)")
        print("2. Create virtual COM port pair")
        print("3. Update SERIAL_PORT in app.py to match")
        print()
        port = input("Enter COM port to simulate (e.g., COM5): ").strip()
    
    try:
        # Open serial port for writing
        ser = serial.Serial(port, 9600, timeout=1)
        print(f"✓ Opened {port} for simulation")
        print("📡 Sending simulated sensor data...")
        print("Press Ctrl+C to stop")
        print()
        
        while True:
            # Generate and send sensor data
            data = simulate_sensor_data()
            ser.write(f"{data}\n".encode())
            print(f"Sent: {data}")
            
            time.sleep(0.2)  # 200ms interval as per PRD
            
    except serial.SerialException as e:
        print(f"❌ Serial error: {e}")
        print("Make sure the COM port exists and isn't in use by another application")
    except KeyboardInterrupt:
        print("\n🛑 Simulation stopped by user")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
    finally:
        if 'ser' in locals():
            ser.close()

if __name__ == "__main__":
    main()
