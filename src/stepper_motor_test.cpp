#include <math.h>
#include <Servo.h> // Include the Servo library
#include <string.h>
#include "Arduino.h"
#include "bin_finder.h"
#include <Stepper.h>


// steps per revolution
const int STEPS_PER_REVOLUTION = 513;

// motor driver pin connections
const int IN1_PIN = 8;
const int IN2_PIN = 9;
const int IN3_PIN = 10;
const int IN4_PIN = 11;


//initialize Stepper library
Stepper myStepper(STEPS_PER_REVOLUTION, IN1_PIN, IN3_PIN, IN2_PIN, IN4_PIN);


void setup() {
  Serial.begin(9600);
  Serial.println("Stepper motor example");

  // set motor speed
  myStepper.setSpeed(12);
}

void loop() {

  // turning 1 rotation (513 ticks) clockwise
  myStepper.step(STEPS_PER_REVOLUTION);
  delay(1000);

  // turning 100 ticks counterclockwise
  myStepper.step(-100);
  delay(1000); 

  // turning 100 ticks clockwise
  myStepper.step(100);
  delay(2000); 
}