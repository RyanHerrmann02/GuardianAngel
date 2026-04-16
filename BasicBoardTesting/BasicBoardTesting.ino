#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);

  Serial.println("Scanning I2C bus...");

  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    Serial.println("Checking...");
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at address 0x");
      Serial.println(address, HEX);
    }
  }

  Serial.println("Scan complete.");
}

void loop() {}