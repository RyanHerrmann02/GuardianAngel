# Guardian Angel
**Course Project — Spring 2026 | APD, University of Colorado Boulder**

## Overview
Guardian Angel is an Arduino-based trailhead safety indicator designed to assist 
Search and Rescue (SAR) personnel and other outdoor enthusiasts to communicate the status of
users when in the backcountry. This project was developed as part of the
Advanced Product Design (APD) course Spring 2026 at the University of Colorado Boulder.

## Hardware Architecture

### Power Management
A core engineering challenge of this project was designing a reliable power sequencing
system that allows the Arduino to control its own power state while also being woken
remotely by an RTC alarm all from a single LiPo battery source with no dedicated
power management IC.

#### Passive Power Latch (RC Hold Circuit)
To allow the Arduino to hold itself powered on after the user presses the power switch,
a passive PMOS-based power latch was designed and verified through SPICE simulation.
On power switch press, an RC circuit (47kΩ / 100µF, τ ≈ 4.7s) pulls the PMOS gate low
long enough for the Arduino to boot and assert its own hold pin, latching the board on
indefinitely. To enable fast and reliable resets, an asymmetric discharge path was
implemented using a 1kΩ resistor in series with a flyback diode in parallel with the
charge resistor, reducing the discharge time constant to ~100ms. This ensures the latch
clears quickly and cleanly between power cycles without requiring any active components.

#### RTC Wake Circuit (Discrete Transistor Level Shifter)
The DS3231 RTC module operates on a 5V rail that remains powered after the Arduino board 
is fully shut off, minimizing system current draw during the wait period. This allows the 
RTC to continue tracking time and fire a wake alarm via its open-drain SQW pin when the 
countdown expires. The PMOS transistor, which is responsible for gating battery voltage 
into the Arduino, requires its gate to be pulled low at battery voltage (~7.4V) to power 
the board back on a different domain from the 5V RTC supply. A two-stage discrete 
transistor circuit was designed to bridge these domains...without connecting them directly. 
When the SQW pin fires and pulls low, current flows
through a PNP transistor biased from the 5V rail, whose collector output drives the base
of an NPN transistor referenced to ground. The NPN collector then pulls the PMOS gate
low at battery voltage, turning the board back on. This keeps the 5V RTC supply domain
fully isolated from the battery voltage domain while reliably translating the alarm signal
into a board power-on event.

---

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
| `RTCPullDown.ino` | Final testing with RTC breakout to verify SQW pin pulls down to turn on PMOS. |

## Development Team
- Ryan Herrmann
- Duncan Liard
- Jena Knaack
- Aidan Stamp
- Riley Apen

##### Circuit lead design by Ryan Herrmann.
