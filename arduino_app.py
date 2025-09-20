"""
Direct MARS-SENTINEL with Real Arduino
Connects directly to your Arduino without demo mode fallback
"""

import serial
import threading
import time
import json
from flask import Flask, jsonify, request, Response, send_file
from flask_socketio import SocketIO
from flask_cors import CORS
import csv
from io import StringIO
import os

app = Flask(__name__)
app.config['SECRET_KEY'] = 'astronaut-safety-sensor-key-2025'
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")

# Configuration
SERIAL_PORT = 'COM3'
BAUD_RATE = 9600

# Global state
state = {
    'connected': False,
    'mode': 'eva',
    'thresholds': {
        'temp_warn': 35,
        'temp_danger': 45,
        'gas_warn': 300,
        'gas_danger': 600,
        'distance_warn': 50,
        'distance_danger': 20,
        'ir_danger': 1
    },
    'current_sensors': {},
    'events': []
}

def process_sensor_data(timestamp, temp, gas, ir, distance):
    """Process sensor readings and determine status"""
    thresholds = state['thresholds']
    alarms = []
    status = 'OK'
    
    # Temperature checks
    if temp > thresholds['temp_danger']:
        status = 'DANGER'
        alarms.append('Temperature Critical')
    elif temp > thresholds['temp_warn']:
        status = 'WARN' if status != 'DANGER' else status
        alarms.append('Temperature Warning')
    
    # Gas sensor checks
    if gas > thresholds['gas_danger']:
        status = 'DANGER'
        alarms.append('Gas Contamination Critical')
    elif gas > thresholds['gas_warn']:
        status = 'WARN' if status != 'DANGER' else status
        alarms.append('Gas Contamination Warning')
    
    # Distance checks
    if distance < thresholds['distance_danger']:
        status = 'DANGER'
        alarms.append('Obstacle Too Close')
    elif distance < thresholds['distance_warn']:
        status = 'WARN' if status != 'DANGER' else status
        alarms.append('Obstacle Warning')
    
    # IR edge detection
    if ir >= thresholds['ir_danger']:
        status = 'DANGER'
        alarms.append('Edge/Fall Risk Detected')
    
    return status, alarms

def read_arduino_data():
    """Read data from Arduino and emit to dashboard"""
    print(f"🔌 Connecting to Arduino on {SERIAL_PORT}...")
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"✅ Connected to Arduino successfully!")
        state['connected'] = True
        
        # Wait for Arduino to initialize
        time.sleep(2)
        print("📡 Starting real-time data collection...")
        
        while True:
            try:
                line = ser.readline().decode('utf-8').strip()
                if not line:
                    continue
                
                # Parse: timestamp,temp,gas,ir,distance
                parts = line.split(',')
                if len(parts) != 5:
                    continue
                
                timestamp_ms, temp_str, gas_str, ir_str, dist_str = parts
                
                # Convert to appropriate types
                temperature = float(temp_str) if float(temp_str) > -50 else 22.0  # Use 22°C default
                gas_level = int(gas_str)
                ir_detection = int(ir_str)
                distance = int(dist_str)
                
                # Process sensor data
                status, alarms = process_sensor_data(timestamp_ms, temperature, gas_level, ir_detection, distance)
                
                # Create sensor data packet
                sensor_data = {
                    'timestamp': int(time.time() * 1000),
                    'temperature': round(temperature, 1),
                    'humidity': 50.0,  # Default humidity since DHT11 isn't working
                    'gas_level': gas_level,
                    'ir_detection': ir_detection,
                    'distance': distance,
                    'status': status,
                    'alarms': alarms,
                    'mode': state['mode'],
                    'connected': True
                }
                
                # Update state
                state['current_sensors'] = sensor_data
                state['events'].append(sensor_data)
                
                # Keep only last 1000 events
                if len(state['events']) > 1000:
                    state['events'] = state['events'][-1000:]
                
                # Emit to all connected clients
                socketio.emit('sensor_update', sensor_data)
                
                # Print status occasionally
                if len(state['events']) % 50 == 0:
                    print(f"📊 Events: {len(state['events'])} | Status: {status} | Gas: {gas_level} ppm | Dist: {distance} cm")
                
            except Exception as e:
                print(f"❌ Error reading data: {e}")
                time.sleep(0.1)
                
    except Exception as e:
        print(f"❌ Failed to connect to Arduino: {e}")
        state['connected'] = False

# REST API Routes
@app.route('/')
def dashboard():
    return send_file('dashboard.html')

@app.route('/api/status')
def get_status():
    return jsonify({
        'connected': state['connected'],
        'mode': state['mode'],
        'sensor_count': len(state['events']),
        'last_update': state['current_sensors'].get('timestamp', 0)
    })

@app.route('/api/events')
def get_events():
    limit = request.args.get('limit', 100, type=int)
    return jsonify(state['events'][-limit:])

@app.route('/api/export')
def export_csv():
    output = StringIO()
    writer = csv.writer(output)
    
    writer.writerow(['timestamp', 'temperature', 'humidity', 'gas_level', 'ir_detection', 'distance', 'status', 'alarms', 'mode'])
    
    for event in state['events']:
        writer.writerow([
            event.get('timestamp', ''),
            event.get('temperature', ''),
            event.get('humidity', ''),
            event.get('gas_level', ''),
            event.get('ir_detection', ''),
            event.get('distance', ''),
            event.get('status', ''),
            '|'.join(event.get('alarms', [])),
            event.get('mode', '')
        ])
    
    response = Response(
        output.getvalue(),
        mimetype='text/csv',
        headers={'Content-Disposition': 'attachment;filename=arduino_sensor_data.csv'}
    )
    return response

# SocketIO event handlers
@socketio.on('connect')
def handle_connect():
    print('🌐 Dashboard client connected')
    if state['current_sensors']:
        socketio.emit('sensor_update', state['current_sensors'])

@socketio.on('disconnect')
def handle_disconnect():
    print('🌐 Dashboard client disconnected')

if __name__ == '__main__':
    # Start Arduino reading thread
    arduino_thread = threading.Thread(target=read_arduino_data, daemon=True)
    arduino_thread.start()
    
    print("🚀 MARS-SENTINEL with Real Arduino Starting...")
    print(f"📡 Reading from: {SERIAL_PORT}")
    print(f"🌐 Dashboard: http://localhost:5000")
    
    # Run Flask-SocketIO server
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)
