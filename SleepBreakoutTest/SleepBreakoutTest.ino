#include <RTClib.h>

RTC_DS3231 rtc;

// Declare days, hours, minutes, seconds to sleep
int days = 0, hours = 0, minutes = 0, seconds = 30;

// Declare DateTime formats to use
DateTime startTime;
DateTime targetTime;

//Timerdone Bool
bool timerDone = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  // Start the RTC Breakout
  rtc.begin();
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // Setting arbitrary start time to start at

  // Create start and end times for the breakout
  startTime = rtc.now();
  targetTime = startTime + TimeSpan(days, hours, minutes, seconds); //To be set by adjusting on display

  // Create the alarm function

  // Clear alarms
  rtc.clearAlarm(1);
  
  // Disable second alarm
  rtc.disableAlarm(2);

  // Set alarm to fire pin
  rtc.setAlarm1(targetTime, DS3231_A1_Second);

  Serial.println("Start");
}

void loop() {
  // Check for alarm firing
  if (rtc.alarmFired(1))
  {
    timerDone = true; // make this timer turn on
    //rtc.clearAlarm(1);
  }

  if(timerDone)
  {
    Serial.println("Done");
  }

}
