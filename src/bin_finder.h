#include <math.h>
#include <MeasureResistance.h>

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
  // Reset catchAll boolean
  catchAll = false;
  return whichBin;
}