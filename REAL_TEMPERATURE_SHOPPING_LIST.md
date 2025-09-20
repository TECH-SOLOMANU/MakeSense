# 🌡️ MARS-SENTINEL Real Temperature Sensor Shopping List

## **Option 1: DHT11 (Basic - Recommended for beginners)**
- **What:** Digital temperature & humidity sensor
- **Accuracy:** ±2°C temperature, ±5% humidity
- **Cost:** $2-5 USD
- **Wiring:** 3 pins (VCC, GND, Data)
- **Where to buy:**
  - Amazon: "DHT11 temperature humidity sensor"
  - AliExpress: "DHT11 module"
  - Local electronics store

## **Option 2: DHT22 (Better accuracy)**
- **What:** Digital temperature & humidity sensor (upgraded)
- **Accuracy:** ±0.5°C temperature, ±2-5% humidity
- **Cost:** $3-8 USD
- **Wiring:** 3 pins (VCC, GND, Data)
- **Where to buy:**
  - Amazon: "DHT22 AM2302 sensor"
  - AliExpress: "DHT22 temperature sensor"

## **Option 3: DS18B20 (High precision temperature only)**
- **What:** Digital temperature sensor (no humidity)
- **Accuracy:** ±0.5°C
- **Cost:** $3-8 USD
- **Wiring:** 3 pins (VCC, GND, Data)
- **Special feature:** Waterproof versions available
- **Where to buy:**
  - Amazon: "DS18B20 temperature sensor"
  - AliExpress: "DS18B20 waterproof"

## **Option 4: LM35 (Analog - Simplest)**
- **What:** Analog temperature sensor
- **Accuracy:** ±1°C
- **Cost:** $1-3 USD
- **Wiring:** 3 pins (VCC, GND, Output)
- **Where to buy:**
  - Amazon: "LM35 temperature sensor"
  - Local electronics store

## **Quick Setup Instructions:**

### **DHT11/DHT22 (Recommended):**
```
DHT Sensor → Arduino
VCC/+ → 5V
GND/- → GND
Data/Signal → Pin D4
```

### **DS18B20:**
```
DS18B20 → Arduino
Red/VCC → 5V
Black/GND → GND
Yellow/Data → Pin D3
(Add 4.7kΩ resistor between VCC and Data)
```

### **LM35:**
```
LM35 → Arduino
VCC → 5V
GND → GND
Output → Pin A2
```

## **What happens after you connect:**

1. **Upload the new firmware** (`mars_sentinel_real_temp.ino`)
2. **The system automatically detects** which sensor you connected
3. **You get REAL temperature data** instead of simulated
4. **Your dashboard shows actual environmental conditions**

## **Current Status:**
- ✅ **Gas Sensor (A1):** Working perfectly with real data
- ❌ **Temperature:** Currently simulated (will be REAL once you add sensor)
- ❌ **Humidity:** Currently simulated (will be REAL with DHT11/DHT22)

## **Total Cost:** $2-8 USD for real temperature data!

The new firmware is ready and will automatically switch to real sensors as soon as you connect them. No code changes needed!
