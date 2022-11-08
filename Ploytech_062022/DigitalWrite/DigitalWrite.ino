/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/DigitalReadSerial
*/

// digital pin 2 has a pushbutton attached to it. Give it a name:
int IO2 = 2;

// the setup routine runs once when you press reset:
void setup() {
  // initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
  // make the pushbutton's pin an input:
  pinMode(IO2, OUTPUT);
}

// the loop routine runs over and over again forever:
void loop() {
  // write the input pin:
  digitalWrite(IO2,1);
  delay(5);
  // print out the state of the button:
  digitalWrite(IO2,0);
  delay(1);        // delay in between reads for stability
}
