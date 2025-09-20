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
app.config['SECRET_KEY'] = os.environ.get('SECRET_KEY', 'astronaut-safety-sensor-key-2025')
CORS(app)
socketio = SocketIO(app, cors_allowed_origins="*")

# Configuration - Update COM port as needed
SERIAL_PORT = 'COM3'  # Windows: COM3, Linux/Mac: /dev/ttyUSB0 or /dev/ttyACM0
BAUD_RATE = 9600
DEMO_MODE = False  # Set to True to run without Arduino hardware

# Global state
state = {
    'connected': False,
    'mode': 'eva',
    'thresholds': {
        'temp_warn': 35,
        'temp_danger': 45,
        'humidity_warn': 70,
        'humidity_danger': 85,
        'gas_warn': 300,
        'gas_danger': 600,
        'distance_warn': 50,
        'distance_danger': 20,
        'ir_danger': 1
    },
    'current_sensors': {},
    'events': []
}

# Mission mode configurations
MISSION_CONFIGS = {
    'eva': {'temp_danger': 45, 'gas_danger': 600, 'distance_danger': 20},
    'mars': {'temp_danger': 40, 'gas_danger': 500, 'distance_danger': 25},
    'emergency': {'temp_danger': 50, 'gas_danger': 400, 'distance_danger': 15},
    'training': {'temp_danger': 60, 'gas_danger': 800, 'distance_danger': 10}
}

def connect_serial():
    """Connect to Arduino serial port"""
    if DEMO_MODE:
        print("🎭 Running in DEMO MODE (no Arduino required)")
        state['connected'] = False
        return None
        
    max_retries = 3
    retry_count = 0
    
    while retry_count < max_retries:
        try:
            ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
            print(f"✓ Connected to Arduino on {SERIAL_PORT}")
            state['connected'] = True
            return ser
        except Exception as e:
            retry_count += 1
            print(f"✗ Failed to connect to {SERIAL_PORT}: {e}")
            if retry_count < max_retries:
                print(f"Retrying in 5 seconds... (Attempt {retry_count}/{max_retries})")
                state['connected'] = False
                time.sleep(5)
            else:
                print(f"❌ Could not connect after {max_retries} attempts")
                print("💡 Running in demo mode without hardware")
                state['connected'] = False
                return None

def process_sensor_data(timestamp, temp, humidity, gas, ir, distance):
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
    
    # Humidity checks
    if humidity > thresholds['humidity_danger']:
        status = 'DANGER'
        alarms.append('Humidity Critical')
    elif humidity > thresholds['humidity_warn']:
        status = 'WARN' if status != 'DANGER' else status
        alarms.append('Humidity Warning')
    
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

def generate_demo_data():
    """Generate simulated sensor data for demo purposes"""
    import random
    
    while True:
        try:
            # Generate realistic demo data
            temperature = 22.5 + random.uniform(-2, 8)  # 20.5-30.5°C
            humidity = 45 + random.uniform(-10, 25)  # 35-70%
            gas_level = random.randint(200, 400)  # Normal air quality
            ir_detection = random.choice([0, 0, 0, 1])  # Mostly clear
            distance = random.randint(30, 100)  # 30-100cm
            
            # Occasionally simulate danger conditions (5% chance)
            if random.random() < 0.05:
                danger_type = random.choice(['temp', 'gas', 'distance'])
                if danger_type == 'temp':
                    temperature = 47 + random.uniform(0, 8)
                elif danger_type == 'gas':
                    gas_level = random.randint(650, 900)
                elif danger_type == 'distance':
                    distance = random.randint(5, 18)
            
            # Process the simulated data
            status, alarms = process_sensor_data(
                int(time.time() * 1000), temperature, humidity, gas_level, ir_detection, distance
            )
            
            # Create sensor data packet
            sensor_data = {
                'timestamp': int(time.time() * 1000),
                'temperature': round(temperature, 2),
                'humidity': round(humidity, 2),
                'gas_level': gas_level,
                'ir_detection': ir_detection,
                'distance': distance,
                'status': status,
                'alarms': alarms,
                'mode': state['mode'],
                'connected': False  # Demo mode
            }
            
            # Update state
            state['current_sensors'] = sensor_data
            state['events'].append(sensor_data)
            
            # Keep only last 1000 events
            if len(state['events']) > 1000:
                state['events'] = state['events'][-1000:]
            
            # Emit to all connected clients
            socketio.emit('sensor_update', sensor_data)
            
            time.sleep(0.5)  # Slower update for demo
            
        except Exception as e:
            print(f"Demo data generation error: {e}")
            time.sleep(1)

def read_serial_loop():
    """Main serial reading loop"""
    if DEMO_MODE:
        print("🎭 Running in DEMO MODE - generating simulated sensor data")
        generate_demo_data()
        return
    
    print(f"🔌 Attempting to connect to {SERIAL_PORT} at {BAUD_RATE} baud...")
    
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"✅ Connected to Arduino on {SERIAL_PORT}")
        state['connected'] = True
        
        print("📡 Starting real-time data collection...")
        event_count = 0
        
        while True:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if not line:
                    continue
                
                # Skip header lines or non-numeric data
                if 'temp' in line.lower() or 'format:' in line.lower() or 'initialized' in line.lower() or 'debug:' in line.lower():
                    print(f"Arduino info: {line}")
                    continue
                
                # Parse CSV: timestamp,temp,humidity,gas,ir,distance (your Arduino format)
                parts = line.split(',')
                if len(parts) != 6:
                    continue
                
                timestamp_ms, temp_str, humidity_str, gas_str, ir_str, dist_str = parts
                
                # Convert to appropriate types with error handling
                try:
                    temperature = float(temp_str) if float(temp_str) > -50 else 22.0  # Use 22°C default if invalid
                    humidity = float(humidity_str) if float(humidity_str) >= 0 else 50.0  # Use real humidity or default
                    gas_level = int(gas_str)
                    ir_detection = int(ir_str)
                    distance = int(dist_str)
                except ValueError as conv_error:
                    print(f"Data conversion error: {conv_error} | Line: {line}")
                    continue
                
                # Process sensor data
                status, alarms = process_sensor_data(
                    timestamp_ms, temperature, humidity, gas_level, ir_detection, distance
                )
                
                # Create sensor data packet
                sensor_data = {
                    'timestamp': int(time.time() * 1000),
                    'temperature': round(temperature, 2),
                    'humidity': round(humidity, 2),
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
                
                # Emit to all connected clients (with error handling)
                try:
                    socketio.emit('sensor_update', sensor_data)
                except Exception as emit_error:
                    print(f"WebSocket emit error: {emit_error}")
                
                event_count += 1
                
                # Print status every 50 events
                if event_count % 50 == 0:
                    print(f"📊 Events: {event_count} | Status: {status} | Gas: {gas_level} ppm | Dist: {distance} cm")
                
            except KeyboardInterrupt:
                print("\n🛑 Stopping data collection...")
                break
            except Exception as e:
                print(f"Serial read error: {e}")
                time.sleep(0.1)
        
        ser.close()
        
    except serial.SerialException as e:
        print(f"❌ Serial connection failed: {e}")
        print("🎭 Starting demo mode with simulated data...")
        generate_demo_data()
    except Exception as e:
        print(f"❌ Unexpected error: {e}")
        print("🎭 Starting demo mode with simulated data...")
        generate_demo_data()# REST API Routes
@app.route('/')
def dashboard():
    """Serve the main dashboard"""
    return send_file('dashboard_polling.html')

@app.route('/polling')
def dashboard_polling():
    """Serve the polling-based dashboard"""
    return send_file('dashboard_polling.html')

@app.route('/ultra')
def ultra_dashboard():
    """Serve the ULTRA advanced dashboard"""
    return send_file('MARS_SENTINEL_ULTRA_UI.html')

@app.route('/final')
def final_dashboard():
    """Serve the FINAL production dashboard"""
    return send_file('dashboard_FINAL.html')

@app.route('/api/status')
def get_status():
    """Get current system status"""
    return jsonify({
        'connected': state['connected'],
        'mode': state['mode'],
        'sensor_count': len(state['events']),
        'last_update': state['current_sensors'].get('timestamp', 0)
    })

@app.route('/api/mission_mode', methods=['GET', 'POST'])
def mission_mode():
    """Get or set mission mode"""
    if request.method == 'POST':
        mode = request.json.get('mode')
        if mode in MISSION_CONFIGS:
            state['mode'] = mode
            # Update thresholds based on mission mode
            state['thresholds'].update(MISSION_CONFIGS[mode])
            return jsonify({'success': True, 'mode': mode, 'thresholds': state['thresholds']})
        else:
            return jsonify({'error': 'Invalid mission mode'}), 400
    else:
        return jsonify({'mode': state['mode']})

@app.route('/api/thresholds', methods=['GET', 'POST'])
def handle_thresholds():
    """Get or update sensor thresholds"""
    if request.method == 'POST':
        data = request.json
        for key, value in data.items():
            if key in state['thresholds']:
                state['thresholds'][key] = float(value)
        return jsonify(state['thresholds'])
    else:
        return jsonify(state['thresholds'])

@app.route('/api/events')
def get_events():
    """Get recent sensor events"""
    limit = request.args.get('limit', 100, type=int)
    return jsonify(state['events'][-limit:])

@app.route('/api/export')
def export_csv():
    """Export sensor data as CSV"""
    output = StringIO()
    writer = csv.writer(output)
    
    # Write header
    writer.writerow([
        'timestamp', 'temperature', 'humidity', 'gas_level', 
        'ir_detection', 'distance', 'status', 'alarms', 'mode'
    ])
    
    # Write data
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
    
    # Create response
    response = Response(
        output.getvalue(),
        mimetype='text/csv',
        headers={'Content-Disposition': 'attachment;filename=astronaut_sensor_log.csv'}
    )
    return response

# SocketIO event handlers
@socketio.on('connect')
def handle_connect():
    print('🌐 Dashboard client connected')
    # Send current status to new client
    if state['current_sensors']:
        try:
            socketio.emit('sensor_update', state['current_sensors'])
        except Exception as e:
            print(f"Error sending initial data: {e}")

@socketio.on('disconnect')
def handle_disconnect():
    print('🌐 Dashboard client disconnected')

if __name__ == '__main__':
    # Start serial reading thread
    serial_thread = threading.Thread(target=read_serial_loop, daemon=True)
    serial_thread.start()
    
    print("🚀 Astronaut Safety Sensor System Starting...")
    print(f"📡 Monitoring serial port: {SERIAL_PORT}")
    print(f"🌐 Dashboard will be available at: http://localhost:5000")
    
    # Run Flask-SocketIO server
    socketio.run(app, host='0.0.0.0', port=5000, debug=False)
