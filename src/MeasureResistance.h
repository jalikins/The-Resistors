// The known resistor values to cycle through in kOhms
double referenceValue [] = {
  20,
  1,
  10,
  49.9,
  100,
  499,
  1000
};

const int hiThreshold = 3;
const int lowThreshold = 1;
double voltageMeas = 0;
double varResistance = 0;
int bins [] = {0, 0, 0, 0, 0, 0};
int i=0;
int binThreshold = 0;
int iterationCount = 1;
double constResistance = 0;
String input = "";
bool catchAll = 0;

const int catchAllBin = 3;
const int resistorPin = A0;
const int muxA = 2;
const int muxB = 3;
const int muxC = 4;
const int voltageIn = 5;
const int maxBins = sizeof(bins);
const double perc_uncertainty = 0.2;

int whichBin = maxBins;