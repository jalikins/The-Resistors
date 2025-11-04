#include <math.h>

double voltageMeas = 0;
double varResistance = 0;
int bins [] = {0, 0, 0, 0, 0, 0};
int i=0;
int binThreshold = 0;
String input = "";

const int resistorPin = A1;
const int constResistance = 1000;
const int voltageIn = 5;
const int maxBins = sizeof(bins);

int whichBin = maxBins;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(resistorPin, INPUT);
}

void loop() {
  // Wait to measure until user types "measure" into serial moniter
  while (true) {  // stay stopped
    if (Serial.available()) {
      input = Serial.readStringUntil('\n');
      input.trim();
      if (input == "measure") {
        break;
      }
    }
  }
  // Read voltage across unknown resistor
  voltageMeas = analogRead(resistorPin);
  voltageMeas = voltageIn*voltageMeas/1024;
  // Calculate the resistance of unknown resistor
  varResistance = voltageMeas*constResistance/(voltageIn - voltageMeas);
  Serial.print("voltageMeas=");
  Serial.println(voltageMeas);
  varResistance = round(varResistance);
  binThreshold = 10 * voltageMeas;

  for (i=0; i<maxBins; i+=1) {
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
