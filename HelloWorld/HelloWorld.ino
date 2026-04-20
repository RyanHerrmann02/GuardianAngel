#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {
  
  Serial.begin(9600);
  delay(1000);
  Wire.begin();
  
  Serial.println("Scanning...");
  for (byte addr = 1; addr < 127; addr++) {
    Serial.println(addr);
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found device at: 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Done.");
  // put your setup code here, to run once:
  lcd.begin(16,2);
  lcd.setBacklight(1);
  lcd.print("Hello World!");
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0, 1);
  lcd.print(millis() / 1000);
  //Serial.println(millis()/1000);
}
