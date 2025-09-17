#include <Servo.h>

Servo myservo;  // create servo object to control a servo
int pos;
void setup() {
  myservo.attach(D4);  // attaches the servo on GIO2 to the servo objec t
  for (pos = 10; pos <= 170; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 170; pos >= 10; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  delay(1000); 
}

void loop() {
  myservo.write(90);
  }


