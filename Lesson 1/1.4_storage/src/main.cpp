/*
 * This is an additional lesson sketch used to put the robot into a storage postion.
 * 
 * NOTES:
 * - The MG90S servo motors have a range of 0° to 180° - the range of motion must be declared in the attach() function as microsecond values to get the full range.
 * - Some of the servo positions won't be exactly 0° or 180°, this is due to tollerances in assembly.
 * 
 * SERVO PIN REFERENCE:
 * - PIN D0  = GPIO16
 * - PIN D1  = GPIO5
 * - PIN D2  = GPIO4
 * - PIN D4  = GPIO2
 * - PIN D5  = GPIO14
 * - PIN D6  = GPIO12
 * - PIN D7  = GPIO13
 * - PIN D8  = GPIO15
 */


// INCLUDES
#include <Arduino.h>
#include <Servo.h>

// GLOBAL VARIABLES
Servo servoD5_URP;    // upper right paw
Servo servoD6_URA;    // upper right arm
Servo servoD7_LRA;    // lower right arm
Servo servoD8_LRP;    // lower right paw
Servo servoD0_ULP;    // upper left paw
Servo servoD1_ULA;    // upper left arm
Servo servoD2_LLA;    // lower left arm
Servo servoD4_LLP;    // lower left paw


// Define the storage postions array, we can modify values from the zero positions.
const int storageArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {  0,  178,    2,  175,  177,    3,  170,    2,  1000}
};


// HELPER FUNCTION
void storageMode() {
  servoD5_URP.write(storageArray[0][0]);    // write position in row 0, column 0
  servoD6_URA.write(storageArray[0][1]);    // write position in row 0, column 1
  servoD7_LRA.write(storageArray[0][2]);    // write position in row 0, column 2
  servoD8_LRP.write(storageArray[0][3]);    // write position in row 0, column 3
  servoD0_ULP.write(storageArray[0][4]);    // write position in row 0, column 4
  servoD1_ULA.write(storageArray[0][5]);    // write position in row 0, column 5
  servoD2_LLA.write(storageArray[0][6]);    // write position in row 0, column 6
  servoD4_LLP.write(storageArray[0][7]);    // write position in row 0, column 7
  delay(storageArray[0][8]);                // delay time - row 0, column 8
}


// SETUP
void setup(){
  // Initialize the GPIO & the range of motion of each servo (500 = 0°, 2500 = 180°)
  servoD5_URP.attach(D5, 500, 2500);
  servoD6_URA.attach(D6, 500, 2500);
  servoD7_LRA.attach(D7, 500, 2500);
  servoD8_LRP.attach(D8, 500, 2500);
  servoD0_ULP.attach(D0, 500, 2500);
  servoD1_ULA.attach(D1, 500, 2500);
  servoD2_LLA.attach(D2, 500, 2500);
  servoD4_LLP.attach(D4, 500, 2500);

  // Call the storage mode function
  storageMode();

  // Detach the servos once moved to position
  servoD5_URP.detach();
  servoD6_URA.detach();
  servoD7_LRA.detach();
  servoD8_LRP.detach();
  servoD0_ULP.detach();
  servoD1_ULA.detach();
  servoD2_LLA.detach();
  servoD4_LLP.detach();
}

// MAIN LOOP
void loop() {
}


/*
 * LESSONS LEARNED:
 * - The Servo library is used to control servo motors.
 * - Declaring the servo motors as global variables makes it easier to use elsewhere in the code.
 * 
 * ARRAY:
 * - The storageArray is a 2D array with 1 row and 9 columns
 * - Each column corresponds to a specific servo motor position with the last value being the delay time after movement completes.
 * - The values in the array have been calibrated to account for mechanical tolerances
 * 
 * FUNCTIONS:
 * - storageMode() helper function is used to set the servo motors to specific angles, which gets called in the setup function.
 * - setup() function runs once when the program starts.
 * - loop() is empty since the logic needs to only run once.
 * 
 * CALIBRATION:
 * - To calibrate the servo motors, adjust the angles in the storage array until the arms & paws are close to but not touching the dead-stops.
 * - These offsets can be used in other programs to ensure the robot moves as expected.
 * - Each build will be different so these values will need to be adjusted.
 */