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
