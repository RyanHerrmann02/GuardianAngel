/*  Full code for the Guardian Angel Car Command:
    This code will handle the following functions, after turning on the device the 
    arduino will hold itself to high. It will also check to see if it was the RTC that 
    turned it back on or if it was done on startup. This will run the emergency command
    as well as setting sleep through UI. We will be implementing 4 input controls. It
    includes 3 buttons for prompts and timer setting. We implement I2C for less pinouts
    on the LCD display as well as Neopixel LEDs, and RTC for timer handling.*/

// Importing Necessary Libraries
#include <Adafruit_LiquidCrystal.h>
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_NeoPixel.h>

//Define Lights
#define BEEPER 7
#define EMERGENCY_LIGHT_1 6
#define EMERGENCY_LIGHT_2 5
#define NUMPIXELS 16

// Define the Scroll Wheel
#define SCROLL_PIN A0

// Define Buttons
#define BTN_YES A1
#define BTN_NO A2
#define BTN_KILL A1

// Define KillPin
#define KILL_SWITCH 4

// Declare and start Backpack
Adafruit_LiquidCrystal lcd(0);

// Declare Sleep Breakout
RTC_DS3231 rtc;

// Declare Pixel ring
Adafruit_NeoPixel pixels1(NUMPIXELS, EMERGENCY_LIGHT_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(NUMPIXELS, EMERGENCY_LIGHT_2, NEO_GRB + NEO_KHZ800);

// Declare sleep time variables
int days, hours, minutes;

// Declare DateTime formats to use
DateTime startTime;
DateTime targetTime;

// For button pressing logic, create enum datatype
enum ButtonPress {NONE, YES, NO};

// Button Press YES NO logic.
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

/*-----------------------------------------------------------
  The following code handles the emergency scenario it will
  function be simultaneously beeping and flashing red lights
  on an interval. Emergency does that while the
  ledLightSetup handles starting the rings. Important to wait
  to set them up until later so that the current pull is low
-----------------------------------------------------------*/
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

// Setting up LED Ring Lights as RED
void ledLightSetup()
{
  pixels1.begin();
  pixels1.setBrightness(60);    //0 - 255

  pixels2.begin();
  pixels2.setBrightness(60);    //0 - 255
}

/*-----------------------------------------------------------
  The following code here is in charge of creating
  a menu where the user can select the time that they
  want the alarm to go off for, this is all UI so they can
  go back and forth between menus as well as wait to 
  start the timer until they are ready. Once ready, the
  timer will be primed for the user to press the button kill
  switch
-----------------------------------------------------------*/
// Create timer Menu
void timeSelection()
{
  // Force I2C to update at slower refresh rate
  unsigned long lastLCDUpdate = 0;
  const int LCD_UPDATE_INTERVAL = 200; // only update every 200ms

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

  // Create the hours and minutes to be set on RTC
  int hourSelect = 0;
  int minuteSelect = 0;

  while (inHourMenu)
  {
    // Selection of Hours
    hourSelect = map(analogRead(SCROLL_PIN), 0, 1023, 0, 23);

    if (hourSelect != lastHours && millis() - lastLCDUpdate > LCD_UPDATE_INTERVAL)  // only redraw if value changed
    {
      lcd.setCursor(0,1);
      lcd.print(hourSelect);
      lcd.print(" Hours          ");
      lastHours = hourSelect;
      lastLCDUpdate = millis();
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

        if (minuteSelect != lastMinutes && millis() - lastLCDUpdate > LCD_UPDATE_INTERVAL)
        {
          lcd.setCursor(0,1);
          lcd.print(minuteSelect);
          lcd.print(" Minutes        ");
          lastMinutes = minuteSelect;
          lastLCDUpdate = millis();
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
  }

  hours = hourSelect;
  minutes = minuteSelect;

  days = 0; // Days selection to be added in later
}

// Ask to start timer
bool timerStart()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Set Alarm?");
  lcd.setCursor(0,1);
  lcd.print("Yes/No");

  while(1)
  {
    ButtonPress btn = readButtons();
    if (btn == YES)
    {
      lcd.clear();
      return true;
    }
    else if (btn == NO)
    {
      lcd.clear();
      return false;
    }
  }
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
    delay(500);
  }
  lcd.clear();
}

// Setup and start the timer for alarm
void RTCSetup()
{
  // Start the RTC Breakout
  rtc.adjust(DateTime(2000, 1, 1, 0, 0, 0)); // Setting arbitrary start time to start at
  delay(50);  // Give the RTC a sec to get all setup to avoid calling code and then messing up the setup

  // Create start and end times for the breakout
  startTime = rtc.now();
  targetTime = startTime + TimeSpan(days, hours, minutes, 0); //To be set by adjusting on display

  // Clear alarms
  rtc.clearAlarm(1);
  
  // Disable second alarm
  rtc.disableAlarm(2);
}

// Sets the alarm and turns off power to main arduino board.
void armAlarm() 
{
  lcd.clear();
  lcd.print("To set alarm,");
  lcd.setCursor(0,1);
  lcd.print("press YES Button");
  delay(500);

  while(1)
  {
    if (digitalRead(BTN_KILL) == LOW)
    {
      break;
    }
  }

  // Setup timer to get start and stop time
  RTCSetup();

  // Start Alarm
  rtc.setAlarm1(targetTime, DS3231_A1_Date);
  delay(100);

  // Kill power to board
  digitalWrite(KILL_SWITCH, LOW);
}

void setup() 
{
  // For every start, check alarm and make kill pin high.
  pinMode(KILL_SWITCH, OUTPUT);
  digitalWrite(KILL_SWITCH, HIGH);

  delay(1000);
  Wire.begin();
  Wire.setWireTimeout(3000, true);  //Keeping wire timeout for testing if lights off then same issues
  rtc.begin();

  // Have LCD Initialize so that know is on
  delay(500);
  lcd.begin(16,2);
  lcd.begin(16,2);
  lcd.setBacklight(1);

  // Scroll wheel and button Setup
  pinMode(BTN_YES, INPUT_PULLUP);
  pinMode(BTN_NO, INPUT_PULLUP);
  pinMode(BTN_KILL, INPUT_PULLUP);

  // If alarm fired, run emergency, if not run setup protocol
  if(rtc.alarmFired(1))
  {
    // Clear Alarm after wake
    rtc.clearAlarm(1);

    // Setup Alarm Lights
    ledLightSetup();

    // Print Missing Person alert on LCD
    lcd.print("MISSING PERSON:");
    lcd.setCursor(0,1);
    lcd.print("CALL S.A.R");
    
    while(1)
    {
      emergency();
    }
  }
  else
  {
    lcdInitializationSetup(); // To be removed later this is validation testing so user can see it is on

    // Handle the menu logic for time selection
    bool timerConfirmed = false;
    while(!timerConfirmed)
    {
      timeSelection();
      timerConfirmed = timerStart();
    }
    
    // Start the hibernation and alarm for wake
    armAlarm();
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
