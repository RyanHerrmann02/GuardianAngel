# Guardian Angel
**Course Project — Spring 2026 | APD, University of Colorado Boulder**

## Overview
Guardian Angel is an Arduino-based trailhead safety indicator designed to assist 
Search and Rescue (SAR) personnel and other outdoor enthusiasts to communicate the status of
users when in the backcountry. This project was developed as part of the
Advanced Product Design (APD) course Spring 2026 at the University of Colorado Boulder.

## Future Development
Long-range wireless communication via LoRa radio integration is currently under 
consideration for future iterations of the project. This would enable communication 
between trail indicators and users within certain ranges. Implementation is subject 
to project timeline and scope constraints.

## Repository Contents
The following files are included in this repository:

| File | Description |
|------|-------------|
| `GACarCommand.ino` | Primary device firmware handling LCD display, RTC alarm scheduling, and emergency lighting. |
| `RFSendCommand.ino` | Test file for transmitting LoRa radio commands, using a potentiometer to select color-coded signal states. |
| `RFReceiveCommand.ino` | Test file for receiving LoRa radio commands, displaying results on an LCD and controlling corresponding LEDs. |
| `HelloWorld.ino` | Basic LCD I2C display verification test. |
| `TurnOnDemo.ino` | Basic verification for turn on and check for Alarm state. |
| `BasicBoardTesting.ino` | "Sandbox" for testing board pulldown transistor testing for utilizing PMOS for board power. |
| `SleepBreakouTest.ino` | Initial test code for RTC breakout to track time. |
| `SleepCommandDemo.ino` | Initial test code for UI interfacing with buttons and potentiometer to set time. |

## Development Team
- Ryan Herrmann
- Duncan Liard
- Jena Knaack
- Aidan Stamp
- Riley Apen