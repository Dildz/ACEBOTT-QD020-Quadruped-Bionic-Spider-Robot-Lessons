/*
 * Movement_Driver.h - Custom library for controlling quadruped robot movements
 * 
 * This library provides an interface for controlling the robot.
 * It defines movement patterns as arrays of positions,
 * and manages them through a table-driven simple state machine.
 * 
 * NOTES:
 * - We determined the useable range of the servo motors in the zeroing project,
 *   so we'll use those values as our reference positions.
 * - Motor pins are connected to the corresponding pins on the servo extension board:
 * 
 * SERVO PIN REFERENCE:
 * - PIN D0 = GPIO16 → Upper Left Paw (ULP)
 * - PIN D1 = GPIO5  → Upper Left Arm (ULA)
 * - PIN D2 = GPIO4  → Lower Left Arm (LLA)
 * - PIN D4 = GPIO2  → Lower Left Paw (LLP)
 * - PIN D5 = GPIO14 → Upper Right Paw (URP)
 * - PIN D6 = GPIO12 → Upper Right Arm (URA)
 * - PIN D7 = GPIO13 → Lower Right Arm (LRA)
 * - PIN D8 = GPIO15 → Lower Right Paw (LRP)
 */


#ifndef MOVEMENT_DRIVER_H
#define MOVEMENT_DRIVER_H

// INCLUDES
#include <Servo.h>

// STATE ENUMS
enum MovementState {
  STANDBY,
  READY,
  FORWARD,
  BACKWARD,
  TURN_LEFT,
  TURN_RIGHT,
  MOVE_LEFT,
  MOVE_RIGHT,
  WAVE_HELLO,
  DANCE1,
  DANCE2,
  DANCE3,
  LIE_DOWN,
  FIGHTING,
  PUSH_UPS,
  SLEEP,
  IDLE
};

// STRUCTS
struct MovementArray {
  const int (*steps)[9];   // pointer to array of steps
  int size;                // number of steps
};

// CLASSES
class MovementDriver {
  private:
    // Servo objects
    Servo servoD5_URP;    // upper right paw
    Servo servoD6_URA;    // upper right arm
    Servo servoD7_LRA;    // lower right arm
    Servo servoD8_LRP;    // lower right paw
    Servo servoD0_ULP;    // upper left paw
    Servo servoD1_ULA;    // upper left arm
    Servo servoD2_LLA;    // lower left arm
    Servo servoD4_LLP;    // lower left paw

    // Position arrays (movement sequences)
    static const int standbyArray[1][9];
    static const int readyArray[1][9];
    static const int forwardArray[8][9];
    static const int backwardArray[8][9];
    static const int turnLeftArray[9][9];
    static const int turnRightArray[9][9];
    static const int moveLeftArray[5][9];
    static const int moveRightArray[5][9];
    static const int waveHelloArray[12][9];
    static const int dance1Array[9][9];
    static const int dance2Array[8][9];
    static const int dance3Array[16][9];    
    static const int lieDownArray[2][9];
    static const int fightingArray[15][9];
    static const int pushUpsArray[21][9];
    static const int sleepArray[4][9];

    // Lookup table for all sequences
    static const MovementArray sequences[17];   // number of sequences (states)

    // Movement state management
    MovementState lastState;
    MovementState currentState;
    MovementState nextState;
    int currentStep;
    unsigned long stepStartTime;
    bool isMoving;
    unsigned long idleDuration;

    // Helper methods
    void setServoPositions(const int positions[]);
    void startMovementSequence(MovementState newState);

  public:
    MovementDriver();
    void begin();
    void update();

    // Movement commands
    void standby();
    void ready();
    void forward();
    void backward();
    void turnLeft();
    void turnRight();
    void moveLeft();
    void moveRight();
    void waveHello();
    void dance1();
    void dance2();
    void dance3();
    void lieDown();
    void fighting();
    void pushUps();
    void sleep();
    void idle(unsigned long duration, MovementState queuedState = IDLE);

    // State info
    MovementState getState() const { return currentState; }
    MovementState getLastState() const { return lastState; }

    bool isBusy();
};

#endif
