


// ------ LIMIT SWITCH ----- /////// YAYYA
const int BUTTONPIN = PIN_PC7;

unsigned long clickCount = 0;       // Total number of clicks
unsigned long prevClickCounter;
bool lastSwitchState = HIGH;        // Previous state (starts released/HIGH)
unsigned long lastDebounceTime = 0; // Timer for debounce
const unsigned long debounceDelay = 50; // 50ms noise filter
bool limitSwitch; // is decided by the button

bool showcount = false; // 

void clickcounter(){
  if (lastSwitchState == HIGH && limitSwitch == LOW) {
        // Debouncer
      if ((millis() - lastDebounceTime) > debounceDelay) {
        
        clickCount++; // Increment counter
        if (showcount){
          Serial1.print("Number of Clicks: ");
          Serial1.println(clickCount);
        }

        lastDebounceTime = millis(); // Reset timer
      }
    }
    lastSwitchState = limitSwitch;
}
// Reading Values
bool reading = false;

