#include <math.h>
#include <Servo.h> 
#include <string.h>
#include "Arduino.h"
#include <Stepper.h>


// motor driver pin connections
const int dirPin = 8; 
const int stepPin = 9; 

// number of ticks per revolution
const int ticks = 516;

// change this to change speed!
const int rpm = 40;

// calculating delay per step needed to keep at rpm
const long delayPerStep = (60L * 1000L * 1000L) / (ticks * rpm);


// rotates stepper clockwise
void clockwise(int dirPin, int stepPin) {
  // set direction pin to clockwise
  digitalWrite(dirPin, LOW); 

  // loop for full revolution
  for (int i = 0; i < ticks; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayPerStep / 2);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayPerStep / 2);
  }
}

// turns stepper counterclockwise
void counterclockwise(int dirPin, int stepPin) {
  // direction counterclockwise
  digitalWrite(dirPin, HIGH); 
  // loop for full revolution
  for (int i = 0; i < ticks; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(delayPerStep / 2);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(delayPerStep / 2);
  }
}

void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  counterclockwise(dirPin, stepPin);

}
