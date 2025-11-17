#include <MeasureResistance.h>
#include <AccelStepper.h>

AccelStepper left_stepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);

void setup() {
  timer = millis();
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
  // if (Serial.available()) {
  //   input = Serial.readStringUntil('\n');
  //   input.trim();
  //   if (input == "go") {
  //     left_stepper.setSpeed(30);
  //     Serial.print("going");
  //   }
    
  //   if (input == "stop") {
  //     left_stepper.setSpeed(0);
  //     Serial.print("stopping");
  //   }
  // }
  left_stepper.setSpeed(30);
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);

  beltPos = left_stepper.currentPosition();
  Serial.print("beltPos=");
  Serial.print(beltPos);

  // Replace with commented code below
  if (round(beltPos % MODULESTEPS - measureOffset) == 0) {
    whichBin = binFinder();
    // Input number of steps to the desired bin plus the current belt position
    binOrder[whichBin][binIndex[whichBin - 1]] = beltPos + MODULESTEPS*(whichBin+FIRSTBIN);
    actuateServo(beltPos);
  }

  left_stepper.runSpeed();

}