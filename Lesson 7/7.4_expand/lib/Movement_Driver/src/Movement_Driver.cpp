/*
 * Movement_Driver.cpp - Implementation of the MovementDriver library
 * 
 * This file manages servo initialization, movement sequencing, and 
 * state transitions using a non-blocking update loop.
 * 
 * Implementation:
 * - Defines arrays of servo positions for each movement type:
 *   - standby (1 step)
 *   - ready (1 step)
 *   - forward (6 steps)
 *   - backward (6 steps)
 *   - turn left (9 steps)
 *   - turn right (9 steps)
 *   - move left (5 steps)
 *   - move right (5 steps)
 *   - wave hello (12 steps)
 *   - dance routine 1 (9 steps)
 *   - dance routine 2 (8 steps)
 *   - dance routine 3 (16 steps)
 * 
 * - Uses a lookup table (sequences[]) to associate states with arrays
 *   instead of large switch/case blocks.
 * 
 * - Each array row encodes:
 *   { URP, URA, LRA, LRP, ULP, ULA, LLA, LLP, milliseconds }
 * 
 * - startMovementSequence():
 *   - Initializes a movement, sets starting step
 *   - Writes initial servo positions
 * 
 * - update():
 *   - Advances to the next step once the current step’s duration expires
 *   - Marks movement complete when all steps are finished
 *   - Can automatically chain to a queued nextState
 * 
 * - Main sketch remains simple:
 *   - Call update() continuously in loop()
 *   - React to getState() and isBusy() for transitions
 * 
 * SERVO LAYOUT:
 *   -----               -----
 *  | ULP |             | URP |
 *  | D0  |             | D5  |
 *   ----- -----   ----- -----
 *        | ULA | | URA |
 *        | D1  | | D6  |
 *         -----   -----
 *         -----   -----
 *         -----   -----
 *        | LLA | | LRA |
 *        | D2  | | D7  |
 *   ----- -----   ----- -----
 *  | LLP |             | LRP |
 *  | D4  |             | D8  |
 *   -----               -----
 */


// INCLUDES
#include "Movement_Driver.h"

// MOVEMENT ARRAYS
const int MovementDriver::standbyArray[1][9] = {    // standby postions array - 1 step
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,  112,   70,   88,   95,   65,  100,   82,  1000}
};

const int MovementDriver::readyArray[1][9] = {    // ready to move position array - 1 step
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   78,   75,   45,  120,   92,  2000}
};

const int MovementDriver::forwardArray[8][9] = {    // forward movement positions array - 8 steps
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 2 - move URA (+30) & LLA forward (-30)
  {100,  162,   50,   53,   50,   45,   90,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   80,   53,   50,   15,  120,   92,  400}, // step 4 - move URA (-30) & LLA back (+30) | move LRA (+30) & ULA forward (-30)
  {125,  132,   80,   78,   75,   15,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 6 - move LRA (-30) & ULA back (+30) | move URA (+30) & LLA forward (-30)
  {100,  162,   50,   78,   75,   45,   90,   92,  200}, // step 7 - drop URP & LLP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 8 - move URA (-30) & LLA back (+30)
};

const int MovementDriver::backwardArray[8][9] = {    // backward movement positions array - 8 steps
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 2 - move URA (-30) & LLA back (+30)
  {100,  102,   50,   53,   50,   45,  150,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   20,   53,   50,   75,  120,   92,  400}, // step 4 - move URA (+30) & LLA forward (-30) | move LRA (-30) & ULA back (+30)
  {125,  132,   20,   78,   75,   75,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 6 - move LRA (+30) & ULA forward (-30) | move URA (-30) & LLA back (+30)
  {100,  102,   50,   78,   75,   45,  150,   92,  200}, // step 7 - drop URP & LLP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 8 - move URA (+30) & LLA forward (-30)
};

// Turn left movement positions array
const int MovementDriver::turnLeftArray[9][9] = {    // there are 9 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  172,   50,   78,   75,   45,  120,   92,  200}, // step 1 - lift URP (-25) | move URA forward (+40)
  {100,  172,   50,   78,   75,   45,  120,   92,  400}, // step 2 - drop URP (+25)
  {100,  172,   90,   53,   75,   45,  120,   92,  200}, // step 3 - lift LRP (-25) | move LRA forward (+40)
  {100,  172,   90,   78,   75,   45,  120,   92,  400}, // step 4 - drop LRP (+25)
  {100,  172,   90,   78,   75,   45,  160,  117,  200}, // step 5 - lift LLP (+25) | move LLA back (+40)
  {100,  172,   90,   78,   75,   45,  160,   92,  400}, // step 6 - drop LLP (-25)
  {100,  172,   90,   78,   50,   85,  160,   92,  200}, // step 7 - lift ULP (-25) | move ULA back (+40)
  {100,  172,   90,   78,   75,   85,  160,   92,  400}, // step 8 - drop ULP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 9 - rotate arms
};

// Turn right movement positions array
const int MovementDriver::turnRightArray[9][9] = {    // there are 9 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,   92,   50,   78,   75,   45,  120,   92,  200}, // step 1 - lift URP (-25) | move URA back (-40)
  {100,   92,   50,   78,   75,   45,  120,   92,  400}, // step 2 - drop URP (+25)
  {100,   92,   10,   53,   75,   45,  120,   92,  200}, // step 3 - lift LRP (-25) | move LRA back (-40)
  {100,   92,   10,   78,   75,   45,  120,   92,  400}, // step 4 - drop LRP (+25)
  {100,   92,   10,   78,   75,   45,   80,  117,  200}, // step 5 - lift LLP (+25) | move LLA forward (-40)
  {100,   92,   10,   78,   75,   45,   80,   92,  400}, // step 6 - drop LLP (-25)
  {100,   92,   10,   78,   50,    5,   80,   92,  200}, // step 7 - lift ULP (-25) | move ULA forward (-40)
  {100,   92,   10,   78,   75,    5,   80,   92,  400}, // step 8 - drop ULP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 9 - rotate arms
};

// Strafe left movement positions array (10 steps combined into 5)
const int MovementDriver::moveLeftArray[5][9] = {   // there are 5 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   20,   53,   50,   75,  120,   92,  200},  // step 1 - lift ULP & LRP (-25) | move ULA (+30) & LRA (-30) back
  {125,  132,   20,   78,   75,   75,  120,  117,  400},  // step 2 - drop ULP & LRP (+25) | lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  200},  // step 3 - move ULA (-30) & LRA (+30) forward | move URA (+30) & LLA (-30) forward
  {100,  162,   50,   53,   50,   45,   90,   92,  400},  // step 4 - drop URP & LLP (-25) | lift ULP & LRP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400},  // step 5 - move URA (-30) & LLA (+30) back | drop ULP & LRP (+25)
};

// Strafe right movement positions array (10 steps combined into 5)
const int MovementDriver::moveRightArray[5][9] = {   // there are 5 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  102,   50,   78,   75,   45,  150,  117,  200},  // step 1 - lift URP & LLP (+25) | move URA (-30) & LLA (+30) back
  {100,  102,   50,   53,   50,   45,  150,   92,  400},  // step 2 - drop URP & LLP (-25) | lift ULP & LRP (-25)
  {100,  132,   80,   53,   50,   15,  120,   92,  200},  // step 3 - move URA (+30) & LLA (-30) forward | move ULA (-30) & LRA (+30) forward
  {125,  132,   80,   78,   75,   15,  120,  117,  400},  // step 4 - drop ULP & LRP (+25) | lift URP & LLP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400},  // step 5 - move ULA (+30) & LRA (-30) back | drop URP & LLP (-25)
};

// Wave hello movement positions array
const int MovementDriver::waveHelloArray[12][9] = {   // there are 12 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   68,   90,   45,  120,  112,  100},  // step 1 - drop ULP (+15) | lift LRP (-10) & LLP (+20)
  {100,  132,   50,   68,   82,   35,  120,  112,   50},  // step 2 - lift ULP (-8) | move ULA forward (-10)
  {180,  132,   50,   68,   90,   35,  120,  112,  400},  // step 3 - drop ULP (+8) | lift URP (+80)
  {180,  172,   50,   68,   90,   35,  120,  112,  200},  // step 4 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  200},  // step 5 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  200},  // step 6 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  200},  // step 7 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  400},  // step 8 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,   50},  // step 9 - swing URA back (-40)
  {100,  132,   50,   68,   90,   35,  120,  112,   50},  // step 10 - drop URP (-80)
  {100,  132,   50,   68,   82,   45,  120,  112,   50},  // step 11 - lift ULP (-8) | move ULA back (+10)
  {100,  132,   50,   68,   90,   45,  120,  112,  900},  // step 12 - drop ULP (+8)
};

// Dance1 movement positions array
const int MovementDriver::dance1Array[9][9] = {   // there are 9 arrays with 9 columns (adjust the MS value to change the speed)
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 60,   92,   90,   78,   90,   85,   80,   92,  400},  // step 1 - drop URP (-30)
  { 90,   92,   90,  108,   90,   85,   80,   92,  400},  // step 2 - lift URP & drop LRP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   62,  400},  // step 3 - lift LRP & drop LLP (-30)
  { 90,   92,   90,   78,  120,   85,   80,   92,  400},  // step 4 - drop ULP & lift LLP (+30)
  { 60,   92,   90,   78,   90,   85,   80,   92,  400},  // step 5 - lift ULP & drop URP (-30)
  { 90,   92,   90,  108,   90,   85,   80,   92,  400},  // step 6 - lift URP & drop LRP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   62,  400},  // step 7 - lift LRP & drop LLP (-30)
  { 90,   92,   90,   78,  120,   85,   80,   92,  400},  // step 8 - drop ULP & lift LLP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   92,  400},  // step 9 - lift ULP (+30)
};

// Dance2 movement positions array
const int MovementDriver::dance2Array[8][9] = {   // there are 8 arrays with 9 columns (adjust the MS value to change the speed)
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

// Dance3 movement positions array
const int MovementDriver::dance3Array[16][9] = {   // there are 16 arrays with 9 columns (adjust the MS value to change the speed)
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
  { 80,   92,   90,   88,   95,   85,   80,   82,  500},  // step 16 - move LLA (-90) forward | drop LLP (+8)
};


// Sequeneces lookup table
const MovementArray MovementDriver::sequences[13] = {
  { standbyArray,   sizeof(standbyArray)    / sizeof(standbyArray[0])   },    // STANDBY
  { readyArray,     sizeof(readyArray)      / sizeof(readyArray[0])     },    // READY
  { forwardArray,   sizeof(forwardArray)    / sizeof(forwardArray[0])   },    // FORWARD
  { backwardArray,  sizeof(backwardArray)   / sizeof(backwardArray[0])  },    // BACKWARD
  { turnLeftArray,  sizeof(turnLeftArray)   / sizeof(turnLeftArray[0])  },    // TURN_LEFT
  { turnRightArray, sizeof(turnRightArray)  / sizeof(turnRightArray[0]) },    // TURN_RIGHT
  { moveLeftArray,  sizeof(moveLeftArray)   / sizeof(moveLeftArray[0])  },    // MOVE_LEFT
  { moveRightArray, sizeof(moveRightArray)  / sizeof(moveRightArray[0]) },    // MOVE_RIGHT
  { waveHelloArray, sizeof(waveHelloArray)  / sizeof(waveHelloArray[0]) },    // WAVE_HELLO
  { dance1Array,    sizeof(dance1Array)     / sizeof(dance1Array[0])    },    // DANCE1
  { dance2Array,    sizeof(dance2Array)     / sizeof(dance2Array[0])    },    // DANCE2
  { dance3Array,    sizeof(dance3Array)     / sizeof(dance3Array[0])    },    // DANCE2
  { nullptr, 0 }                                                              // IDLE
};

// CLASS IMPLEMENTATION
MovementDriver::MovementDriver() {
  lastState = IDLE;
  currentState = IDLE;
  nextState = IDLE;
  currentStep = 0;
  stepStartTime = 0;
  isMoving = false;
}

// Initialize all servos with their pulse width ranges 
void MovementDriver::begin() {
  servoD5_URP.attach(D5, 500, 2500);
  servoD6_URA.attach(D6, 500, 2500);
  servoD7_LRA.attach(D7, 500, 2500);
  servoD8_LRP.attach(D8, 500, 2500);
  servoD0_ULP.attach(D0, 500, 2500);
  servoD1_ULA.attach(D1, 500, 2500);
  servoD2_LLA.attach(D2, 500, 2500);
  servoD4_LLP.attach(D4, 500, 2500);
}

// Non-blocking update method - must be called in main loop
void MovementDriver::update() {
  // If in IDLE state, check if the duration has passed before moving to the next state.
  if (currentState == IDLE) {
    if (millis() - stepStartTime >= idleDuration) {
      if (nextState != IDLE) {
        startMovementSequence(nextState);
        nextState = IDLE;
      }
    }
    return;
  }
  
  if (!isMoving) return;

  unsigned long currentTime = millis();
  const MovementArray &seq = sequences[currentState];
  unsigned long stepDuration = seq.steps[currentStep][8];

  if (currentTime - stepStartTime >= stepDuration) {
    currentStep++;

    if (currentStep >= seq.size) {
      // Sequence finished
      isMoving = false;
      
      // If a nextState is queued, automatically start it.
      if (nextState != IDLE) {
        startMovementSequence(nextState);
        nextState = IDLE;
      }
      return;
    }

    // Move to next step
    setServoPositions(seq.steps[currentStep]);
    stepStartTime = currentTime;
  }
}

// Write servo positions
void MovementDriver::setServoPositions(const int positions[]) {
  servoD5_URP.write(positions[0]);
  servoD6_URA.write(positions[1]);
  servoD7_LRA.write(positions[2]);
  servoD8_LRP.write(positions[3]);
  servoD0_ULP.write(positions[4]);
  servoD1_ULA.write(positions[5]);
  servoD2_LLA.write(positions[6]);
  servoD4_LLP.write(positions[7]);
}

// Start a new movement sequence
void MovementDriver::startMovementSequence(MovementState newState) {
  // Block if robot is already moving
  if (isMoving) {
    nextState = newState;
    return;
  }

  // Save the previous state
  lastState = currentState;

  // Set the new state and start time
  currentState = newState;
  stepStartTime = millis();
  isMoving = true;

  // Start at the 1st step
  currentStep = 0;

  const MovementArray &seq = sequences[currentState];
  setServoPositions(seq.steps[currentStep]);
}

// Public movement commands
void MovementDriver::standby() {
  startMovementSequence(STANDBY);
}

void MovementDriver::ready() {
  startMovementSequence(READY);
}

void MovementDriver::forward() {
  startMovementSequence(FORWARD);
}

void MovementDriver::backward() {
  startMovementSequence(BACKWARD);
}

void MovementDriver::turnLeft() {
  startMovementSequence(TURN_LEFT);
}

void MovementDriver::turnRight() {
  startMovementSequence(TURN_RIGHT);
}

void MovementDriver::moveLeft() {
  startMovementSequence(MOVE_LEFT);
}

void MovementDriver::moveRight() {
  startMovementSequence(MOVE_RIGHT);
}

void MovementDriver::waveHello() {
  startMovementSequence(WAVE_HELLO);
}

void MovementDriver::dance1() {
  startMovementSequence(DANCE1);
}

void MovementDriver::dance2() {
  startMovementSequence(DANCE2);
}

void MovementDriver::dance3() {
  startMovementSequence(DANCE3);
}

void MovementDriver::idle(unsigned long duration, MovementState queuedState) {
  isMoving = false;
  currentState = IDLE;
  stepStartTime = millis();
  idleDuration = duration;
  nextState = queuedState;
}

bool MovementDriver::isBusy() {
  return isMoving;
}
