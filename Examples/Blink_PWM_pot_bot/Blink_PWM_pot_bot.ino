/* Program for ATtiny85.
   Comanda la luminosità d'un led tramite un potenziometro.
   Tenendo premuto il bottone si imposti la luminosità.
   Rilasciando il bottone si manterrà la luminosità.
*/

const int ledPin = 0;      // pin 5 tiny
const int analogPin = A1;  // pin 7 tiny
const int button = 4; // pin 3 tiny
int val = 0;         // variable to store the read value

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(analogPin, INPUT);
  pinMode(button, INPUT);
}


void loop() {
  while (digitalRead(button) == 1) {
    val = analogRead(analogPin);   // read the input pin
    analogWrite(ledPin, val / 4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
  }
}
