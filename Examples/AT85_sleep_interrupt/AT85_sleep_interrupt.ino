/*
  ATtiny85 sleep mode and wake on pin change
  Author: Davide Gariselli

  Original version made by Nick Gammon https://goo.gl/62b0Im
  // ATMEL ATTINY 25/45/85 / ARDUINO
  //
  //                  +- \/-+
  // Ain0 (D 5) PB5  1|*    |8  Vcc
  // Ain3 (D 3) PB3  2|     |7  PB2 (D 2) Ain1
  // Ain2 (D 4) PB4  3|     |6  PB1 (D 1) pwm1
  //            GND  4|     |5  PB0 (D 0) pwm0
  //                  +-----+
  
  It's works with 3,3V (8Mhz) or 5V (8Mhz or 16Mhz)
*/

// PinChangeInterrupt ( attachPCINT ) Library 1.2.4 https://goo.gl/WhlCwl
#include <PinChangeInterrupt.h>

#include <avr/sleep.h>    // Sleep Modes https://goo.gl/WJUszs
#include <avr/power.h>    // Power management https://goo.gl/58Vdvv

#define LED 3 // D3
#define SWITCH 4 // D4

void setup () {
  pinMode (LED, OUTPUT);
  pinMode (SWITCH, INPUT_PULLUP); // internal pull-up
  // Valid interrupt modes are: RISING, FALLING or CHANGE
  attachPCINT(digitalPinToPinChangeInterrupt(SWITCH), wakeUpNow, FALLING);
}

void loop () {
  digitalWrite (LED, HIGH);
  delay (2000);
  digitalWrite (LED, LOW);
  delay (2000);
  goToSleep ();
}  // end of loop

void wakeUpNow() {
  // execute code here after wake-up before returning to the loop() function
  // timers and code using timers (serial.print and more...) will not work here.
  // we don't really need to execute any special functions here, since we
  // just want the thing to wake up
}

void goToSleep () {
  //    * The 5 different modes are:
  //     *     SLEEP_MODE_IDLE         -the least power savings
  //     *     SLEEP_MODE_ADC
  //     *     SLEEP_MODE_PWR_SAVE
  //     *     SLEEP_MODE_STANDBY
  //     *     SLEEP_MODE_PWR_DOWN     -the most power savings
  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // <avr/sleep.h>
  ADCSRA = 0;            // turn off ADC
  power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface <avr/power.h>
  sleep_enable(); // <avr/sleep.h>
  sleep_cpu(); // <avr/sleep.h>
  //---------------  THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP ---------------
  sleep_disable();// <avr/sleep.h>
  power_all_enable();    // power everything back on <avr/power.h>
}
