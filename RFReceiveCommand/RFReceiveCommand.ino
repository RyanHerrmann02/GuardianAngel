#include <SPI.h>
#include <RH_RF95.h>
#include <LiquidCrystal.h>

//Set Analog Pins to output
#define RED A0
#define YELLOW A1
#define GREEN A2

// LoRa Pins
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup()
{
  // Analog Pin Setup
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);
  
  // LCD setup
  lcd.begin(16, 2);
  printSetup();

  // LoRa reset
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  delay(100);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    lcd.clear();
    lcd.print("LoRa Failed");
    while (1);
  }

  if (!rf95.setFrequency(RF95_FREQ)) {
    lcd.clear();
    lcd.print("Freq Failed");
    while (1);
  }

  lcd.clear();
  lcd.print("LoRa Ready");
}

void loop()
{
  if (rf95.available())
  {
    //charMessage();
    intMessage();
  }
}

void charMessage()
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      buf[len] = '\0';   // Null terminate safely

      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Received:");

      lcd.setCursor(0,1);
      lcd.print((char*)buf);
    }
}

void intMessage()
{
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

    if (rf95.recv(buf, &len))
    {
      if (len == 1)   // Make sure we only received 1 byte
      {
        uint8_t receivedValue = buf[0];

        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Received:");

        lcd.setCursor(0,1);
        lcd.print(writeColor(receivedValue));

        controlLEDs(receivedValue);
      }
    }
}

void controlLEDs(int value)
{
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(GREEN, LOW);

  if (value == 1)
    digitalWrite(RED, HIGH);
  else if (value == 2)
    digitalWrite(YELLOW, HIGH);
  else if (value == 3)
    digitalWrite(GREEN, HIGH);
}

const char* writeColor(uint8_t value)
{
  switch(value)
  {
    case 1: return "RED";
    case 2: return "YELLOW";
    case 3: return "GREEN";
    default: return "UNKNOWN";
  }
}



void printSetup()
{
  lcd.print("Starting...");
  delay(1000);
  for (int i = 3; i>0; i--)
  {
    lcd.clear();
    lcd.print(i);
    delay(1000);
  }
}