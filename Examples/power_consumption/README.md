# ATtiny85 power consumption
We use the ATtinyCore for this test: https://goo.gl/hin2re  

ATtiny85 with:  
- 1Mhz  
- no BOD  
- 3v3  


## Gammon base example https://goo.gl/62b0Im

```
// ATtiny85 sleep mode, wake on pin change interrupt demo
// Author: Nick Gammon
// Date: 12 October 2013


#include <avr/sleep.h>    // Sleep Modes
#include <avr/power.h>    // Power management

const byte LED = 3;  // pin 2
const byte SWITCH = 4; // pin 3 / PCINT4

ISR (PCINT0_vect)
 {
 // do something interesting here
 }

void setup ()
  {
  pinMode (LED, OUTPUT);
  pinMode (SWITCH, INPUT);
  digitalWrite (SWITCH, HIGH);  // internal pull-up

  // pin change interrupt (example for D4)
  PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3
  GIFR   |= bit (PCIF);    // clear any outstanding interrupts
  GIMSK  |= bit (PCIE);    // enable pin change interrupts

  }  // end of setup

void loop ()
  {
  digitalWrite (LED, HIGH);
  delay (500);
  digitalWrite (LED, LOW);
  delay (500);
  goToSleep ();
  }  // end of loop


void goToSleep ()
  {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();                             
  sleep_disable();   
  power_all_enable();    // power everything back on
  }  // end of goToSleep
  ```

Datasheet    |     Real
------------ | -------------
 0.2uA       |     0.17uA


 ## Serial examples

The Tiny85 has not a Serial (UART) in the chip.  
So to use the Serial, we need have 2 possibility:  
- Standard SoftwareSerial from Arduino https://goo.gl/uYKaG1  
- Edited SoftwareSerial from ATtinyCore https://goo.gl/k8g67g  

The main difference is that with the ATtinyCore SoftwareSerial you can use the "pin change interrupts" (PCINT), in the normal SoftwareSerial you can't.  


 ### Standard SoftwareSerial
```
// It wakes up roughly every 8 seconds and send data over the Standard SoftwareSerial

#include <SoftwareSerial.h>
#include <avr/sleep.h>    // Sleep Modes https://goo.gl/Mm7wuk
#include <avr/power.h>    // Power management https://goo.gl/g6bHHz
#include <EEPROM.h>
#include <avr/wdt.h>      // Watchdog timer

#define epromMemory 100

SoftwareSerial mySerial(1, 0); // RX, TX

//
// ========== watchdog interrupt
//
ISR (WDT_vect)
{
  wdt_disable();  // disable watchdog
}  // end of WDT_vect


//
// ========== resetWatchdog
//
void resetWatchdog ()
{
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  // pat the dog
  wdt_reset();
}  // end of resetWatchdog


//
// ========== sleepMode
//
void sleepMode() {
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  // Turn off ADC
  // You must use the PRR after setting ADCSRA to zero,
  // otherwise the ADC is "frozen" in an active state.
  ADCSRA = 0; // turn off ADC, we don't need it, it's consume alot.

  power_all_disable (); // power off ADC, Timer 0 and 1, serial interface

  // disable all interrupts (including timer interrupts, serial interrupts, etc.)
  // clear interrupts flag, equal to cli
  noInterrupts (); // make sure we don't get interrupted before we sleep

  resetWatchdog ();      // get watchdog ready

  sleep_enable (); // ready to sleep
  interrupts (); // interrupts allowed now, next instruction WILL be executed
  sleep_cpu (); // sleep

  sleep_disable (); // precaution
  power_all_enable (); // power everything back on
}


//
// ========== serialEnable
//
void serialEnable() {
  // Load data from EEPROM
  for (unsigned int i = 0; i < epromMemory; i++) {
    // Read the value stored in the EEPROM
    byte value = EEPROM.read(i);
    mySerial.write(i);
    mySerial.write(value);
    delay(100);
  }
}


//
//====================== SETUP ======================
//
void setup() {
  // SoftwareSerial
  mySerial.begin(9600);

  resetWatchdog ();  // do this first in case WDT fires

}


//
// ========== LOOP
//
void loop() {

  // Sent over Serial
  serialEnable();

  // Go to sleep baby
  sleepMode();
}
```

Serial on    |     Sleep
------------ | -------------
 865uA       |     4,65uA


### ATtinyCore SoftwareSerial
```
// It wakes up roughly every 8 seconds and send data over the ATtinyCore SoftwareSerial

#include <avr/sleep.h>    // Sleep Modes https://goo.gl/Mm7wuk
#include <avr/power.h>    // Power management https://goo.gl/g6bHHz
#include <EEPROM.h>
#include <avr/wdt.h>      // Watchdog timer

#define epromMemory 100


//
// ========== watchdog interrupt
//
ISR (WDT_vect)
{
  wdt_disable();  // disable watchdog
}  // end of WDT_vect


//
// ========== resetWatchdog
//
void resetWatchdog ()
{
  // clear various "reset" flags
  MCUSR = 0;
  // allow changes, disable reset, clear existing interrupt
  WDTCR = bit (WDCE) | bit (WDE) | bit (WDIF);
  // set interrupt mode and an interval (WDE must be changed from 1 to 0 here)
  WDTCR = bit (WDIE) | bit (WDP3) | bit (WDP0);    // set WDIE, and 8 seconds delay
  // pat the dog
  wdt_reset();
}  // end of resetWatchdog


//
// ========== sleepMode
//
void sleepMode() {
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  // Turn off ADC
  // You must use the PRR after setting ADCSRA to zero,
  // otherwise the ADC is "frozen" in an active state.
  ADCSRA = 0; // turn off ADC, we don't need it, it's consume alot.

  power_all_disable (); // power off ADC, Timer 0 and 1, serial interface

  // disable all interrupts (including timer interrupts, serial interrupts, etc.)
  // clear interrupts flag, equal to cli
  noInterrupts (); // make sure we don't get interrupted before we sleep

  resetWatchdog ();      // get watchdog ready

  sleep_enable (); // ready to sleep
  interrupts (); // interrupts allowed now, next instruction WILL be executed
  sleep_cpu (); // sleep

  sleep_disable (); // precaution
  power_all_enable (); // power everything back on
}


//
// ========== serialEnable
//
void serialEnable() {
  // Load data from EEPROM
  for (unsigned int i = 0; i < epromMemory; i++) {
    // Read the value stored in the EEPROM
    byte value = EEPROM.read(i);
    Serial.write(i);
    Serial.write(value);
    delay(100);
  }
}


//
//====================== SETUP ======================
//
void setup() {
  // SoftwareSerial
  Serial.begin(9600);

  resetWatchdog ();  // do this first in case WDT fires

}


//
// ========== LOOP
//
void loop() {

  // Sent over Serial
  serialEnable();

  // Go to sleep baby
  sleepMode();
}
```

Serial on    |     Sleep
------------ | -------------
 885uA       |     21,21uA
