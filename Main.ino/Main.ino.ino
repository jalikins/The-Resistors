#include "Functions-constants.h"

void setup() {
  Serial.begin(9600);

  left_stepper.setMaxSpeed(250); // change this if going too fast
  left_stepper.setAcceleration(200);

  for (unsigned int i=0; i<sizeof(ACTUATORPIN); i++) {
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
      for (unsigned int i = 0; i < sizeof(bins)/sizeof(bins[0]); ++i) {
        bins[i] = 0;
      }
    }
  }

  // Serial.print("beltPos=");
  // Serial.print(beltPos);
  
  if (limitSwitch == true) {
    // Choose reference resistance value and take measurement
    const int constResistance = measureVoltage();
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

// Test actuators
// #include "Functions-constants.h"

// void setup() {
//   for (i=0; i<sizeof(ACTUATORPIN); i++) {
//     pinMode(ACTUATORPIN[i], OUTPUT);
//   }
//   i = 0;
// }

// void loop() {
//   digitalWrite(ACTUATORPIN[i], HIGH);
//   delay(250);
//   digitalWrite(ACTUATORPIN[i], HIGH);
//   delay(250);
//   i = (i + 1)%14;
// }