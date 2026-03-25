/*  To start, we need to implement 3 things:
    LCD Display, LoRa Integration, Deep Sleep 
    LCD Display will be using a shifter, but eventually
    that will be on its own breakout. LoRa will be used
    for the radio and that will work with deep sleep
    deep sleep will be used including a breakout allowing for 
    it to use ~0.1μA of power including an interupt from a LoRa
    signal. For the demo, Pins will all be the same however,
    LoRa will not be implemented here. */

// Importing Necessary Libraries
#include <SPI.h>
#include <RH_RF95.h>
#include <LiquidCrystal.h>
#include <avr/sleep.h>
#include <avr/wdt.h>        // Watchdog Timer

// Defining LoRa breakout pins and Radio Freq to be ignored
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

// Defining the lcd display pinouts
// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

// Define the Scroll Wheel
#define SCROLL_PIN A0

// Define Buttons
#define BTN_YES A1
#define BTN_NO A2

// Define Green, Red, Backlight Pins
#define RED_ALERT A3
#define LCD_BACKLIGHT A4

// For button pressing logic, create enum datatype
enum ButtonPress {NONE, YES, NO};

// Create the deep sleep variables and routine
// Watchdog Timer max is 8s, to get to the needed sleep time, divide total time by 8
volatile int wdtCount = 0;
uint32_t wdtTarget = 0; // To be cycled through

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
  int hours = 0;
  int minutes = 0;

  while (inHourMenu)
  {
    // Selection of Hours
    hours = map(analogRead(SCROLL_PIN), 0, 1023, 0, 24);

      if (hours != lastHours)  // only redraw if value changed
      {
        lcd.setCursor(0,1);
        lcd.print(hours);
        lcd.print(" Hours          ");
        lastHours = hours;
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
        minutes = map(analogRead(SCROLL_PIN), 0, 1023, 0, 59);

        if (minutes != lastMinutes)
        {
          lcd.setCursor(0,1);
          lcd.print(minutes);
          lcd.print(" Minutes        ");
          lastMinutes = minutes;
        }

        ButtonPress btnMin = readButtons();

        if (btnMin == YES)
        {
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Time Selected");
          lcd.setCursor(0,1);
          lcd.print(hours);
          lcd.print("h ");
          lcd.print(minutes);
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
      hours = 0;
      minutes = 0;

      // Escape Menu and loop
      inHourMenu = 0;

      // Allow user to see
      delay(2000);
      break;
    }
  }

  uint32_t totalSeconds = (uint32_t)hours * 3600 + (uint32_t)minutes * 60;
  wdtTarget = totalSeconds / 8;
}

// Ask to start timer
void timerStart()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Start Sleep");
  lcd.setCursor(0,1);
  lcd.print("Timer? y/n");

  while(1)
  {
    ButtonPress btn = readButtons();
    if (btn == YES)
    {
      lcd.clear();
      break;
    }
    else if (btn == NO)
    {
      lcd.clear();
      while(1);
    }
  }
}

// Red Alert
void alertAll()
{
  lcd.setCursor(0,0);
  lcd.print("ALERT!  ALERT!  ");

  //alertMessage("POTENTIAL MISSING PERSON! CALL SEARCH AND RESCUE!", 1, 250);
  lcd.setCursor(0,1);
  lcd.print("CALL SAR!");
  while(1)
  {
    digitalWrite(RED_ALERT, HIGH);
    delay(300);
    digitalWrite(RED_ALERT, LOW);
    delay(300);
  }
}

// Scrolling Alert Message Call
void alertMessage(const char* message, int row, int delayMs)
{
  int msgLen = strlen(message);
  // Pad with spaces so text slides cleanly off-screen
  String padded = String("                ") + message + String("                ");

  for (int i = 0; i <= padded.length() - 16; i++)
  {
    lcd.setCursor(0, row);
    lcd.print(padded.substring(i, i + 16));
    delay(delayMs);
  }
}
// Watchdog Interrupt Service Routine this will get replaced with breakout for less drift
ISR(WDT_vect)
{
  wdtCount++;
}

// Set up the Watchdog Timer 
void setupWDT()
{
  // Disable interrupts for configuration
  cli();

  // Reset WDT
  wdt_reset();

  // Enter WDT configuration mode
  WDTCSR |= (1 << WDCE) | (1 << WDE);

  // Setup 8 second timeout according to WDT max, and interrupt mode (not reset mode)
  WDTCSR = (1 << WDIE) | (1 << WDP3) | (1 << WDP0);

  // Re-enable Interrupts
  sei();
}

// Create DeepSleep call
void deepSleep()
{
  wdtCount = 0;
  
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // Deepest Sleep Mode
  sleep_enable();

  while(wdtCount < wdtTarget)
  {
    setupWDT(); // Arm WDT
    sleep_cpu(); // This is sleeping - wakes board on interrupt
  }

  sleep_disable();

  // Turn off Watchdog when woken up
  wdt_disable();
}

void setup() {
  // Scroll wheel and button Setup
  pinMode(BTN_YES, INPUT_PULLUP);
  pinMode(BTN_NO, INPUT_PULLUP);

  pinMode(RED_ALERT, OUTPUT);

  pinMode(LCD_BACKLIGHT, OUTPUT);
  digitalWrite(LCD_BACKLIGHT, HIGH);

  // LCD Initialization
  lcd.begin(16, 2);
  lcdInitializationSetup();
  timeSelection();
  delay(500);
  timerStart();
  delay(200);

  // Actual Sleep function call
  lcd.print("Going to sleep...");
  delay(1000);
  lcd.clear();
  lcd.noDisplay();
  digitalWrite(LCD_BACKLIGHT, LOW);

  deepSleep();

  lcd.display();
  digitalWrite(LCD_BACKLIGHT, HIGH);
  lcd.setCursor(0,0);
}

void loop() {
  // put your main code here, to run repeatedly:
  alertAll();
}
