/*
 * NOTES:
 * - This positions.h file is used to store the positional arrays for the robot & will need to get imported into the main file by adding:
 *  #include "positions.h"
 * - We determined the useable range of the servo motors in the zeroing project,
 *   so we'll use those values as our reference positions.
 * - Motor pins are connected to the corresponding pins on the servo extension board:
 * 
 * SERVO PIN REFERENCE:
 * - PIN D0 = GPIO16
 * - PIN D1 = GPIO5
 * - PIN D2 = GPIO4
 * - PIN D4 = GPIO2
 * - PIN D5 = GPIO14
 * - PIN D6 = GPIO12
 * - PIN D7 = GPIO13
 * - PIN D8 = GPIO15
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

bool firstForwardIteration = false;     // first iteration flag for forward function
bool firstBackwardIteration = false;    // first iteration flag for backward function

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

// Define the forward movement positions array
const int forwardArray[6][9] = {   // there are 6 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 2 - move URA (+30) & LLA forward (-30)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  162,   50,   53,   50,   45,   90,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   80,   53,   50,   15,  120,   92,  400}, // step 4 - move URA (-30) & LLA back (+30) | move LRA (+30) & ULA forward (-30)
  {125,  132,   80,   78,   75,   15,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 6 - move LRA (-30) & ULA back (+30) | move URA (+30) & LLA forward (-30)
};

// Define the backward movement positions array
const int backwardArray[6][9] = {   // there are 6 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 2 - move URA (-30) & LLA back (+30)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  102,   50,   53,   50,   45,  150,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   20,   53,   50,   75,  120,   92,  400}, // step 4 - move URA (+30) & LLA forward (-30) | move LRA (-30) & ULA back (+30)
  {125,  132,   20,   78,   75,   75,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 6 - move LRA (+30) & ULA forward (-30) | move URA (-30) & LLA back (+30)
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

void forward() {    // Forward function
  if (!firstForwardIteration) {    // if false
    // Run through all steps for the first iteration (steps 1–6)
    for (int i = 0; i < 6; i++) {   // start at 0 (1st step), go up to 5 (less than 6 - last step)
      writeServoPositions(forwardArray + i, 1);   // pass the specific row (step)
    }
    firstForwardIteration = true;   // set the first iteration flag
  }
  else {    // if ture
    // Run only steps 3–6 for subsequent iterations
    for (int i = 2; i < 6; i++) {   // start at 2 (3rd step), go up to 5 (less than 6 - last step)
      writeServoPositions(forwardArray + i, 1);   // pass the specific row (step)
    }
  }
}

void backward() {   // Backward function
  if (!firstBackwardIteration) {    // if false
    // Run through all steps for the first iteration (steps 1–6)
    for (int i = 0; i < 6; i++) {   // start at 0 (1st step), go up to 5 (less than 6 - last step)
      writeServoPositions(backwardArray + i, 1);  // pass the specific row (step)
    }
    firstBackwardIteration = true;   // set the first iteration flag
  }
  else {    // if true
    // Run only steps 3–6 for subsequent iterations
    for (int i = 2; i < 6; i++) {   // start at 2 (3rd step), go up to 5 (less than 6 - last step)
      writeServoPositions(backwardArray + i, 1);  // pass the specific row (step)
    }
  }
}
