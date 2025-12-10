#include <MeasureResistance.h>

void setup() {
  timer = millis() + 1;
  Serial.begin(9600);

  left_stepper.setMaxSpeed(250); // change this if going too fast
  left_stepper.setAcceleration(200);

  for (i=0; i<sizeof(ACTUATORPIN); i++) {
    pinMode(ACTUATORPIN[i], OUTPUT);
  }

  pinMode(RESISTORPIN, INPUT);

  pinMode(MUXA, OUTPUT);
  pinMode(MUXB, OUTPUT);
  pinMode(MUXC, OUTPUT);
  pinMode(MUXD, OUTPUT);

  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);
  digitalWrite(MUXD, LOW);
}

void loop() {
  timer = millis() + timeOffset;
  if (Serial.available()) {
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
    if (input == "bins") {
      for (i=0; i<sizeof(binVals); i++) {
        bins[i] = binVals[i];
      }
      Serial.print("bins assigned");
    }
  }

  // Serial.print("beltPos=");
  // Serial.print(beltPos);
  // Serial.print("timer=");
  // Serial.print(timer);
  
  if (limitSwitch == true) {
    constResistance, voltageMeas = measureVoltage();
    // Calculate the resistance of unknown resistor
    varResistance = voltageMeas*constResistance/(VOLTAGEIN - voltageMeas);
    // varResistance = round(varResistance);
    // Input number of modules to the desired bin plus the current belt position
    whichBin = binFinder(constResistance, varResistance);
    binOrder[whichBin][binIndex[whichBin - 1]] = beltPos + FIRSTBIN + whichBin - 1;
    actuate(beltPos, binOrder);
  }

  left_stepper.runSpeed();

}