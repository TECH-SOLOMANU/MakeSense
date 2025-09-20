"""
Simple API Test for MARS-SENTINEL
Tests if the REST API is working properly
"""

import requests
import json
import time

def test_api():
    print("🧪 Testing MARS-SENTINEL API...")
    
    base_url = "http://localhost:5000"
    
    try:
        # Test status endpoint
        print("1. Testing /api/status...")
        response = requests.get(f"{base_url}/api/status", timeout=5)
        print(f"   Status: {response.status_code}")
        if response.status_code == 200:
            print(f"   Data: {response.json()}")
        
        # Test events endpoint
        print("\n2. Testing /api/events...")
        response = requests.get(f"{base_url}/api/events?limit=3", timeout=5)
        print(f"   Status: {response.status_code}")
        if response.status_code == 200:
            events = response.json()
            print(f"   Events received: {len(events)}")
            if events:
                latest = events[-1]
                print(f"   Latest: Gas={latest.get('gas_level')}ppm, Dist={latest.get('distance')}cm, Status={latest.get('status')}")
        
        # Test thresholds endpoint
        print("\n3. Testing /api/thresholds...")
        response = requests.get(f"{base_url}/api/thresholds", timeout=5)
        print(f"   Status: {response.status_code}")
        if response.status_code == 200:
            print(f"   Thresholds: {response.json()}")
        
        print("\n✅ API is working correctly!")
        print("🌐 Dashboard should update at: http://localhost:5000/polling")
        
    except requests.exceptions.ConnectionError:
        print("❌ Cannot connect to server. Make sure app.py is running.")
    except requests.exceptions.Timeout:
        print("❌ Request timeout. Server might be overloaded.")
    except Exception as e:
        print(f"❌ Unexpected error: {e}")

if __name__ == "__main__":
    test_api()
