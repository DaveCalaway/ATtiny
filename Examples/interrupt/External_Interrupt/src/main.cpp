/*
   That code use the D4 pin to wake the Devicea as interrupt using the "pin change interrupt".
   After the wake, the Device wait for SW_pin to be released and go to sleep.

   ATMEL ATTINY 25/45/85 / ARDUINO

   Ain0 (D 5) PB5  1| *  |8  Vcc
   Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
   Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
              GND  4|____|5  PB0 (D 0) pwm0

   DaveCalaway
 */

/* Bitwise Operators:
   "&= ~" change only a specific register bit position to zero.
   "|=" change only a specific register bit position to one.
 */

#include <Arduino.h>
#include <avr/sleep.h>    // Sleep Modes https://goo.gl/Mm7wuk
#include <avr/power.h>    // Power management https://goo.gl/g6bHHz

#define interruptPin 4
#define ledDebugPin 1


//
// ========== Interrupt handling only from PCINT0_vect, not INT0_vect
//
ISR (PCINT0_vect){
}


//
// ========== sleepMode
//
void sleepMode() {
        // Turn off ADC
        // You must use the PRR after setting ADCSRA to zero,
        // otherwise the ADC is "frozen" in an active state.
        ADCSRA = 0; // turn off ADC
        
        // disable all interrupts (including timer interrupts, serial interrupts, etc.)
        // clear interrupts flag, equal to cli
        noInterrupts (); // make sure we don't get interrupted before we sleep

        set_sleep_mode (SLEEP_MODE_PWR_DOWN);
        power_all_disable ();  // power off ADC, Timer 0 and 1, serial interface
        sleep_enable (); // ready to sleep
        interrupts ();     // interrupts allowed now, next instruction WILL be executed
        sleep_cpu ();    // sleep

        sleep_disable (); // precaution
        power_all_enable (); // power everything back on
}


//
// ========== SETUP
//
void setup() {
        pinMode(interruptPin, INPUT_PULLUP);
        pinMode(ledDebugPin, OUTPUT);

        // I'm alive!
        digitalWrite(ledDebugPin, HIGH);
        delay(1000);
        digitalWrite(ledDebugPin, LOW);

        // Turn off ADC
        // You must use the PRR after setting ADCSRA to zero,
        // otherwise the ADC is "frozen" in an active state.
        ADCSRA = 0;   // turn off ADC, we don't need it, it's consume alot.

        // pin change interrupt (example for D4)
        PCMSK  |= bit (PCINT4);  // want pin D4 / pin 3

        // General Interrupt Flag Register
        // When an edge or logic change on the INT0 pin triggers an interrupt request, INTF0 becomes set (one).
        // Bit 6 – INTF0: External Interrupt Flag 0
        GIFR   |= bit (PCIF);    // clear any outstanding interrupts

        // General Interrupt Mask Register
        // Bit 6 – INT0: External Interrupt Request 0 Enable
        GIMSK  |= bit (PCIE);    // enable pin change interrupts
}


//
// ========== LOOP
//
void loop() {
        digitalWrite(ledDebugPin, HIGH);
        delay(250);
        digitalWrite(ledDebugPin, LOW);

        while (digitalRead (interruptPin) == LOW) {
                // wait for switch to be released
        }
        // Go to sleep baby
        sleepMode();
}
