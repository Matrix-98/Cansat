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
