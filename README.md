# 🚀 MARS-SENTINEL Setup & Usage Guide
*Portable Astronaut Safety & Habitat Monitoring System*

## Hardware Setup

### Required Components
- Arduino UNO/Nano
- DHT11 Temperature & Humidity Sensor
- MQ-2 or MQ-135 Gas Sensor
- HC-SR04 Ultrasonic Distance Sensor
- IR Proximity Sensor
- LDR (Light Dependent Resistor) + 10kΩ resistor
- Buzzer (Active or Passive)
- 2x LEDs (Red & Green) + 220Ω resistors
- Breadboard and jumper wires

### Wiring Diagram
```
Arduino Pin | Component
------------|----------
A0          | LDR (with 10kΩ resistor to GND)
A1          | Gas Sensor (MQ-2/MQ-135) analog output
A2          | LM35 Temperature Sensor (if not using DHT11)
D4          | DHT11 Data Pin
D5          | IR Proximity Sensor Output
D6          | Ultrasonic Trigger Pin
D7          | Ultrasonic Echo Pin
D9          | Buzzer Positive
D10         | Red LED Anode (+ 220Ω resistor)
D11         | Green LED Anode (+ 220Ω resistor)
5V          | Power for sensors
GND         | Common ground
```

## Software Setup

### 1. Install Python Dependencies
```bash
pip install -r requirements.txt
```

### 2. Upload Arduino Firmware
1. Open `mars_sentinel_firmware.ino` in Arduino IDE
2. Install DHT sensor library: Tools → Manage Libraries → Search "DHT sensor library"
3. Select your Arduino board and COM port
4. Upload the sketch

### 3. Configure COM Port
Edit `app.py` line 14 to match your Arduino's COM port:
```python
SERIAL_PORT = 'COM4'  # Windows: COM3, COM4, etc.
                      # Linux/Mac: /dev/ttyUSB0 or /dev/ttyACM0
```

## Running the System

### 1. Start the Backend Server
```bash
python app.py
```

### 2. Open Dashboard
Navigate to: http://localhost:5000

## Demo Script for Judges

### Safety Demo Sequence
1. **System Startup**: Show green LED and "Connected" status on dashboard
2. **Temperature Demo**: Touch DHT11 sensor to trigger heat alarm
3. **Gas Demo**: Breathe on MQ sensor to simulate contamination
4. **Obstacle Demo**: Wave hand in front of ultrasonic sensor
5. **Edge Detection**: Trigger IR sensor to simulate fall risk
6. **Mission Mode**: Switch between EVA, Mars, Emergency modes
7. **Data Export**: Download CSV log of all events

### Expected Behaviors
- **GREEN LED**: All systems normal
- **RED LED + Buzzer**: Any danger condition detected
- **Dashboard Updates**: Real-time sensor values and alarms
- **Event Logging**: All warnings and dangers logged with timestamps

## API Endpoints

| Endpoint | Method | Description |
|----------|--------|-------------|
| `/` | GET | Dashboard HTML |
| `/api/status` | GET | System connection status |
| `/api/thresholds` | GET/POST | View/update sensor thresholds |
| `/api/mission_mode` | GET/POST | View/change mission profile |
| `/api/events` | GET | Recent sensor events |
| `/api/export` | GET | Download CSV data export |

## Serial Commands (Arduino)

Send these commands via Serial Monitor or programmatically:

| Command | Description |
|---------|-------------|
| `SET TEMP_HIGH 50` | Set temperature danger threshold |
| `SET GAS_WARN 400` | Set gas warning threshold |
| `STATUS` | Get current system status |
| `THRESHOLDS` | Show all current thresholds |
| `HELP` | List all available commands |

## Mission Profiles

### EVA Mode (Default)
- Temperature Danger: 45°C
- Gas Danger: 600 ppm
- Distance Danger: 20 cm

### Mars Mode
- Temperature Danger: 40°C (stricter for harsh environment)
- Gas Danger: 500 ppm
- Distance Danger: 25 cm

### Emergency Mode
- Temperature Danger: 50°C (relaxed for emergencies)
- Gas Danger: 400 ppm (more sensitive)
- Distance Danger: 15 cm (very close proximity alerts)

### Training Mode
- Temperature Danger: 60°C (safe for training)
- Gas Danger: 800 ppm
- Distance Danger: 10 cm

## Troubleshooting

### "Failed to connect" error
1. Check COM port in `app.py`
2. Verify Arduino is plugged in and firmware uploaded
3. Close Arduino IDE Serial Monitor if open
4. Try different COM port

### Dashboard shows "Disconnected"
1. Restart Python server
2. Check browser console for errors
3. Verify Flask-SocketIO is running

### Sensors showing incorrect values
1. Check wiring connections
2. Verify sensor power (5V for most sensors)
3. Use Arduino Serial Monitor to debug raw values

### No alarms triggering
1. Check LED wiring and polarity
2. Verify buzzer connections
3. Test with manual threshold adjustments

## Extension Ideas (Stretch Goals)

- **Fall Detection**: Detect no movement for extended periods
- **3D Printed Enclosure**: Make it truly wearable
- **Wireless Communication**: ESP32 with WiFi instead of USB serial
- **Mobile App**: React Native companion app
- **Machine Learning**: Predictive failure detection
- **Multiple Astronauts**: Support for team monitoring

---

**Note**: MQ gas sensors are proxies for more sophisticated O₂/CO₂ monitoring that would be used in actual space applications. This system demonstrates the integration concept for educational purposes.
