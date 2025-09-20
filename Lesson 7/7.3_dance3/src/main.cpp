/*
 * This function is used to make the robot perform dance routine 3.
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

// Dance3 movement positions array
const int dance3Array[16][9] = {   // there are 16 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,   92,   90,   80,   95,   85,   80,   82,   50},  // step 1 - lift LRP (-8)
  { 80,   92,    2,   88,   95,   85,   80,   82,  100},  // step 2 - move LRA (-88) back | drop LRP (+8)
  { 80,   92,    2,   88,   95,   85,   80,   76,   50},  // step 3 - lift LLP (-8)
  { 80,   92,    2,   88,   95,   85,  170,   82,  100},  // step 4 - move LLA (+90) back | drop LLP (+8)
  {100,   92,    2,   63,   75,   85,  170,   82,  400},  // step 5 - lift URP (+20), ULP (-20) & LRP (-25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 6 - drop URP (-20), ULP (+20) & LRP (+25)
  {100,   92,    2,   88,   75,   85,  170,  107,  400},  // step 7 - lift URP (+20), ULP (-20) & LLP (+25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 8 - drop URP (-20), ULP (+20) & LLP (-25)
  {100,   92,    2,   63,   75,   85,  170,   82,  400},  // step 9 - lift URP (+20), ULP (-20) & LRP (-25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 10 - drop URP (-20), ULP (+20) & LRP (+25)
  {100,   92,    2,   88,   75,   85,  170,  107,  400},  // step 11 - lift URP (+20), ULP (-20) & LLP (+25)
  { 80,   92,    2,   88,   95,   85,  170,   82,   50},  // step 12 - drop URP (-20), ULP (+20) & LLP (-25)
  { 80,   92,    2,   80,   95,   85,  170,   82,   50},  // step 13 - lift LRP (-8)
  { 80,   92,   90,   88,   95,   85,  170,   82,  100},  // step 14 - move LRA (+88) forward | drop LRP (+8)
  { 80,   92,   90,   80,   95,   85,  170,   76,   50},  // step 15 - lift LLP (-8)
  { 80,   92,   90,   88,   95,   85,   80,   82,  100},  // step 16 - move LLA (-90) forward | drop LLP (+8)
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

void dance3() {    // Dance 2 function
  writeServoPositions(dance3Array, 16);    // move through all steps
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
  dance3();    // call dance 3 routine function
  delay(1000);
}


/*
 * LESSONS LEARNED:
 * - The Servo library is used to control servo motors.
 * - Declaring the servo motors as global variables makes it easier to use elsewhere in the code.
 * 
 * ARRAY:
 * - standbyArray and readyArray have 1 row and 9 columns
 * - dance3Array has 16 rows and 9 columns
 * - Each column corresponds to a specific servo motor position with the last value being the delay time after movement completes.
 * - The values in the array have been calibrated to account for mechanical tolerances
 * 
 * FUNCTIONS:
 * - dance3() function uses the position arrays to move the limbs
 * - setup() function runs once when the program starts and attaches the servos, writes the positions in the standbyArray, then writes the positions in the readyArray.
 * - loop() function calls the dance3() function which runs repeatedly as long as the board is powered on.
 */
