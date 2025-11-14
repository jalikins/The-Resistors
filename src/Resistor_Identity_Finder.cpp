#include <Servo.h> // Include the Servo library
#include <Stepper.h> // Include stepper library
#include <string.h>
#include <Arduino.h>
#include <bin_finder.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

//initialize Stepper library
Stepper myStepper(STEPS_PER_REVOLUTION, IN1_PIN, IN3_PIN, IN2_PIN, IN4_PIN);

void setup() {
  timer = millis();
  Serial.begin(9600);

  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);

  // set motor speed
  myStepper.setSpeed(beltSpeed);

  pinMode(resistorPin, INPUT);
  pinMode(muxA, OUTPUT);
  pinMode(muxB, OUTPUT);
  pinMode(muxC, OUTPUT);

  binOrder[]
}

void loop() {
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  digitalWrite(muxA, LOW);
  digitalWrite(muxB, LOW);
  digitalWrite(muxC, LOW);

  beltPos = beltSpeed * timer / 1000;

  // Replace with commented code below
  if (round(beltPos % moduleSteps - measureOffset) == 0) {
    whichBin = binFinder();
      // Belt position still needs to be defined using steppers 
      binOrder[whichBin] = binOrder[whichBin].push_back(beltPos + moduleSteps*(whichBin+firstBin))
      actuateServo(beltPos)
  }
  // if (Serial.available()) {
  //   input = Serial.readStringUntil('\n');
  //   input.trim();
  //   if (input == "measure") {
  //     whichBin = binFinder();
  //     // Belt position still needs to be defined using steppers 
  //     binOrder[whichBin] = binOrder[whichBin].push_back(beltPos + moduleWidth*(whichBin+firstBin))
  //     actuateServo(beltPos)
  //   }
  // }
}