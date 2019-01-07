// Written by Nick Gammon
// Committed by DaveCalaway
//
// That code use the interrupt pin to wake the Device.
// After the wake, the Device wait for SW_pin to be released.
//
// ATMEL ATTINY 25/45/85 / ARDUINO
// Pin 1 is /RESET
//
//                  +-\/-+
// Ain0 (D 5) PB5  1|    |8  Vcc
// Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1
// Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1
//            GND  4|    |5  PB0 (D 0) pwm0
//                  +----+

/* Connections:
   Pin 7  <-- SW_pinitch to Gnd
   Pin 5 (PB0) <---- LED_pin ---> 330 R <-----> Gnd
 */

/* Bitwise Operators:
   &= ~ change only a specific register bit position to zero.
 |= change only a specific register bit position to one.
 */

#include <Arduino.h>
#include <avr/sleep.h>    // Sleep Modes https://goo.gl/Mm7wuk
#include <avr/power.h>    // Power management https://goo.gl/g6bHHz

#define LED_pin 0          // pin 5 (D0)
#define SW_pin 2           // pin 7 (D2)



//
// ========== Interrupt service routine
// This ISR handling the interrupt from the pin interrupt INT0 = PB2 = D2
ISR (INT0_vect){
        // General Interrupt Mask Register
        // Bit 6 – INT0: External Interrupt Request 0 Enable
        GIMSK &= ~bit(INT0); // disable INT0
}


//
// ========== SETUP
//
void setup (){
        pinMode (LED_pin, OUTPUT);
        pinMode (SW_pin, INPUT_PULLUP);

        // I'm alive!
        digitalWrite(LED_pin, HIGH);
        delay(1000);
        digitalWrite(LED_pin, LOW);

        // Turn off ADC
        // You must use the PRR after setting ADCSRA to zero,
        // otherwise the ADC is "frozen" in an active state.
        ADCSRA = 0;      // turn off ADC, we don't need it, it's consume alot.

        // MCU Control Register
        // The External Interrupt Control Register A contains control bits for interrupt sense control.
        // Remember, during the deepSleep (SLEEP_MODE_PWR_DOWN) the interrupt only sees the LOW trigger,
        // not the change, falling or rising edge.
        // ISC01 = 0 and ISC00 = 0 means LOW trigger.
        MCUCR &= ~(bit(ISC01) | bit(ISC00)); // INT0 on low level trigger
}


//
// ========== LOOP
//
void loop (){

        // To measure the time (delay function), we need to active the timer0.
        power_timer0_enable ();
        delay (100); // let timer reach a known point
        digitalWrite (LED_pin, HIGH);
        delay (20);
        digitalWrite (LED_pin, LOW);
        power_timer0_disable (); // Disable the timer0

        while (digitalRead (SW_pin) == LOW) {
                // wait for SW_pin to be released
        }

        // clear interrupts flag, equal to cli
        noInterrupts (); // disable all interrupts (including timer interrupts, serial interrupts, etc.)

        // General Interrupt Flag Register
        // When an edge or logic change on the INT0 pin triggers an interrupt request, INTF0 becomes set (one).
        // Bit 6 – INTF0: External Interrupt Flag 0
        GIFR  = bit (INTF0);// clear interrupt flag

        // General Interrupt Mask Register
        // Bit 6 – INT0: External Interrupt Request 0 Enable
        GIMSK = bit (INT0); // enable INT0

        power_all_disable (); // power off ADC, Timer 0 and 1, serial interface
        set_sleep_mode (SLEEP_MODE_PWR_DOWN);
        sleep_enable (); // ready to sleep
        interrupts (); // enable all interrupts (including timer interrupts, serial interrupts, etc.)
        sleep_cpu ();    // sleep
        sleep_disable (); // precaution

}
