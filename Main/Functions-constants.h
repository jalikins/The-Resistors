#include <Stepper.h> // Include stepper library
#include <AccelStepper.h>
#include <Arduino.h>
#include <avr/io.h>
#include <math.h>
#include <string.h>

#include "Acuator.h"
#include "Sorting.h" // includes Pins and Constants resistor, MUX, and motor 
#include "communication.h" // external Communication



const int BIN_DIST = 19; // how many modules from the measurement to the first bin + total bins
const int FIRSTBIN = 5;

// steps per revolution
const double PERC_UNCERTAINTY = 0.6;
const double HITHRESHOLD = 4.0;


// Equal to total number of bins
const uint8_t CATCHALLBIN = 14;
// Total number of modules
const int MODULENUM = 36;

uint8_t whichBin = CATCHALLBIN;

// Counts the number of steps (resets each full revolution)
unsigned int beltPos = 0;

// Calibrate where on module to start taking measurements
unsigned long measureOffset = 40;
double voltageMeas = 0;
double varResistance = 0;

double bins [CATCHALLBIN];

bool catchAll = 0;
// Serial inputs
String input = "";
String inputBuffer = "";

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
int binOrder[14][19] = {
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

AccelStepper belt_stepper(MOTORINTERFACETYPE, STEPPIN, DIRPIN);




bool running = false; 
unsigned long laststall_tim = 0; 
const unsigned long stallDelay = 1000;
unsigned long prev_count_state = 0; 

void keep_going(){ 
  belt_stepper.runSpeed();
  
  if (running) {
    if ((millis() - laststall_tim) > stallDelay) {
      
      // If the count hasn't changed in 1 second, we are stalled
      if (prev_count_state == clickCount) {
        // Stall handling
        belt_stepper.setSpeed(0); // Stop
        Serial1.println("stalled"); 
        delay(200);               // Brief pause
        belt_stepper.setSpeed(BELTSPEED); // Restart
        belt_stepper.runSpeed();

      }
      
      prev_count_state = clickCount;
      laststall_tim = millis(); 
    }
  }
}



// Determine which actuators to trigger at a given belt position //(sizeof(ACTUATORPIN)/sizeof(ACTUATORPIN[0]))
void actuateThread() { // decides when to run which acutator

  for (uint8_t k=0; k<CATCHALLBIN; k++) {
    digitalWrite(ACTUATORPIN[k], LOW);
    belt_stepper.runSpeed();
  }
  for (uint8_t i=0; i < NUM_ACTUAT; i++) {
    for (uint8_t j=0; j < BIN_DIST; j++) {
        if (clickCount - binOrder[j][i] == 0) {
            digitalWrite(ACTUATORPIN[j], HIGH);
            break;
        }
      belt_stepper.runSpeed();
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
    belt_stepper.runSpeed();
    if (iterationCount > 16) {
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
    voltageMeas = analogRead(RESISTORPIN); // look at resistor pin
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
    int binThreshold = round(PERC_UNCERTAINTY * varResistance); // should this go in a bin or not

    for (uint8_t i=0; i<CATCHALLBIN; i++) { // creating a new bin 
      whichBin = i + 1;
      // if (bins[i] == 0) {
      //   // Assign a new bin
      //   bins[i] = varResistance; // all the raspit ahs to do is change the bins
      //   Serial.print("New Bin ");
      //   Serial.print(whichBin);
      //   Serial.print(": ");
      //   Serial.println(varResistance);
      //   break;
      // }
      if (bins[i] - binThreshold < varResistance && varResistance < bins[i] + binThreshold) {
        // Put in existing bin
        Serial1.print(whichBin);
        Serial1.print(':');
        Serial1.println(varResistance);
        break;
      }
      // Last bin catch all
      if (whichBin == CATCHALLBIN) {
        catchAll = true;
      }
    }
    belt_stepper.runSpeed();
  }
  if (catchAll == true) {
    whichBin = CATCHALLBIN;
    Serial1.print("Catch All Bin");
  }
  return whichBin;
}




void processCommand(String input) {
  if (input == "go") {
    belt_stepper.setSpeed(BELTSPEED);
    Serial1.println("going"); 
    running = true;
  }
  
  else if (input == "stop") {
    belt_stepper.stop();
    belt_stepper.setSpeed(0 );
    Serial1.println("stopping"); 
    running = false;
  }

  if (input == "up") {
    for (int i=0; i<NUM_ACTUAT; i++){
      Serial1.println("Lifting Acutators");
      digitalWrite(ACTUATORPIN[i], HIGH);
      belt_stepper.runSpeed();
    }
  }

  if (input == "down") {
    for (int i=0; i<NUM_ACTUAT; i++){
      Serial1.println("Droping Actuators");
      digitalWrite(ACTUATORPIN[i], LOW);
      belt_stepper.runSpeed();
    }
  }

  if (input == "zero") {
    Serial1.println("resetting");
    // Reset bins
    for (unsigned int i = 0; i < sizeof(bins)/sizeof(bins[0]); ++i) {
      bins[i] = 0;
    }
  }

  if (input == "read") {
    Serial1.println("Looking for a Resistor");
    belt_stepper.setSpeed(BELTSPEED);
    reading = true;
  }

  if (input == "noread") {
    belt_stepper.stop();
    Serial1.println("Not looking for Resistor");
    reading = false;
  }

  if (input == "count") {
    showcount = true;
    Serial1.println("Showing Count");
    Serial1.println(limitSwitch);
  }

  if (input.startsWith("setbins:")) { //setbins:100,4,4000,
    Serial1.println("Updating Bin Values");
    
    // Remove the command prefix "setbins:"
    String data = input.substring(8); 
    
    int currentBin = 0;
    
    while (data.length() > 0 && currentBin < CATCHALLBIN - 1) {
      int commaIndex = data.indexOf(',');
      
      if (commaIndex != -1) {
        String valStr = data.substring(0, commaIndex);
        bins[currentBin] = valStr.toDouble();
        
        data = data.substring(commaIndex + 1);
      } else {
        // Take the last number (no trailing comma)
        bins[currentBin] = data.toDouble();
        data = ""; // Empty string to end loop
      }
      
      // Debug print to verify
      Serial1.print("Bin "); Serial.print(currentBin + 1);
      Serial1.print(" = "); Serial.println(bins[currentBin]);
      
      currentBin++;
      belt_stepper.runSpeed();
    }
  }
}

// a function to inturpret the values coming from the arduino
void listener() {
  while (Serial1.available() > 0) {
    char inChar = (char)Serial1.read();
    if (inputBuffer.length() > 50) { // make sure that the string not too long 
       inputBuffer = ""; 
    }
    if (inChar == '\n') {
      inputBuffer.trim();
      processCommand(inputBuffer); 
      inputBuffer = "";
    } else {
      inputBuffer += inChar;
    }
    belt_stepper.runSpeed();
  }
}