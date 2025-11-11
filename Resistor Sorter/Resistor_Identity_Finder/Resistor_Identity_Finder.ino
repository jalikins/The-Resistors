#include <math.h>
#include "bin_finder.h"


void setup() {
  Serial.begin(9600);
  pinMode(resistorPin, INPUT);
  pinMode(muxA, OUTPUT);
  pinMode(muxB, OUTPUT);
  pinMode(muxC, OUTPUT);
}

void loop() {
  // Wait to measure until user types "measure" into serial monitor
  while (true) {  // stay stopped
    if (Serial.available()) {
      input = Serial.readStringUntil('\n');
      input.trim();
      if (input == "measure") {
        whichBin = binFinder();
      }
    }
  }
  
}