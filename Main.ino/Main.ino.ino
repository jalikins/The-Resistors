#include "Functions-constants.h"

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600); // for PC0 and PC1

  belt_stepper.setMaxSpeed(BELTSPEED); // change this if going too fast
  belt_stepper.setAcceleration(200);

  PT_INIT(&ptActuate);
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
  if (Serial1.available()) {
    input = Serial1.readStringUntil('\n');
    input.trim(); 
    if (input == "go") {
      belt_stepper.setSpeed(BELTSPEED);
      Serial.print("going");
    }
    
    if (input == "stop") {
      belt_stepper.setSpeed(0);
      Serial.print("stopping");
    }
    if (input == "zero") {
      Serial.print("resetting");
      // Reset bins
      for (unsigned int i = 0; i < sizeof(bins)/sizeof(bins[0]); ++i) {
        bins[i] = 0;
      }
    }
    // Change limitswitch to false between modules
    if (input == "limitoff") {
      // Progress belt position, bin index, and actuate when a new module is reached
      if (limitSwitch == true) {
        (beltPos++)%MODULENUM;
        (binIndex[whichBin - 1]++)%CATCHALLBIN;
        PT_SCHEDULE(actuate(&ptActuate));
      }
      limitSwitch == false;
    }
    // Make sure limitswitch is true when on a module 
    if (input == "limit") {
      limitSwitch = true;
    }

    if (input.startsWith("setbins:")) { // for this to work the format needs to be "setbins:100,220,330,470,1000,2000"
      
      String data = input.substring(8); //deletes the first 7 values
      
      int currentBin = 0;
      
      while (data.length() > 0 && currentBin < CATCHALLBIN) {
        int commaIndex = data.indexOf(','); // creates a true and false data set
        
        if (commaIndex != -1) {
          // Extract number before the comma
          String valStr = data.substring(0, commaIndex);
          bins[currentBin] = valStr.toDouble(); 
          
          data = data.substring(commaIndex + 1); // delete the number
        } else {
          // Take the last number 
          bins[currentBin] = data.toDouble();
          data = ""; // Empty string to end loop
        }
        currentBin++;
      }
    }
  }

  // limit switch will need to be a serial command sent from external arduino
  if (limitSwitch == true) {
    // Choose reference resistance value and take measurement
    const int constResistance = measureVoltage(); // will need to send a signal to an arduino
    // Calculate the resistance of unknown resistor
    varResistance = voltageMeas*constResistance/(VOLTAGEIN - voltageMeas); // this si the actual resistor value
    // varResistance = round(varResistance);
    // Input number of modules to the desired bin plus the current belt position
    whichBin = binFinder(constResistance, varResistance);
    binOrder[whichBin][binIndex[whichBin - 1]] = beltPos + FIRSTBIN + whichBin - 1; // add to bin order take the bin where in the row should the resistor be placed
  }
  belt_stepper.runSpeed();
  
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