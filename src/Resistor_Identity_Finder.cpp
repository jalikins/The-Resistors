#include <MeasureResistance.h>


void setup() {
  timer = millis();
  Serial.begin(9600);

  servo1.attach(SERVOPIN1);
  servo2.attach(SERVOPIN2);
  servo3.attach(SERVOPIN3);
  servo4.attach(SERVOPIN4);


  pinMode(RESISTORPIN, INPUT);
  pinMode(MUXA, OUTPUT);
  pinMode(MUXB, OUTPUT);
  pinMode(MUXC, OUTPUT);


}

void loop() {
  servo1.write(0);
  servo2.write(0);
  servo3.write(0);
  servo4.write(0);
  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);

  beltPos = BELTSPEED * timer / 1000;

  // Replace with commented code below
  if (round(beltPos % MODULESTEPS - measureOffset) == 0) {
    whichBin = binFinder();
    // Input number of steps to the desired bin plus the current belt position
    binOrder[whichBin][binIndex[whichBin - 1]] = beltPos + MODULESTEPS*(whichBin+FIRSTBIN);
    actuateServo(beltPos);
  }
  // if (Serial.available()) {
  //   input = Serial.readStringUntil('\n');
  //   input.trim();
  //   if (input == "measure") {
  //     whichBin = binFinder();
  //     // Belt position still needs to be defined using steppers 
  //     binOrder[whichBin] = binOrder[whichBin].push_back(beltPos + moduleWidth*(whichBin+FIRSTBIN))
  //     actuateServo(beltPos)
  //   }
  // }
}