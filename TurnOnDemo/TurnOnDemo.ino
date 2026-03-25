#include <RTClib.h>

RTC_DS3231 rtc;

// Declare Kill Pin
#define KILLPIN 6

int days = 0, hours = 0, minutes = 0, seconds = 5;

// Declare DateTime formats to use
DateTime startTime;
DateTime targetTime;

//Timerdone Bool
bool alarm = false;

void pinSetup()
{
  pinMode(KILLPIN, OUTPUT);
  digitalWrite(KILLPIN, LOW);
}

void RTCSetup()
{
  // Start the RTC Breakout
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // Setting arbitrary start time to start at

  // Create start and end times for the breakout
  startTime = rtc.now();
  targetTime = startTime + TimeSpan(days, hours, minutes, seconds); //To be set by adjusting on display

  // Clear alarms
  rtc.clearAlarm(1);
  Serial.print("Cleared Alarm: Alarm is --> ");
  Serial.println(alarm);
  
  // Disable second alarm
  rtc.disableAlarm(2);
}

void sleepAlarm()
{
  // Set alarm to fire pin
  rtc.setAlarm1(targetTime, DS3231_A1_Second);

  Serial.println("Starting!");
  
  delay(200);
  
  // Kill the PMOS Using the pin to high
  digitalWrite(KILLPIN, HIGH);
}

void setup() {
  // Will check if alarm went off for turning on arduino
  Serial.begin(9600);
  Serial.println("TEST");

  rtc.begin();
  alarm = rtc.alarmFired(1);

  Serial.println(alarm);
  pinSetup();
  delay(100);
  if (alarm)
  {
    rtc.clearAlarm(1);
    Serial.println("Done");
    digitalWrite(7, HIGH);
    while(1);
  }
  else
  {
    Serial.println("SETUP");
    RTCSetup();
    delay(100);
    sleepAlarm();
    delay(1000);
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}


