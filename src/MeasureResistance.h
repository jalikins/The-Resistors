#include <Servo.h> // Include the Servo library
#include <Stepper.h> // Include stepper library
#include <Arduino.h>
#include <math.h>
#include <string.h>

Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

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
const double perc_uncertainty = 0.2;
const double hiThreshold = 4.0;
// Speed in steps per second
const int beltSpeed = 12;
const int moduleSteps = STEPS_PER_REVOLUTION/8;
// Steps from measurement to the first bin
const int firstBin = moduleSteps*5;
const uint8_t nonbinModules = 4;
const uint8_t servoPin1 = 1;
const uint8_t servoPin2 = 2;
const uint8_t servoPin3 = 3;
const uint8_t servoPin4 = 4;
const int catchAllBin = 4;
const int resistorPin = A0;
const int muxA = 2;
const int muxB = 3;
const int muxC = 4;
const int voltageIn = 5;
const int lowThreshold = 1;
const int voltageConversion = 1030;
const int servoAngle = 10;



// motor driver pin connections
const int IN1_PIN = 8;
const int IN2_PIN = 9;
const int IN3_PIN = 10;
const int IN4_PIN = 11;

unsigned long timer = 0;
// Counts the number of steps
unsigned long beltPos = 0;
// Needs to be assigned (calibrate where to measure)
unsigned long measureOffset = 0;
double voltageMeas = 0;
double varResistance = 0;
double constResistance = 0;
int bins [catchAllBin];
int i=0;
int binThreshold = 0;
int iterationCount = 1;
bool catchAll = 0;
// String input = "";

int whichBin = catchAllBin;
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
            servo1.write(servoAngle);
        }
    }
    for (i=0; i<=sizeof(binOrder[1]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo2.write(servoAngle);
        }
    }
    for (i=0; i<=sizeof(binOrder[2]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo3.write(servoAngle);
        }
    }
    for (i=0; i<=sizeof(binOrder[3]); i+=1) {
        if (round(beltPos - i) == 0) {
            servo4.write(servoAngle);
        }
    }
}

int binFinder() {
  whichBin = 0;
  iterationCount = 1;
  digitalWrite(muxA, LOW);
  digitalWrite(muxB, LOW);
  digitalWrite(muxC, LOW);
  // Read signal across unknown resistor
  voltageMeas = analogRead(resistorPin);
  // Convert to voltage
  voltageMeas = voltageIn*voltageMeas/voltageConversion;
  Serial.print("voltageMeas=");
  Serial.println(voltageMeas);
  constResistance = referenceValue[0];
  
  // Assign to catch all bin if resistance is too small to measure
  if (voltageMeas < lowThreshold) {
    catchAll = 1;
    whichBin = catchAllBin;
    Serial.println("CatchAll");
  }

  // Step down reference resistor values until receiving on a good signal
  while (voltageMeas > hiThreshold) {
    if (iterationCount > 8) {
      catchAll = true;
      whichBin = catchAllBin;
      Serial.print("CatchAll");
      break;
    }
    // Progress mux converter output
    digitalWrite(muxA, !digitalRead(muxA));
    if (iterationCount%2 == 0) {
      digitalWrite(muxB, !digitalRead(muxB));
    }
    if (iterationCount%4 == 0) {
      digitalWrite(muxC, !digitalRead(muxC));
    }
    
    // Take new voltage reading
    voltageMeas = analogRead(resistorPin);
    voltageMeas = voltageIn*voltageMeas/voltageConversion;
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
    varResistance = voltageMeas*constResistance/(voltageIn - voltageMeas);
    varResistance = round(varResistance);
    // Determine the raw uncertainty in bin value
    binThreshold = perc_uncertainty * varResistance;

    for (i=0; i<catchAllBin; i+=1) {
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