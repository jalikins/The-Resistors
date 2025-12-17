#include <SoftwareSerial.h>
#include <Arduino.h>

// specifying pcb serial pins
SoftwareSerial pcbSerial(2, 3); 

// setting baud
const long BAUD_RATE = 9600;

void setup() {
  // start comms with Raspi
  Serial.begin(BAUD_RATE);
  while (!Serial) {
    // wait for serial to connect
    ; 
  }

  // start comms with PCB
  pcbSerial.begin(BAUD_RATE);
  
  // blink built in LED to indicate comms successful
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  // direction 1: pi to pcb
  if (Serial.available()) {
    char c = Serial.read();
    pcbSerial.write(c);
  }

  // direction 2: pcb to pi
  if (pcbSerial.available()) {
    char c = pcbSerial.read();
    Serial.write(c);
  }
}