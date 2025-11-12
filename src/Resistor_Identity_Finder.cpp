#include <math.h>
#include <Servo.h> // Include the Servo library
#include <string.h>
#include "Arduino.h"
#include "bin_finder.h"


void setup() {
  Serial.begin(9600);
  pinMode(resistorPin, INPUT);
  pinMode(muxA, OUTPUT);
  pinMode(muxB, OUTPUT);
  pinMode(muxC, OUTPUT);
}

void loop() {
  digitalWrite(muxA, LOW);
  digitalWrite(muxB, LOW);
  digitalWrite(muxC, LOW);
  // Wait to measure until user types "measure" into serial monitor
  if (Serial.available()) {
    input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "measure") {
      whichBin = binFinder();
    }
  }
  
}