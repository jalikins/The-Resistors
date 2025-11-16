#include <Servo.h> // Include the Servo library
#include <Stepper.h> // Include stepper library
#include <Arduino.h>
#include <math.h>
#include <string.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

// -- PIN DEFINITIONS -- //

// Servo pin definitions
const uint8_t SERVOPIN1 = 1;
const uint8_t SERVOPIN2 = 2;
const uint8_t SERVOPIN3 = 3;
const uint8_t SERVOPIN4 = 4;

// Resistor measurement pin definition
const int RESISTORPIN = A0;

// MUX pin definitions
const int MUXA = 2;
const int MUXB = 3;
const int MUXC = 4;

// Stepper motor pin definitions
const int DIRPIN = 8; 
const int STEPPIN = 9; 


// The known resistor values to cycle through in kOhms
double referenceValue [] = {
  0.05,
  0.1,
  0.5,
  1,
  10,
  20,
  50,
  100
};


// steps per revolution
const int STEPS_PER_REVOLUTION = 513;
const double PERC_UNCERTAINTY = 0.2;
const double HITHRESHOLD = 4.0;
// Speed in steps per second
const int BELTSPEED = 12;
const int MODULESTEPS = STEPS_PER_REVOLUTION/8;


const int FIRSTBIN = MODULESTEPS*5; // Steps from measurement to the first bin

const uint8_t NONBINMODULES = 4; // can be deleted




const int CATCHALLBIN = 4;

// voltage reading constants
const int VOLTAGEIN = 5;
const int LOWTHRESHOLD = 1;
const int VOLTAGECONVERSION = 1030;
const int SERVOANGLE = 10;


unsigned long timer = 0;

// Counts the number of steps
unsigned long beltPos = 0;
// Needs to be assigned (calibrate where to measure)
unsigned long measureOffset = 0;
double voltageMeas = 0;
double varResistance = 0;
double constResistance = 0;
int bins [CATCHALLBIN];
int i=0;
int binThreshold = 0;
int iterationCount = 1;
bool catchAll = 0;
// String input = "";

int whichBin = CATCHALLBIN;
// Stores an unused element index for each row in binOrder
uint8_t binIndex[] = {
  0,
  0,
  0,
  0
};
// Define 2D array with rows corresponding to bins and columns corresponding 
// to resistors assigned to that bin
int binOrder[4][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int actuateServo(float beltPos) {
    for (i=0; i<=sizeof(binOrder[0]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo1.write(SERVOANGLE);
        }
    }
    for (i=0; i<=sizeof(binOrder[1]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo2.write(SERVOANGLE);
        }
    }
    for (i=0; i<=sizeof(binOrder[2]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo3.write(SERVOANGLE);
        }
    }
    for (i=0; i<=sizeof(binOrder[3]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo4.write(SERVOANGLE);
        }
    }
}

int binFinder() {
  whichBin = 0;
  iterationCount = 1;
  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);
  // Read signal across unknown resistor
  voltageMeas = analogRead(RESISTORPIN);
  // Convert to voltage
  voltageMeas = VOLTAGEIN*voltageMeas/VOLTAGECONVERSION;
  Serial.print("voltageMeas=");
  Serial.println(voltageMeas);
  constResistance = referenceValue[0];
  
  // Assign to catch all bin if resistance is too small to measure
  if (voltageMeas < LOWTHRESHOLD) {
    catchAll = 1;
    whichBin = CATCHALLBIN;
    Serial.println("CatchAll");
  }

  // Step down reference resistor values until receiving on a good signal
  while (voltageMeas > HITHRESHOLD) {
    if (iterationCount > 8) {
      catchAll = true;
      whichBin = CATCHALLBIN;
      Serial.print("CatchAll");
      break;
    }
    // Progress mux converter output
    digitalWrite(MUXA, !digitalRead(MUXA));
    if (iterationCount%2 == 0) {
      digitalWrite(MUXB, !digitalRead(MUXB));
    }
    if (iterationCount%4 == 0) {
      digitalWrite(MUXC, !digitalRead(MUXC));
    }
    
    // Take new voltage reading
    voltageMeas = analogRead(RESISTORPIN);
    voltageMeas = VOLTAGEIN*voltageMeas/VOLTAGECONVERSION;
    Serial.print("voltageMeas=");
    Serial.println(voltageMeas);
    // Update the reference resistance value
    constResistance = referenceValue[iterationCount];
    
    // Assign to catch all bin if all reference values are tested
    iterationCount += 1;
  }
  
  // Assign resistors outside of range to catch all bin


  if (catchAll == false) {
    // Calculate the resistance of unknown resistor
    varResistance = voltageMeas*constResistance/(VOLTAGEIN - voltageMeas);
    varResistance = round(varResistance);
    // Determine the raw uncertainty in bin value
    binThreshold = PERC_UNCERTAINTY * varResistance;

    for (i=0; i<CATCHALLBIN; i+=1) {
      if (bins[i] == 0) {
        // Assign a new bin
        bins[i] = varResistance;
        whichBin = i + 1;
        Serial.print("New Bin ");
        Serial.print(whichBin);
        Serial.print(": ");
        Serial.println(varResistance);
        break;
      }
      else if (bins[i] - binThreshold < varResistance && varResistance < bins[i] + binThreshold) {
        // Put in existing bin
        whichBin = i + 1;
        Serial.print(whichBin);
        Serial.print(':');
        Serial.println(varResistance);
        break;
      }
      // Last bin catch all
      whichBin = i + 1;
    }
  }
  // Reset catchAll boolean
  catchAll = false;
  return whichBin;
}

