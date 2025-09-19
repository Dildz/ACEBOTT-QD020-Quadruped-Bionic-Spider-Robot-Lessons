/*
NOTES:
- This positions.h file is used to store the positional arrays for the robot & will need to get imported into the main file by adding:
 #include "positions.h"
- We determined the useable range of the servo motors in the zeroing project,
  so we'll use those values as our reference positions.
- Motor pins are connected to the corresponding pins on the servo extension board:

//  -----                -----
// | ULP |              | URP |
// | P16 |              | P14 |
//  ----- -----    ----- -----
//       | ULA |  | URA |
//       | P05 |  | P12 |
//        -----    -----
//        -----    -----
//        -----    -----
//       | LLA |  | LRA |
//       | P04 |  | P13 |
//  ----- -----    ----- -----
// | LLP |              | LRP |
// | P02 |              | P15 |
//  -----                -----
*/


// Imports
#include <Servo.h>      // include the servo library

// Servo variables for each servo motor
Servo servo14_URP;   // upper right paw
Servo servo12_URA;   // upper right arm
Servo servo13_LRA;   // lower right arm
Servo servo15_LRP;   // lower right paw
Servo servo16_ULP;   // upper left paw
Servo servo5_ULA;    // upper left arm
Servo servo4_LLA;    // lower left arm
Servo servo2_LLP;    // lower left paw


// Standby position array
const int standbyArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,  112,   70,   88,   95,   65,  100,   82,  1000}
};


// Ready position array (lower the paws to a more stable position)
const int danceReadyArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,   92,   90,   88,   95,   85,   80,   82,  1000}
};


// Dance2 movement positions array
int dance2Array[8][9] = {   // there are 8 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 1 - lift URP (+20) & LRP (-20)

  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 2 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 3 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)

  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 4 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 5 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)

  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 6 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 7 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)

  { 80,   92,   90,   88,   95,   85,   80,   82,  400},  // step 8 - drop URP & LRP (-20)
};


// Write servo positions function
void writeServoPositions(const int positions[][9], int rowCount) {
  for (int i = 0; i < rowCount; i++) {
    servo14_URP.write(positions[i][0]);    // write position in row i, column 0
    servo12_URA.write(positions[i][1]);    // write position in row i, column 1
    servo13_LRA.write(positions[i][2]);    // write position in row i, column 2
    servo15_LRP.write(positions[i][3]);    // write position in row i, column 3
    servo16_ULP.write(positions[i][4]);    // write position in row i, column 4
    servo5_ULA.write(positions[i][5]);     // write position in row i, column 5
    servo4_LLA.write(positions[i][6]);     // write position in row i, column 6
    servo2_LLP.write(positions[i][7]);     // write position in row i, column 7
    delay(positions[i][8]);                // delay time - row i, column 8
  }
}


// Standby function
void standby() {
  writeServoPositions(standbyArray, 1);     // standby positions, row count 1
}


// Ready function
void danceReady() {
  writeServoPositions(danceReadyArray, 1);  // ready positions, row count 1
}


// Dance2 function
void dance2() {
 writeServoPositions(dance2Array, 8);       // dance2 positions, row count 8
}
