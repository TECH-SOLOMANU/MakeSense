"""
MARS-SENTINEL Data Demonstration
Shows all the data handling capabilities of the system
"""

import json
import time
from datetime import datetime

def demonstrate_data_features():
    print("🚀 MARS-SENTINEL Data Capabilities Demonstration")
    print("=" * 60)
    
    # 1. Show data structure
    print("\n📊 1. SENSOR DATA STRUCTURE")
    print("-" * 30)
    
    sample_data = {
        'timestamp': int(time.time() * 1000),
        'temperature': 24.5,
        'humidity': 45.2,
        'gas_level': 320,
        'ir_detection': 0,
        'distance': 75,
        'status': 'OK',
        'alarms': [],
        'mode': 'eva',
        'connected': True
    }
    
    print("Sample sensor reading:")
    print(json.dumps(sample_data, indent=2))
    
    # 2. Show danger condition data
    print("\n🚨 2. DANGER CONDITION EXAMPLE")
    print("-" * 30)
    
    danger_data = {
        'timestamp': int(time.time() * 1000),
        'temperature': 47.8,  # High temperature
        'humidity': 45.2,
        'gas_level': 650,     # High gas levels
        'ir_detection': 1,    # Edge detected
        'distance': 15,       # Too close to obstacle
        'status': 'DANGER',
        'alarms': [
            'Temperature Critical',
            'Gas Contamination Critical', 
            'Edge/Fall Risk Detected',
            'Obstacle Too Close'
        ],
        'mode': 'eva',
        'connected': True
    }
    
    print("Danger condition data:")
    print(json.dumps(danger_data, indent=2))
    
    # 3. Show mission profiles
    print("\n🎯 3. MISSION PROFILE DATA")
    print("-" * 30)
    
    mission_configs = {
        'eva': {
            'description': 'Extra-Vehicular Activity',
            'temp_danger': 45, 
            'gas_danger': 600, 
            'distance_danger': 20
        },
        'mars': {
            'description': 'Mars Surface Operations',
            'temp_danger': 40, 
            'gas_danger': 500, 
            'distance_danger': 25
        },
        'emergency': {
            'description': 'Emergency Protocols',
            'temp_danger': 50, 
            'gas_danger': 400, 
            'distance_danger': 15
        },
        'training': {
            'description': 'Training Mode',
            'temp_danger': 60, 
            'gas_danger': 800, 
            'distance_danger': 10
        }
    }
    
    for mode, config in mission_configs.items():
        print(f"{mode.upper()}: {config['description']}")
        print(f"  - Temperature Limit: {config['temp_danger']}°C")
        print(f"  - Gas Limit: {config['gas_danger']} ppm")
        print(f"  - Distance Limit: {config['distance_danger']} cm")
    
    # 4. Show data export format
    print("\n📁 4. CSV EXPORT FORMAT")
    print("-" * 30)
    
    csv_header = "timestamp,temperature,humidity,gas_level,ir_detection,distance,status,alarms,mode"
    csv_sample = f"{int(time.time() * 1000)},24.5,45.2,320,0,75,OK,,eva"
    csv_danger = f"{int(time.time() * 1000)},47.8,45.2,650,1,15,DANGER,Temperature Critical|Gas Contamination Critical,eva"
    
    print("CSV Header:")
    print(csv_header)
    print("\nSample normal data:")
    print(csv_sample)
    print("\nSample danger data:")
    print(csv_danger)
    
    # 5. Show API endpoints for data access
    print("\n🌐 5. DATA ACCESS ENDPOINTS")
    print("-" * 30)
    
    endpoints = {
        '/api/status': 'System connection and health status',
        '/api/events': 'Recent sensor events (default: last 100)',
        '/api/events?limit=50': 'Last 50 sensor events',
        '/api/thresholds': 'Current safety thresholds',
        '/api/mission_mode': 'Current mission profile',
        '/api/export': 'Download complete data as CSV'
    }
    
    for endpoint, description in endpoints.items():
        print(f"GET {endpoint}")
        print(f"    {description}")
    
    # 6. Show real-time capabilities
    print("\n⚡ 6. REAL-TIME DATA FEATURES")
    print("-" * 30)
    
    realtime_features = [
        "WebSocket live updates (200ms sensor polling)",
        "Instant alarm notifications",
        "Live temperature trend charting", 
        "Real-time status badge updates",
        "Event log streaming",
        "Mission mode synchronization"
    ]
    
    for feature in realtime_features:
        print(f"✓ {feature}")
    
    # 7. Show data persistence
    print("\n💾 7. DATA PERSISTENCE & STORAGE")
    print("-" * 30)
    
    storage_info = [
        "In-memory storage of last 1000 events",
        "Automatic event pruning (FIFO)",
        "CSV export for long-term storage",
        "Event timestamps in milliseconds",
        "Status history preservation",
        "Alarm event logging"
    ]
    
    for info in storage_info:
        print(f"• {info}")
    
    print("\n" + "=" * 60)
    print("🎯 DEMO READY: All data features implemented and functional!")
    print("📊 Total data points per reading: 9 sensor values + metadata")
    print(f"⏱️  Data collection rate: Every 200ms (as per PRD)")
    print("🔄 Real-time updates via WebSocket to dashboard")
    print("💾 Export capability for post-mission analysis")

if __name__ == "__main__":
    demonstrate_data_features()
