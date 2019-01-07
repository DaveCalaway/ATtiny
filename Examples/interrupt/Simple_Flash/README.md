# Interrupt and Sleep Mode
In this tutorial we will cover the case when you want to use an interrupt ( over all pins ) during the Deep Sleep Mode ( SLEEP_MODE_PWR_DOWN ).  

**Remember**:  
This tutorial is made for the ATtiny85/45/25, but the same steps are uses over other AVR MicroController, for example Arduino R3.  

### Interrupt
We use interrupts to immediately take the control of the MicroController.  
If the MicroController are working in a specific line of code, but an event ( interrupt ) arrived, we want to move immediately at the function that the interrupt want.  
After the interrupt's function, we return at the last point in the normal code we left.  

### Interrupts in normal mode
In the Arduino, in normal operation mode ( not in sleepMode ), we use the **attachInterrupt()** function to enable the interrupt over a specific pins. For the Arduino R3 we have 2 interrupts pins IN0 (D1) and IN1 (D2).  

As the [reference page](https://goo.gl/DGR2bH) says the **attachInterrupt(digitalPinToInterrupt(pin), ISR, mode);** is the syntax.  

```
const byte ledPin = 13;
const byte interruptPin = 2;
volatile byte state = LOW;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
}

void loop() {
  digitalWrite(ledPin, state);
}

void blink() {
  state = !state;
}
```  

In this example we have set the "mode" to CHANGE. We can decide what types of mode use:  
- LOW to trigger the interrupt whenever the pin is low  
- CHANGE to trigger the interrupt whenever the pin changes value  
- RISING to trigger when the pin goes from low to high  
- FALLING for when the pin goes from high to low  

Very useful, but we can use them only in the normal mode, that is, when the Clock run.  

### Interrupt in sleepMode
We use the sleepMode for put the Device in low power consumption.  
Image you want to connect your Device in the garden, we need a battery. If your Device wake-up one a hours, the rest of the time is useful to put the Device in sleepMode to preserve the battery.  

**How can enable the sleepMode?**
In Arduino R3, use this well done library: https://goo.gl/IxDhY  
In the ATtiny we use the follow basic example:  

```
#include <avr/sleep.h>    // Sleep Modes https://goo.gl/Mm7wuk
#include <avr/power.h>    // Power management https://goo.gl/g6bHHz

void setup (){
        pinMode (LED_pin, OUTPUT);
        pinMode (SW_pin, INPUT_PULLUP);

        // Set the interrupt
        attachInterrupt(digitalPinToInterrupt(interruptPin), interrupt, LOW);
}

void loop (){
  digitalWrite (LED_pin, HIGH);
  delay (2000);
  digitalWrite (LED_pin, LOW);
  delay (2000);

  // Turn off ADC
  // You must use the PRR after setting ADCSRA to zero,
  // otherwise the ADC is "frozen" in an active state.
  ADCSRA = 0;      // turn off ADC, we don't need it, it's consume alot.

  power_all_disable (); // power off ADC, Timer 0 and 1, serial interface
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);
  sleep_enable (); // ready to sleep
  interrupts (); // enable all interrupts (including timer interrupts, serial interrupts, etc.)
  sleep_cpu ();    // sleep

  // now the device are sleeping

  sleep_disable (); // precaution
  power_all_enable (); // power everything back on
}

void interrupt(){
// do something
}
```
