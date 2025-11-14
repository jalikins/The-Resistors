#include <Servo.h> // Include the Servo library
#include <string.h>
#include <Arduino.h>
#include <bin_finder.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;


void setup() {
  servo1.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);
  servo4.attach(servoPin4);
  Serial.begin(9600);

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
  // Wait to measure until user types "measure" into serial monitor
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "measure") {
      whichBin = binFinder();
      // Belt position still needs to be defined using steppers 
      binOrder[whichBin] = binOrder[whichBin].push_back(beltPos + moduleWidth*(whichBin+firstBin))
      actuateServo(beltPos)
    }
  }
}