"""
MARS-SENTINEL Data Analysis Example
Shows how to analyze exported mission data
"""

import csv
import json
from datetime import datetime
from collections import Counter

def analyze_mission_data(csv_file):
    """Analyze mission data from CSV export"""
    
    print("🔬 MARS-SENTINEL Mission Data Analysis")
    print("=" * 50)
    
    events = []
    status_counts = Counter()
    alarm_types = Counter()
    temp_readings = []
    gas_readings = []
    distance_readings = []
    
    # Read and parse CSV data
    with open(csv_file, 'r') as file:
        reader = csv.DictReader(file)
        for row in reader:
            events.append(row)
            status_counts[row['status']] += 1
            
            # Parse temperature
            if row['temperature']:
                temp_readings.append(float(row['temperature']))
            
            # Parse gas levels
            if row['gas_level']:
                gas_readings.append(int(row['gas_level']))
            
            # Parse distance
            if row['distance']:
                distance_readings.append(int(row['distance']))
            
            # Parse alarms
            if row['alarms']:
                alarms = row['alarms'].split('|')
                for alarm in alarms:
                    alarm_types[alarm.strip()] += 1
    
    # Analysis results
    total_events = len(events)
    mission_duration = (int(events[-1]['timestamp']) - int(events[0]['timestamp'])) / 1000  # seconds
    
    print(f"\n📊 MISSION OVERVIEW")
    print(f"Total Events: {total_events}")
    print(f"Mission Duration: {mission_duration:.1f} seconds")
    print(f"Data Rate: {total_events/mission_duration:.1f} events/second")
    
    print(f"\n🎯 STATUS DISTRIBUTION")
    for status, count in status_counts.items():
        percentage = (count / total_events) * 100
        print(f"{status:8}: {count:3} events ({percentage:.1f}%)")
    
    if temp_readings:
        print(f"\n🌡️ TEMPERATURE ANALYSIS")
        print(f"Min Temperature: {min(temp_readings):.1f}°C")
        print(f"Max Temperature: {max(temp_readings):.1f}°C")
        print(f"Avg Temperature: {sum(temp_readings)/len(temp_readings):.1f}°C")
        
        # Find temperature spikes
        temp_spikes = [t for t in temp_readings if t > 40]
        if temp_spikes:
            print(f"Temperature Spikes (>40°C): {len(temp_spikes)} readings")
            print(f"Peak Temperature: {max(temp_spikes):.1f}°C")
    
    if gas_readings:
        print(f"\n☣️ GAS LEVEL ANALYSIS")
        print(f"Min Gas Level: {min(gas_readings)} ppm")
        print(f"Max Gas Level: {max(gas_readings)} ppm")
        print(f"Avg Gas Level: {sum(gas_readings)/len(gas_readings):.1f} ppm")
        
        # Find contamination events
        contamination = [g for g in gas_readings if g > 500]
        if contamination:
            print(f"Contamination Events (>500ppm): {len(contamination)} readings")
            print(f"Peak Contamination: {max(contamination)} ppm")
    
    if distance_readings:
        print(f"\n📏 PROXIMITY ANALYSIS")
        print(f"Min Distance: {min(distance_readings)} cm")
        print(f"Max Distance: {max(distance_readings)} cm")
        print(f"Avg Distance: {sum(distance_readings)/len(distance_readings):.1f} cm")
        
        # Find close calls
        close_calls = [d for d in distance_readings if d < 20]
        if close_calls:
            print(f"Close Calls (<20cm): {len(close_calls)} readings")
            print(f"Closest Approach: {min(close_calls)} cm")
    
    if alarm_types:
        print(f"\n🚨 ALARM FREQUENCY")
        for alarm, count in alarm_types.most_common():
            print(f"{alarm}: {count} occurrences")
    
    # Safety score calculation
    danger_events = status_counts.get('DANGER', 0)
    warn_events = status_counts.get('WARN', 0)
    ok_events = status_counts.get('OK', 0)
    
    safety_score = (ok_events * 100 + warn_events * 50 + danger_events * 0) / total_events
    
    print(f"\n🛡️ MISSION SAFETY SCORE")
    print(f"Safety Score: {safety_score:.1f}/100")
    if safety_score >= 80:
        print("✅ EXCELLENT - Mission completed safely")
    elif safety_score >= 60:
        print("⚠️ GOOD - Some safety concerns noted")
    elif safety_score >= 40:
        print("🔶 FAIR - Multiple safety issues encountered")
    else:
        print("🚨 POOR - Significant safety risks during mission")
    
    # Timeline analysis
    print(f"\n⏱️ CRITICAL EVENTS TIMELINE")
    for i, event in enumerate(events):
        if event['status'] == 'DANGER':
            timestamp = int(event['timestamp'])
            time_offset = (timestamp - int(events[0]['timestamp'])) / 1000
            print(f"T+{time_offset:6.1f}s: DANGER - {event['alarms']}")
    
    return {
        'total_events': total_events,
        'mission_duration': mission_duration,
        'safety_score': safety_score,
        'status_counts': dict(status_counts),
        'alarm_types': dict(alarm_types),
        'temperature_stats': {
            'min': min(temp_readings) if temp_readings else 0,
            'max': max(temp_readings) if temp_readings else 0,
            'avg': sum(temp_readings)/len(temp_readings) if temp_readings else 0
        }
    }

def generate_mission_report(analysis_data):
    """Generate a JSON mission report"""
    
    report = {
        'mission_id': f"MARS-SENTINEL-{datetime.now().strftime('%Y%m%d-%H%M%S')}",
        'analysis_timestamp': datetime.now().isoformat(),
        'summary': analysis_data,
        'recommendations': []
    }
    
    # Add recommendations based on analysis
    if analysis_data['safety_score'] < 60:
        report['recommendations'].append("Review EVA procedures and safety protocols")
    
    if 'Temperature Critical' in analysis_data['alarm_types']:
        report['recommendations'].append("Check thermal management systems")
    
    if 'Gas Contamination Critical' in analysis_data['alarm_types']:
        report['recommendations'].append("Inspect life support and air filtration")
    
    if 'Obstacle Too Close' in analysis_data['alarm_types']:
        report['recommendations'].append("Improve navigation and proximity awareness")
    
    return report

if __name__ == "__main__":
    # Analyze the sample data
    analysis_results = analyze_mission_data('sample_mission_data.csv')
    
    # Generate mission report
    mission_report = generate_mission_report(analysis_results)
    
    # Save report as JSON
    with open('mission_report.json', 'w') as f:
        json.dump(mission_report, f, indent=2)
    
    print(f"\n📋 Mission report saved to: mission_report.json")
    print(f"🔬 Analysis complete!")
