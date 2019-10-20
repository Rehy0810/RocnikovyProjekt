#include <Stepper.h>
// Define number of steps per rotation
const int stepsPerRevolution = 2048;
// Pin 8 to IN1 
// Pin 9 to IN2 
// Pin 10 to IN3 
// Pin 11 to IN4 
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);
void setup() {
  myStepper.setSpeed(5);
  
  Serial.begin(9600);
}
void loop() {
  Serial.println("clockwise");
  myStepper.step(stepsPerRevolution);
  delay(500);
  
  Serial.println("counterclockwise");
  myStepper.step(-stepsPerRevolution);
  delay(500);
}
