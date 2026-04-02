/*-----------------------------------------------------
  This is the final test of if the RTC pull down will 
  work. This will have most of the same pin
  configurations as the final GA Car Command code,
  however it will not include the UI input and rather 
  wait until the user presses the kill button to start
  a 2 minute timer to then figure out if the SQW pin
  is pulled low.
-----------------------------------------------------*/
#include <RTClib.h>

// Define Kill Pin and buttons
#define BTN_KILL A3
#define KILL_SWITCH 4
#define RED 5
#define GREEN 6

// Declare RTC breakout
RTC_DS3231 rtc;

// Declare sleep time as 2 minutes.
int minutes = 2;

// Declare DateTime formats to use
DateTime startTime;
DateTime targetTime;

// Setup and start the timer for alarm
void RTCSetup()
{
  // Start the RTC Breakout
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // Setting arbitrary start time to start at

  // Create start and end times for the breakout
  startTime = rtc.now();
  targetTime = startTime + TimeSpan(0, 0, minutes, 0); //To be set by adjusting on display

  // Clear alarms
  rtc.clearAlarm(1);
  
  // Disable second alarm
  rtc.disableAlarm(2);
}

// Sets the alarm and turns off power to main arduino board.
void armAlarm() 
{
  while(1)
  {
    if (digitalRead(BTN_KILL) == LOW)
    {
      delay(10);  //Debounce
      break;
    }
  }
  
  RTCSetup();

  rtc.setAlarm1(targetTime, DS3231_A1_Date);
  delay(100);

  digitalWrite(KILL_SWITCH, LOW);
}

void setup() {

  // For every start, check alarm and make kill pin high.
  pinMode(KILL_SWITCH, OUTPUT);
  digitalWrite(KILL_SWITCH, HIGH);

  // Let pin hold on before next method
  delay(100);
  rtc.begin();

  // Check alarm for going off to decide steps
  if (rtc.alarmFired(1))
  {
    rtc.clearAlarm(1);
    pinMode(RED, OUTPUT);
    digitalWrite(RED, HIGH);
  }
  else
  {
    pinMode(BTN_KILL, INPUT_PULLUP);
    pinMode(GREEN, OUTPUT);
    digitalWrite(GREEN, HIGH);
    armAlarm();
  }

}

void loop() {
  // put your main code here, to run repeatedly:

}
