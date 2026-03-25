/*  To start, we need to implement 3 things:
    LCD Display, LoRa Integration, Deep Sleep 
    LCD Display will be using a shifter, but eventually
    that will be on its own breakout. LoRa will be used
    for the radio and that will work with deep sleep
    deep sleep will be used including a breakout allowing for 
    it to use ~0.1μA of power including an interupt from a LoRa
    signal. */

// Importing Necessary Libraries
#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>
#include <avr/sleep.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

//Define Lights
#define BEEPER 7
#define EMERGENCY_LIGHT_1 6
#define EMERGENCY_LIGHT_2 5
#define NUMPIXELS 16

// Define Interrupt Pin
#define RTC_INT_PIN 2

// Define the Scroll Wheel
#define SCROLL_PIN A0

// Define Buttons
#define BTN_YES A1
#define BTN_NO A2

// Declare and start Backpack
Adafruit_LiquidCrystal lcd(0);

// Declare Sleep Breakout
RTC_DS3231 rtc;

// Declare Pixel ring
Adafruit_NeoPixel pixels1(NUMPIXELS, EMERGENCY_LIGHT_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, EMERGENCY_LIGHT_2, NEO_GRB + NEO_KHZ800);

// Declare sleep time variables
int days = 0, hours = 0, minutes = 0, seconds = 20;

// Declare DateTime formats to use
DateTime startTime;
DateTime targetTime;

// For button pressing logic, create enum datatype
enum ButtonPress {NONE, YES, NO};

// Emergency Alert Code
void emergency()
{
  uint32_t red = pixels1.Color(255, 0, 0);  // Red warning color

  // First beep + flash
  pixels1.fill(red);      // turn all LEDs on
  pixels2.fill(red);
  pixels1.show();
  pixels2.show();
  tone(BEEPER, 1000);     // 1 kHz beep
  delay(200);             // beep & flash duration

  pixels1.clear();        // turn LEDs off
  pixels2.clear(); 
  pixels1.show();       
  pixels2.show();
  noTone(BEEPER);         // stop beep
  delay(150);             // short gap between beeps

  // Second beep + flash
  pixels1.fill(red);
  pixels2.fill(red);
  pixels1.show();
  pixels2.show();
  tone(BEEPER, 1000);
  delay(200);

  pixels1.clear();
  pixels2.clear();
  pixels1.show();       
  pixels2.show();
  noTone(BEEPER);

  delay(800);         // long pause before repeating
}

// Green Status Light Blink (Future?)

// Timer selection Menu
// Create timer Menu
void timeSelection()
{
  // Create the menu at the top of the Display and move to the next
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time Selection");
  lcd.setCursor(0,1);

  // Menu Switching
  bool inHourMenu = 1;
  bool inMinuteMenu = 0;
  int lastHours = -1;
  int lastMinutes = -1;

  // Create the hours and minutes to be multipied later
  int hourSelect = 0;
  int minuteSelect = 0;

  while (inHourMenu)
  {
    // Selection of Hours
    hourSelect = map(analogRead(SCROLL_PIN), 0, 1023, 0, 24);

      if (hourSelect != lastHours)  // only redraw if value changed
      {
        lcd.setCursor(0,1);
        lcd.print(hourSelect);
        lcd.print(" Hours          ");
        lastHours = hourSelect;
      }

    ButtonPress btnHour = readButtons();

    // Check to see if needing to continue
    if (btnHour == YES)
    {
      delay(100);
      inMinuteMenu = 1;
      
      // Selection of minutes
      while(inMinuteMenu)
      {
        minuteSelect = map(analogRead(SCROLL_PIN), 0, 1023, 0, 59);

        if (minuteSelect != lastMinutes)
        {
          lcd.setCursor(0,1);
          lcd.print(minuteSelect);
          lcd.print(" Minutes        ");
          lastMinutes = minuteSelect;
        }

        ButtonPress btnMin = readButtons();

        if (btnMin == YES)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Time Selected");
          lcd.setCursor(0,1);
          lcd.print(hourSelect);
          lcd.print("h ");
          lcd.print(minuteSelect);
          lcd.print("m");
          delay(3000);
          inMinuteMenu = 0;
          inHourMenu = 0;
          break;
        }

        else if (btnMin == NO)
        {
          delay(100);
          inMinuteMenu = 0;
          inHourMenu = 1;
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Time Selection");
          lcd.setCursor(0,1);
        }
      }
    }

    else if (btnHour == NO)
    {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Exiting");
      
      // No selection made
      hourSelect = 0;
      minuteSelect = 0;

      // Escape Menu and loop
      inHourMenu = 0;

      // Allow user to see
      delay(2000);
      break;
    }
  }

  hours = hourSelect;
  minutes = minuteSelect;
}

// Button Press Checking Logic
ButtonPress readButtons()
{
  if (digitalRead(BTN_YES) == LOW)
  {
      delay(50); // debounce: Checks to make sure not noise
      if (digitalRead(BTN_YES) == LOW)
          return YES;
  }
  if (digitalRead(BTN_NO) == LOW)
  {
      delay(50); // debounce 
      if (digitalRead(BTN_NO) == LOW)
          return NO;
  }
  return NONE;
}

// Start by Initializing the LCD Screen Startup
void lcdInitializationSetup()
{
  lcd.print("Starting...");
  delay(1000);
  for (int i = 3; i>0; i--)
  {
    lcd.clear();
    lcd.print(i);
    delay(1000);
  }
  lcd.clear();
}

// Setup and start the timer for alarm
void RTCSetup()
{
  // Start the RTC Breakout
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // Setting arbitrary start time to start at

  // Create start and end times for the breakout
  startTime = rtc.now();
  targetTime = startTime + TimeSpan(days, hours, minutes, seconds); //To be set by adjusting on display

  // Clear alarms
  rtc.clearAlarm(1);
  
  // Disable second alarm
  rtc.disableAlarm(2);
}

void ledLightSetup()
{
  pixels1.begin();
  pixels1.setBrightness(50);    //0 - 255

  pixels2.begin();
  pixels2.setBrightness(50);    //0 - 255
}

// Putting all to sleep
void goToSleep() 
{
  // Attach interrupt on pin 2, wake on LOW signal from RTC alarm
  attachInterrupt(digitalPinToInterrupt(RTC_INT_PIN), wakeUp, LOW);

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // Deepest sleep mode
  sleep_enable();
  sleep_mode();                         // Board sleeps HERE

  // --- Wakes up here after interrupt ---
  sleep_disable();
  detachInterrupt(digitalPinToInterrupt(RTC_INT_PIN));

  //lcd.setBacklight(1);
}

void wakeUp() {
  // ISR — intentionally empty, just wakes the board
}

// Sets the alarm and tells board to go to sleep also calling sleep command
void setAlarmAndSleep() 
{
  lcd.clear();
  lcd.print("Sleeping...");
  delay(1000);
  lcd.clear();
  lcd.setBacklight(0);

  rtc.setAlarm1(targetTime, DS3231_A1_Second);
  goToSleep();
  rtc.clearAlarm(1);  // clears AFTER waking
  delay(100);
}

void setup() 
{
  Serial.begin(9600);

  // Scroll wheel and button Setup
  pinMode(BTN_YES, INPUT_PULLUP);
  pinMode(BTN_NO, INPUT_PULLUP);

  // Begin the setup of RTC Breakout
  rtc.begin();

  // Have LCD Initialize so that know is on
  lcd.begin(16,2);
  lcd.setBacklight(1);
  lcdInitializationSetup();
  Serial.println("LCD Setup..");

  timeSelection();

  RTCSetup();

  Serial.println("RTC Setup...");

  // Setup Alarm Lights
  ledLightSetup();

  // Start timer and go to sleep
  setAlarmAndSleep();
}

void loop() {
  // put your main code here, to run repeatedly:
  emergency();
}
