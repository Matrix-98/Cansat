# CanSat Project: LoRa-based Telemetry System

A compact CanSat (can-sized satellite) project that transmits real-time sensor data using LoRa communication. This system collects environmental and motion data and transmits it wirelessly to a ground station.

## PROJECT OVERVIEW

This CanSat system simulates a real satellite mission by:
- Collecting temperature, humidity, and acceleration data
- Transmitting data via long-range LoRa communication
- Receiving and displaying data on a ground station
- Operating within a compact soda-can-sized enclosure

## HARDWARE COMPONENTS

### Transmitter (CanSat)
- Microcontroller: Arduino Nano
- Communication: LoRa module (SX1278)
- Sensors:
  - AHT10 Temperature & Humidity Sensor
  - ADXL345 3-Axis Accelerometer
- Power: 3.7V LiPo battery

### Receiver (Ground Station)
- Microcontroller: Arduino Nano (or compatible)
- Communication: LoRa module (SX1278)
- Interface: Serial monitor or display

## REPOSITORY STRUCTURE

cansat-lora-telemetry/
├── firmware/
│   ├── transmitter/
│   │   └── transmitter.ino
│   └── receiver/
│       └── receiver.ino
├── docs/
│   ├── wiring_diagrams/
│   └── datasheets/
├── hardware/
│   ├── pcb/
│   └── enclosure/
└── README.md

## PIN CONNECTIONS

### Transmitter Wiring
Component    | Arduino Nano Pin
-------------|------------------
LoRa SS      | D10
LoRa RST     | D9
LoRa DIO0    | D2
AHT10        | I2C (A4, A5)
ADXL345      | I2C (A4, A5)

### Receiver Wiring
Component    | Arduino Nano Pin
-------------|------------------
LoRa SS      | D10
LoRa RST     | D9
LoRa DIO0    | D2

## INSTALLATION & SETUP

### Prerequisites
- Arduino IDE 1.8.x or newer
- Required libraries:
  - LoRa by Sandeep Mistry
  - Adafruit AHT10 library
  - DFRobot ADXL345 library

### Library Installation
1. Open Arduino IDE
2. Go to Sketch → Include Library → Manage Libraries
3. Search and install:
   - LoRa by Sandeep Mistry
   - Adafruit AHT10 by Adafruit
   - DFRobot ADXL345 by DFRobot

### Flashing the Firmware

1. Transmitter Setup:
   - Connect Arduino Nano to computer
   - Open transmitter.ino from firmware/transmitter/
   - Select correct board and port
   - Upload the sketch

2. Receiver Setup:
   - Connect second Arduino Nano to computer
   - Open receiver.ino from firmware/receiver/
   - Select correct board and port
   - Upload the sketch
   - Open Serial Monitor at 9600 baud to view data

## DATA FORMAT

The system transmits data in CSV format:
temperature,humidity,accel_x,accel_y,accel_z

Example Transmission:
23.45,65.20,124,-38,1024

## CONFIGURATION

### LoRa Parameters
- Frequency: 433 MHz
- Spreading Factor: 7 (default)
- Signal Bandwidth: 125 kHz (default)
- Coding Rate: 4/5 (default)

### Transmission Interval
- Default: 5 seconds (adjustable in transmitter.ino)
- Modify delay(5000) for different intervals

## OPERATION

### Launch Sequence
1. Power on the CanSat transmitter
2. Start the ground station receiver
3. Monitor serial output for data reception
4. Observe RSSI values for signal strength

### Data Interpretation
- Temperature: Degrees Celsius (°C)
- Humidity: Relative humidity (%)
- Acceleration: Raw values from ADXL345
- RSSI: Received Signal Strength Indicator (dBm)

## TROUBLESHOOTING

### Common Issues

1. LoRa Initialization Failed
   - Check pin connections
   - Verify antenna connection
   - Ensure 433MHz modules match regional regulations

2. Sensor Detection Issues
   - Verify I2C connections
   - Check sensor addresses
   - Ensure proper power supply

3. No Data Received
   - Check receiver wiring
   - Verify both devices on same frequency
   - Monitor RSSI for signal presence

### Debugging Tips
- Use Serial monitor for status messages
- Check power supply stability
- Verify library versions match
- Test in open areas for better range

## EXPECTED PERFORMANCE

- Range: 1-2 km (line of sight)
- Battery Life: 2-4 hours (depending on transmission interval)
- Data Accuracy:
  - Temperature: ±0.3°C
  - Humidity: ±2%
  - Acceleration: ±2g range

## CUSTOMIZATION

### Adding Sensors
1. Connect new sensor to I2C or digital pins
2. Add library includes in transmitter
3. Read sensor data in loop()
4. Append to data string

### Changing Transmission Rate
Modify the delay in transmitter.ino:
delay(5000); // Change 5000 to desired milliseconds

## FIRMWARE CODE

=== TRANSMITTER CODE (transmitter.ino) ===
#include <Wire.h>
#include <SPI.h>
#include <LoRa.h>
#include <Adafruit_AHT10.h>
#include <DFRobot_ADXL345.h>

// Sensor instances
Adafruit_AHT10 aht;
DFRobot_ADXL345_I2C ADXL345(&Wire, 0x53); // I2C address

// LoRa pins (defaults are OK unless changed)
#define SS_PIN     10
#define RST_PIN     9
#define DIO0_PIN    2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  // AHT10 init
  if (!aht.begin()) {
    Serial.println("Could not find AHT10! Check wiring.");
    while (1);
  }
  Serial.println("✅ AHT10 initialized");

  // ADXL345 init
  Serial.println("Initializing ADXL345...");
  ADXL345.begin();
  ADXL345.powerOn();
  Serial.println("✅ ADXL345 initialized");

  // LoRa init
  LoRa.setPins(SS_PIN, RST_PIN, DIO0_PIN);
  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa init failed! Check wiring.");
    while (1);
  }
  Serial.println("✅ LoRa initialized @ 433MHz");
}

void loop() {
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);

  int accval[3];
  ADXL345.readAccel(accval);

  // Create CSV-style data string
  String data = "";
  data += String(temp.temperature, 2) + ",";
  data += String(humidity.relative_humidity, 2) + ",";
  data += String(accval[0]) + ",";
  data += String(accval[1]) + ",";
  data += String(accval[2]);

  // Send via LoRa
  LoRa.beginPacket();
  LoRa.print(data);
  LoRa.endPacket();

  Serial.print("📤 Sent: ");
  Serial.println(data);

  delay(5000); // Wait 5 seconds
}

=== RECEIVER CODE (receiver.ino) ===
#include <SPI.h>
#include <LoRa.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);

  Serial.println("🔌 LoRa Receiver Starting...");

  if (!LoRa.begin(433E6)) {
    Serial.println("❌ LoRa init failed. Check wiring or frequency.");
    while (true);
  }

  Serial.println("✅ LoRa initialized @ 433MHz");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String received = "";

    while (LoRa.available()) {
      received += (char)LoRa.read();
    }

    Serial.println("📥 Received packet:");
    Serial.println(received);
    Serial.print("📶 RSSI: ");
    Serial.println(LoRa.packetRssi());

    // Parsing the CSV values manually
    float temp = 0, hum = 0;
    int ax = 0, ay = 0, az = 0;

    int c1 = received.indexOf(',');
    int c2 = received.indexOf(',', c1 + 1);
    int c3 = received.indexOf(',', c2 + 1);
    int c4 = received.indexOf(',', c3 + 1);

    if (c1 > 0 && c2 > 0 && c3 > 0 && c4 > 0) {
      String tempStr = received.substring(0, c1);
      String humStr  = received.substring(c1 + 1, c2);
      String xStr    = received.substring(c2 + 1, c3);
      String yStr    = received.substring(c3 + 1, c4);
      String zStr    = received.substring(c4 + 1);

      temp = tempStr.toFloat();
      hum  = humStr.toFloat();
      ax   = xStr.toInt();
      ay   = yStr.toInt();
      az   = zStr.toInt();

      Serial.println("✅ Parsed Sensor Data:");
      Serial.print("🌡 Temperature: "); Serial.print(temp); Serial.println(" °C");
      Serial.print("💧 Humidity:    "); Serial.print(hum); Serial.println(" %");
      Serial.print("📈 Accel X:     "); Serial.println(ax);
      Serial.print("📈 Accel Y:     "); Serial.println(ay);
      Serial.print("📈 Accel Z:     "); Serial.println(az);
    } else {
      Serial.println("⚠️ Failed to parse full sensor data.");
    }

    Serial.println("-----------------------------\n");
  }
}

## LICENSE

MIT License

Copyright (c) [2025] [Afm Abdur Rahman]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## SUPPORT

For support and questions:
- Open an issue on GitHub
- Check Arduino forum for LoRa projects
- Review sensor datasheets for technical details

## ACKNOWLEDGMENTS

- LoRa library by Sandeep Mistry
- Adafruit for AHT10 library
- DFRobot for ADXL345 library
- Arduino community for continuous support

---
Note: Ensure compliance with local regulations when using 433MHz LoRa modules. 
Transmission power and frequency restrictions may apply in your region.
