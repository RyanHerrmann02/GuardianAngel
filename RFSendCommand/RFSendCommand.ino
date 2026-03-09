#include <SPI.h>
#include <RH_RF95.h>

// Define Read Pin
#define POT_PIN A0

// Define Commands for sending
#define CMD_RED     1
#define CMD_YELLOW  2
#define CMD_GREEN   3

// Declare the last command  for spam checking
uint8_t lastCommand = 0;

// Define LoRa Settings
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2

RH_RF95 rf95(RFM95_CS, RFM95_INT);

void setup() {
  // put your setup code here, to run once:
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  Serial.begin(9600);
  delay(100);

  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  if (!rf95.init()) {
    Serial.println("LoRa init failed");
    while (1);
  }

  rf95.setFrequency(915.0); // US frequency
  rf95.setTxPower(23, false);

}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t command = getCommandFromPot();
  sendCommandIfChanged(command);
  delay(500);
}

uint8_t getCommandFromPot()
{
  int potValue = analogRead(POT_PIN);

  if (potValue < 341)
    return CMD_GREEN;
  else if (potValue < 682)
    return CMD_YELLOW;
  else
    return CMD_RED;
}

void sendCommandIfChanged(uint8_t command)
{
  if (command != lastCommand)
  {
    rf95.send(&command, sizeof(command));
    rf95.waitPacketSent();
    lastCommand = command;
  }

  Serial.print("Sent: ");
  Serial.println(command);
}

void sendMessage()
{
  const char *msg = "TEST MESSAGE";
  Serial.println("Sending: TEST MESSAGE");
  rf95.send((uint8_t *)msg, strlen(msg));
  rf95.waitPacketSent();
  Serial.println("Message sent!");
  delay(2000);
}
