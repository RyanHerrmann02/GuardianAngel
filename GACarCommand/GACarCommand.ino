/*  To start, we need to implement 3 things:
    LCD Display, LoRa Integration, Deep Sleep 
    LCD Display will be using a shifter, but eventually
    that will be on its own breakout. LoRa will be used
    for the radio and that will work with deep sleep
    deep sleep will be used including a breakout allowing for 
    it to use ~0.1μA of power including an interupt from a LoRa
    signal. */

// Importing Necessary Libraries
#include <SPI.h>
#include <RH_RF95.h>
#include <LiquidCrystal.h>
#include <avr/sleep.h>
#include <avr/wdt.h> // Watchdog Timer

// Defining LoRa breakout pins and Radio Freq
#define RFM95_CS 10
#define RFM95_RST 9
#define RFM95_INT 2
#define RF95_FREQ 915.0

// Defining the lcd display pinouts using shifter
// LCD Pins: RS, E, D4, D5, D6, D7
LiquidCrystal lcd(7, 6, 5, 4, 3, 8);

// Create the deep sleep variables and routine
// Watchdog Timer max is 8s, to get to the needed sleep time, divide total time by 8
volatile int wdtCount = 0;
const int WDT_TARGET = 30; // 4 min is 240s / 8 = 30

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

  while(wdtCount < WDT_TARGET)
  {
    setupWDT(); // Arm WDT
    sleep_cpu(); // This is sleeping - wakes board on interrupt
  }

  sleep_disable();

  // Turn off Watchdog when woken up
  wdt_disable();
}

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
