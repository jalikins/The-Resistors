#include <MeasureResistance.h>

void setup() {
  timer = millis() + 1;
  Serial.begin(9600);

  servo1.attach(SERVOPIN1);
  servo2.attach(SERVOPIN2);
  servo3.attach(SERVOPIN3);
  servo4.attach(SERVOPIN4);
  left_stepper.setMaxSpeed(250); // change this if going to fast
  left_stepper.setAcceleration(200);

  pinMode(RESISTORPIN, INPUT);
  pinMode(MUXA, OUTPUT);
  pinMode(MUXB, OUTPUT);
  pinMode(MUXC, OUTPUT);


}

void loop() {
  timer = millis() + timeOffset;
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "go") {
      left_stepper.setSpeed(BELTSPEED);
      Serial.print("going");
    }
    
    if (input == "stop") {
      left_stepper.setSpeed(0);
      Serial.print("stopping");
    }
    if (input == "zero") {
      Serial.print("resetting");
      // Reset bins
      for (i = 0; i < sizeof(bins)/sizeof(bins[0]); ++i) {
        bins[i] = 0;
      }
      // Reset timer to 1
      timeOffset = 2 - timer;
    }
  }
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);

  // beltPos = left_stepper.currentPosition();
  beltPos = -BELTSPEED*timer/1000;
  // Serial.print("beltPos=");
  // Serial.print(beltPos);
  // Serial.print("timer=");
  // Serial.print(timer);
  

  // Replace with commented code below
  if (round((beltPos - measureOffset) % MODULESTEPS) == 0) {
    whichBin = binFinder();
    // Input number of steps to the desired bin plus the current belt position
    binOrder[whichBin][binIndex[whichBin - 1]] = beltPos + MODULESTEPS*(whichBin+FIRSTBIN);
    actuateServo(beltPos);
  }

  left_stepper.runSpeed();

}