/*
 * This function is used to make the robot wave hello.
 * 
 * NOTES:
 * - We determined the useable range of the servo motors in the zeroing project,
 *   so we'll use those values as our reference positions.
 * - Motor pins are connected to the corresponding pins on the servo extension board:
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
 * 
 *   -----                -----
 *  | ULP |              | URP |
 *  | D0  |              | D5  |
 *   ----- -----    ----- -----
 *        | ULA |  | URA |
 *        | D1  |  | D6  |
 *         -----    -----
 *         -----    -----
 *         -----    -----
 *        | LLA |  | LRA |
 *        | D2  |  | D7  |
 *   ----- -----    ----- -----
 *  | LLP |              | LRP |
 *  | D4  |              | D8  |
 *   -----                -----
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

// Define the standby postions array, we can modify values from the zero positions
const int standbyArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,  112,   70,   88,   95,   65,  100,   82,  500}
};

// Define a ready to move position array (lower the paws to a more stable position)
const int readyArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   78,   75,   45,  120,   92,  2000}
};

// Create an array of hello actions
const int waveHelloArray[12][9] = {   // there are 12 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   68,   90,   45,  120,  112,  100},  // step 1 - drop ULP (+15) | lift LRP (-10) & LLP (+20)
  {100,  132,   50,   68,   82,   35,  120,  112,   50},  // step 2 - lift ULP (-8) | move ULA forward (-10)
  {180,  132,   50,   68,   90,   35,  120,  112,  400},  // step 3 - drop ULP (+8) | lift URP (+80)
  {180,  172,   50,   68,   90,   35,  120,  112,  100},  // step 4 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  100},  // step 5 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  100},  // step 6 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  100},  // step 7 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  200},  // step 8 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,   50},  // step 9 - swing URA back (-40)
  {100,  132,   50,   68,   90,   35,  120,  112,   50},  // step 10 - drop URP (-80)
  {100,  132,   50,   68,   82,   45,  120,  112,   50},  // step 11 - lift ULP (-8) | move ULA back (+10)
  {100,  132,   50,   68,   90,   45,  120,  112,  900},  // step 12 - drop ULP (+8)
};


// HELPER FUNCTIONS
void writeServoPositions(const int positions[][9], int rowCount) {    // Write servo positions function
  for (int i = 0; i < rowCount; i++) {
    servoD5_URP.write(positions[i][0]);    // write position in row i, column 0
    servoD6_URA.write(positions[i][1]);    // write position in row i, column 1
    servoD7_LRA.write(positions[i][2]);    // write position in row i, column 2
    servoD8_LRP.write(positions[i][3]);    // write position in row i, column 3
    servoD0_ULP.write(positions[i][4]);    // write position in row i, column 4
    servoD1_ULA.write(positions[i][5]);    // write position in row i, column 5
    servoD2_LLA.write(positions[i][6]);    // write position in row i, column 6
    servoD4_LLP.write(positions[i][7]);    // write position in row i, column 7
    delay(positions[i][8]);                // delay time - row i, column 8
  }
}

void waveHello() {    // Wave hello function
  writeServoPositions(waveHelloArray, 12);    // move through all steps
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
  
  // Move into standy mode & then into ready mode
  writeServoPositions(standbyArray, 1);   // standby array has 500ms delay
  delay(500);                             // wait an extra 500ms
  writeServoPositions(readyArray, 1);     // ready array has 2sec delay
}

// MAIN LOOP
void loop() {
  waveHello();    // call the wave hello function
  delay(1000);
}


/*
 * LESSONS LEARNED:
 * - The Servo library is used to control servo motors.
 * - Declaring the servo motors as global variables makes it easier to use elsewhere in the code.
 * 
 * ARRAY:
 * - standbyArray and readyArray have 1 row and 9 columns
 * - waveHelloArray has 12 rows and 9 columns
 * - Each column corresponds to a specific servo motor position with the last value being the delay time after movement completes.
 * - The values in the array have been calibrated to account for mechanical tolerances
 * 
 * FUNCTIONS:
 * - waveHello() function uses the position arrays to move the limbs
 * - setup() function runs once when the program starts and attaches the servos, writes the positions in the standbyArray, then writes the positions in the readyArray.
 * - loop() function calls the waveHello() function which runs repeatedly as long as the board is powered on.
 */