#include <Stepper.h> // Include stepper library
#include <AccelStepper.h>
#include <Arduino.h>
#include <avr/io.h>
#include <math.h>
#include <string.h>

// -- PIN DEFINITIONS -- //

// Actuators in order (starting with actuator 1)
const uint8_t ACTUATORPIN [] = {
  16,
  18,
  17,
  4,
  5,
  6,
  7,
  8,
  9,
  10,
  11,
  12,
  13,
  19
};

// Resistor measurement
const int RESISTORPIN = 30;

// MUX
const int MUXA = 37;
const int MUXB = 38;
const int MUXC = 39;
const int MUXD = 40;

// Stepper motor
const int DIRPIN = 3;
const int STEPPIN = 2;

// Button 
const int BUTTONPIN = 21;

// Timing switch
const int TXDPIN = 14;
const int RXDPIN = 15;


// Stepper speed
const int BELTSPEED;

// Net measuring voltage
const int VOLTAGEIN = 5;

// The known resistor values to cycle through in kOhms
double referenceValue [16] = {
  0.05,
  0.1,
  0.2,
  0.5,
  1,
  2, 
  5,
  10,
  20,
  50,
  100,
  200,
  500,
  1000,
  2000,
  5000,
};

const uint8_t MOTORINTERFACETYPE = 1;

// steps per revolution
const double PERC_UNCERTAINTY = 0.6;
const double HITHRESHOLD = 4.0;


const uint8_t FIRSTBIN = 5; // Modules from measurement to the first bin

// Equal to total number of bins
const int CATCHALLBIN = 14;

uint8_t whichBin = CATCHALLBIN;

// Counts the number of steps (resets each full revolution)
unsigned int beltPos = 0;

// Calibrate where on module to start taking measurements
unsigned long measureOffset = 40;
double voltageMeas = 0;
double varResistance = 0;

int bins [CATCHALLBIN];

// Iterators
unsigned int n=0;

bool limitSwitch = false;
bool catchAll = 0;
// Serial inputs
String input = "";

// Stores an unused element index for each row in binOrder
uint8_t binIndex [14] = {
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0
};
// Define 2D array with rows corresponding to bins and columns corresponding 
// to the belt position when a measured resister must be dropped
int binOrder[14][10] = {
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

AccelStepper left_stepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);

// Determine which actuators to trigger at a given belt position
void actuate(unsigned int beltPos, int binOrder[14][10]) {
  for (uint8_t n=0; n<=sizeof(ACTUATORPIN); n++) {
    for (uint8_t i=0; i<=sizeof(binOrder[0]); i++) {
        if (beltPos - binOrder[0][i] == 0) {
            digitalWrite(ACTUATORPIN[n], LOW);
            break;
        }
      left_stepper.runSpeed();
    }
  }
}

// Return voltage across mystery resistor and reference resistor value
int measureVoltage() {
  // voltage reading constants
  const int LOWTHRESHOLD = 1;
  const int VOLTAGECONVERSION = 1030;

  // Reset catchAll boolean
  catchAll = false;
  digitalWrite(MUXA, LOW);
  digitalWrite(MUXB, LOW);
  digitalWrite(MUXC, LOW);
  digitalWrite(MUXD, LOW);
  // Read signal across unknown resistor
  voltageMeas = analogRead(RESISTORPIN);
  // Convert to voltage
  voltageMeas = VOLTAGEIN*voltageMeas/VOLTAGECONVERSION;
  // Serial.print("voltageMeas=");
  // Serial.println(voltageMeas);

  // Assign to catch all bin if resistance is too small to measure
  if (voltageMeas < LOWTHRESHOLD) {
    catchAll = true;
  }

  double constResistance = referenceValue[0];
  uint8_t iterationCount = 1;
  // Step down reference resistor values until receiving on a good signal
  while (voltageMeas > HITHRESHOLD) {
    // left_stepper.runSpeed();
    if (iterationCount > sizeof(referenceValue)) {
      catchAll = true;
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
    if (iterationCount%8 == 0) {
      digitalWrite(MUXD, !digitalRead(MUXD));
    }
    
    // Take new voltage reading
    voltageMeas = analogRead(RESISTORPIN);
    voltageMeas = VOLTAGEIN*voltageMeas/VOLTAGECONVERSION;
    // Serial.print("voltageMeas=");
    // Serial.println(voltageMeas);
    // Update the reference resistance value
    constResistance = referenceValue[iterationCount];
    
    // Assign to catch all bin if all reference values are tested
    iterationCount += 1;
  }
  return constResistance;
};

// Return which bin to drop measured resistor
int binFinder(double constResistance, double varResistance) {
  whichBin = CATCHALLBIN;
  if (catchAll == false) {
    // Determine the raw uncertainty in bin value
    int binThreshold = round(PERC_UNCERTAINTY * varResistance);

    for (uint8_t i=0; i<CATCHALLBIN; i++) {
      whichBin = i + 1;
      if (bins[i] == 0) {
        // Assign a new bin
        bins[i] = varResistance;
        Serial.print("New Bin ");
        Serial.print(whichBin);
        Serial.print(": ");
        Serial.println(varResistance);
        break;
      }
      else if (bins[i] - binThreshold < varResistance && varResistance < bins[i] + binThreshold) {
        // Put in existing bin
        Serial.print(whichBin);
        Serial.print(':');
        Serial.println(varResistance);
        break;
      }
      // Last bin catch all
      if (whichBin == CATCHALLBIN) {
        catchAll == true;
      }
    }
    // left_stepper.runSpeed();
  }
  if (catchAll == true) {
    whichBin = CATCHALLBIN;
    Serial.print("Catch All Bin");
  }
  return whichBin;
}