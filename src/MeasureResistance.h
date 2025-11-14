#include <list>

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


const double perc_uncertainty = 0.2;
const double hiThreshold = 4.0;
// Needs to be assigned
const double moduleWidth = 0;
// Needs to be assigned (The distance from measurement to the first bin)
const double firstBin = 0;
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

// Needs to be assigned
unsigned float beltPos = 0;
double voltageMeas = 0;
double varResistance = 0;
double constResistance = 0;
int bins [catchAllBin];
int i=0;
int binThreshold = 0;
int iterationCount = 1;
String input = "";
bool catchAll = 0;

int whichBin = catchAllBin;
std::list<int> binOrder[catchAllBin];