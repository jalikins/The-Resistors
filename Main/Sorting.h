
// ----- MUX AND RESISTOR -------- // 

// Net measuring voltage
const int VOLTAGEIN = 5;


// Resistor measurement
const int RESISTORPIN = 30;

// MUX
const int MUXA = 37;
const int MUXB = 38;
const int MUXC = 39;
const int MUXD = 40;

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

// ---- STEPPER MOTOR ---- // 

// Stepper motor
const int DIRPIN = PIN_PA3;
const int STEPPIN = PIN_PA2;

// Stepper speed
const int BELTSPEED = 200;

const uint8_t MOTORINTERFACETYPE = 1;