/*
 * This function demonstrates the basic usage of the Servo library to test a servo motor.
 * The servo motor moves from 0 degrees to 180 degrees and then back to 0 degrees.
 * It demonstrates the use of the attach, write, and delay functions to control the servo motor.
 *
 * NOTES:
 * - The MG90S servo motors have a range of 0° to 180° - the range of motion must be declared in the attach() function as microsecond values to get the full range.
 * - Test 1 servo motor at a time on pin D4.
 * 
 * !!IMPORTANT!!
 * - Servo motors should NOT be installed to a load while testing as this may damage the motor.
 * - Turn OFF the board before connecting or disconnecting a servo motor.
 */


// INCLUDES
#include <Arduino.h>    // for PlatformIO (not needed for Arduino IDE)
#include <Servo.h>      // include the Servo library to control the servo motors


// GLOBAL VARIABLES
Servo robotServo;   // create a servo object
int servoPos;       // integer variable to store the servo position


// SETUP
void setup() {
  robotServo.attach(D4, 500, 2500);                       // attach the servo on pin D4 to the servo object and set the range of motion (500 = 0°, 2500 = 180°)
  for (servoPos = 0; servoPos <= 180; servoPos += 1) {    // move from 0 degrees to 180 degrees in 1 degree increments
    robotServo.write(servoPos);                           // set the servo to the value of servoPo
    delay(15);                                            // wait 15ms for the servo to reach the position
  }

  for (servoPos = 180; servoPos >= 0; servoPos -= 1) {    // move from 180 degrees to 0 degrees in 1 degree increments
    robotServo.write(servoPos);                           // set the servo to the value of servoPos
    delay(15);                                            // wait 15ms for the servo to reach the position
  }

  delay(1000);  // wait for 1 second
}


// MAIN LOOP
void loop() {
  robotServo.write(90);   // set the servo to 90 degrees (change to set a different final position).
}


/*
 * LESSONS LEARNED:
 * - The Servo library is used to control servo motors.
 * - Using declarations makes it easier to use the Servo library and makes the code more readable.
 *
 * - setup() function runs once when the program starts.
 * - attach() is used to connect the servo motor to a specific pin and we must specify the range of motion in microseconds to get the full range out of the servo.
 * - The for loop is used to move the servo motor from one position to another using degrees.
 * - write() is used to set the servo motor to the positions defined in the servoPos variable.
 * - delay() is used to pause the program for a specified number of milliseconds.
 * - The servo motor moves from 0 degrees to 180 degrees and back to 0 degrees and then gets delayed for 1 second before entering the loop() function.
 *
 * - loop() function runs repeatedly as long as the board is powered on.
 * - In this case, the servo motor is set to 90 degrees in the loop() function constantly, keeping it stationary.
 *
 *
 * How the for loops work:
 * - The 1st for loop sets the servoPos variable to 0 degrees
 *   (the servo will move from whatever position it was in to 0 degrees).
 * 
 * - The loop continues as long as the servoPos value is LESS THAN OR EQUAL TO 180 degrees with servoPos being incremented by 1 degree (+=1) every time.
 * 
 * - Inside the loop, the servo motor is set to the value of servoPos, and a delay of 15ms is added to allow the servo to reach the position.
 *   Since the servoPos value is incremented by 1 degree each time, the servo moves from 0 degrees to 180 degrees in small 1 degree increments every 15ms.
 * 
 * - The same process is repeated for the second for loop, but in reverse (-=1) - moving the servo from 180 degrees to 0 degrees in small 1 degree increments every 15ms.
 */
