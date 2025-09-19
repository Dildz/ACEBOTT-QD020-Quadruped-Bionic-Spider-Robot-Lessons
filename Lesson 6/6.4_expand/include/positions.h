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
const int readyArray[1][9] = {    // 1 arrays with 9 columns (0 to 8)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   78,   75,   45,  120,   92,  1000}
};

// Strafe left movement positions array
const int moveLeftArray[9][9] = {   // there are 9 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   53,   50,   45,  120,   92,   50},  // step 1 - lift ULP & LRP (-25)
  {100,  132,   20,   53,   50,   75,  120,   92,  100},  // step 2 - move ULA (+30) & LRA (-30) back
  {100,  132,   20,   78,   75,   75,  120,   92,  400},  // step 3 - drop ULP & LRP (+25)

  {125,  132,   20,   78,   75,   75,  120,  117,  100},  // step 4 - lift URP & LLP (+25)
  {125,  132,   50,   78,   75,   45,  120,  117,   50},  // step 5 - move ULA (-30) & LRA (+30) forward
  {125,  162,   50,   78,   75,   45,   90,  117,   50},  // step 6 - move URA (+30) & LLA (-30) forward
  {100,  162,   50,   78,   75,   45,   90,   92,  400},  // step 7 - drop URP & LLP (-25)

  {100,  162,   50,   53,   50,   45,   90,   92,  100},  // step 8 - lift ULP & LRP (-25)
  {100,  132,   50,   53,   50,   45,  120,   92,  100},  // step 9 - move URA (-30) & LLA (+30) back
};

// Strafe right movement positions array
 const int moveRightArray[9][9] = {   // there are 9 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,   50},  // step 1 - lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  100},  // step 2 - move URA (-30) & LLA (+30) back
  {100,  102,   50,   78,   75,   45,  150,   92,  400},  // step 3 - drop URP & LLP (-25)

  {100,  102,   50,   53,   50,   45,  150,   92,  100},  // step 4 - lift ULP & LRP (-25)
  {100,  132,   50,   53,   50,   45,  120,   92,   50},  // step 5 - move URA (+30) & LLA (-30) forward
  {100,  132,   80,   53,   50,   15,  120,   92,   50},  // step 6 - move ULA (-30) & LRA (+30) forward
  {100,  132,   80,   78,   75,   15,  120,   92,  400},  // step 7 - drop ULP & LRP (+25)

  {125,  132,   80,   78,   75,   15,  120,  117,  100},  // step 8 - lift URP & LLP (+25)
  {125,  132,   50,   78,   75,   45,  120,  117,  100},  // step 9 - move ULA (+30) & LRA (-30) back
};

// Wave hello movement positions array
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
  writeServoPositions(standbyArray, 1);  // standby positions, row count 1
}


// Ready function
void ready() {
  writeServoPositions(readyArray, 1);  // ready positions, row count 1
}


// Move (strafe) left function
void moveLeft() {
 writeServoPositions(moveLeftArray, 9);  // move left positions, row count 9
}


// Move (strafe) right function
void moveRight() {
 writeServoPositions(moveRightArray, 9);  // move right positions, row count 9
}


// Wave Hello function
void waveHello() {
 writeServoPositions(waveHelloArray, 12);  // wave hello positions, row count 12
}
