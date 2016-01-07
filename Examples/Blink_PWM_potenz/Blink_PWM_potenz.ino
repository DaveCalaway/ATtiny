/* Program for ATtiny85.
   Comanda la luminosità d'un led tramite un potenziometro.
*/

const int ledPin = 0;      // pin 5 tiny
const int analogPin = A1;  // pin 7 tiny
int val = 0;         // variable to store the read value

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(analogPin, INPUT);
}


void loop() {
  val = analogRead(analogPin);   // read the input pin
  analogWrite(ledPin, val / 4);  // analogRead values go from 0 to 1023, analogWrite values from 0 to 255
}
