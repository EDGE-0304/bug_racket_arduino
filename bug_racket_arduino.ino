/* Version 1.0 
  Testing the connection of backend and bluetooth binding with front end
   The bug killing part is simulated by a simple button and LED on board,
   Once the switch is on, the backend will receive the status of on, then
   each time the button is pressed, a new bug is killed, and it shows on
   the front end. Also, the front end should bind with the arduino through 
   bluetooth connection. For the battery level, we can use a resistor to
   design the voltage divider then access the voltage level, thus get the
   battery level. When the battery is off, the count for locally stops.
For next step
   make sure change the bug killing part
   

*/

const int buttonPin = 2; // the number of the pushbutton pin
const int switchPin = 3; // the number of the switch pin
int buttonState = 0;     // variable for reading the pushbutton status
int switchState = 0;     // variable for reading the switch status
int counter = 0;         // counter for the button presses

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(switchPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  switchState = digitalRead(switchPin);
  
  if (switchState == HIGH) {
    buttonState = digitalRead(buttonPin);
    
    if (buttonState == HIGH) {
      counter++;
      Serial.println(counter);
      delay(100); // debounce delay
    }
  } else {
    counter = 0; // reset counter when switch is off
  }
}
