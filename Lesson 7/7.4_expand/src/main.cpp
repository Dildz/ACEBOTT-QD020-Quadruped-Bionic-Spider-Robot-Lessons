/*
 * This sketch combines the robot's movement functions from previous lessons into one program.
 * We introduce the creation & use of a custom library and handle movements via a state machine.
 * 
 * Program flow:
 *   - Start in standby()
 *   - Transition to ready()
 *   - Then follow sequence:
       forward twice, turn left, turn right, backward twice, move left, move right, wave hello, perfom dance routines, loop back to forward.
 * 
 * All servo control logic and position arrays are in the Movement_Driver,
 * so the main sketch only controls movement transitions and can run other tasks.
 * 
 * NOTES:
 * - To simplify the main sketch, all servo objects, position arrays, and movement logic
 *   are encapsulated inside the Movement_Driver library.
 * - The Servo.h include is moved into Movement_Driver.h, so the implementation file (Movement_Driver.cpp)
 *   can access it directly.
 * - The main sketch only needs to include Movement_Driver.h and call the high-level movement methods.
 * - For the idle state we can pass the period & queue the next state.
 */


// INCLUDES
#include <Arduino.h>
#include "Movement_Driver.h"

// INSTANCES
MovementDriver robot;   // create movement driver instance

// GLOBAL VARIABLES
// Timer variables for seconds counter
unsigned long previousMillis = 0;
const long interval = 1000;   // 1 second interval
int secondsCounter = 0;

// Movement counters
byte repeatAmount = 2;    // how many times to repeat a movement
byte forwardCount = 0;
byte backwardCount = 0;
int idleTime = 1000;      // 1 second idle time


// SETUP
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\nSerial monitor started");

  // Initialize the movement driver
  robot.begin();
  
  // Start in standby mode
  robot.standby();
  Serial.println("\nStandy mode active");
  Serial.println("\nSetup Complete\n");
  delay(1000);
}

// MAIN LOOP
void loop() {
  // Always call update to handle movement sequencing
  robot.update();
  
  // Check if robot is moving or idle
  if (!robot.isBusy()) {
    switch (robot.getState()) {
      // After standby, move to ready
      case STANDBY:
        Serial.println("Moving to ready");
        robot.ready();
        break;
      
      // After ready, start the forward movement
      case READY:
        Serial.println("Moving forward");
        robot.forward();
        break;
      
      // After forward, check if we need to do it again or move to turn left
      case FORWARD:
        forwardCount++;
        if (forwardCount < repeatAmount) {
          Serial.println("Continuing forward");
          robot.forward();
        }
        else {
          Serial.println("Forward complete, moving to turn left");
          forwardCount = 0;
          robot.idle(idleTime, TURN_LEFT);    // move left after idle time
        }
        break;
      
      // After turn left, move to turn right
      case TURN_LEFT:
        Serial.println("Turn left complete, moving to turn right");
        robot.turnRight();
        break;
      
      // After turn right, move to backward
      case TURN_RIGHT:
        Serial.println("Turn right complete, moving to backward");
        robot.idle(idleTime, BACKWARD);    // move backward after idle time
        break;
      
      // After backward, check if we need to do it again or move to (strafe) left
      case BACKWARD:
        backwardCount++;
        if (backwardCount < repeatAmount) {
          Serial.println("Continuing backward");
          robot.backward();
        }
        else {
          Serial.println("Backward complete, moving to strafe left");
          backwardCount = 0;
          robot.idle(idleTime, MOVE_LEFT);    // strafe left after idle time
        }
        break;

      // After strafe left, move to strafe right
      case MOVE_LEFT:
        Serial.println("Strafe left complete, moving to strafe right");
        robot.moveRight();
        break;

      // After strafe right, move to wave hello
      case MOVE_RIGHT:
        Serial.println("Strafe right complete, moving to wave hello");
        robot.idle(idleTime, WAVE_HELLO);    // wave hello after idle time
        break;

      // After wave hello, move back to dance routine 1
      case WAVE_HELLO:
        Serial.println("Wave hello complete, moving to dance routine 1");
        robot.idle(idleTime, DANCE1);    // dance routine 1 after idle time
        break;

      // After dance routine 1, move to dance routine 2
      case DANCE1:
        Serial.println("Dance routine 1 complete, moving to dance routine 2");
        robot.idle(idleTime, DANCE2);    // dance routine 2 after idle time
        break;

      // After dance routine 2, move to dance routine 3
      case DANCE2:
        Serial.println("Dance routine 2 complete, moving to dance routine 3");
        robot.idle(idleTime, DANCE3);    // dance routine 3 after idle time
        break;

      // After dance routine 3, move to forward
      case DANCE3:
        Serial.println("Dance routine 3 complete, moving to back to forward");
        robot.idle(idleTime, FORWARD);    // forward after idle time
        break;

      default:
        break;
    }
  }

  // Example of additional task (counter) that runs together with robot movements
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis >= interval) {
    // Save the last time we updated the counter
    previousMillis = currentMillis;
    
    // Increment the seconds counter & print to serial monitor
    secondsCounter++;
    Serial.print("Seconds Counter: ");
    Serial.println(secondsCounter);
    
    /* We could add other periodic tasks here that need to run every second
     * For example:
     * - read sensor values
     * - check battery level
     * - update a display */
  }

  /* We could add other non-blocking tasks here
   * For example: 
   * - Read buttons or other inputs
   * - Communicate with other devices
   * These will all run together with the robot movements */
}


/*
 * LESSONS LEARNED:
 * 
 * CUSTOM MOVEMENT DRIVER LIBRARY:
 * - A custom Movement_Driver library is used to control servo motors.
 * - To create a custom library we need to create a new folder structure inside of the 'lib' project folder:
 *   ── lib
 *      └── Movement_Driver
 *          └── src
 *              ├── library.json (library metadata & dependencies)
 *              ├── Movement_Driver.h (class declaration & interface)
 *              └── Movement_Driver.cpp (class implementation)
 * 
 * - library.json
 *   - Informs PlatformIO about the custom library with metadata & any dependencies
 *   - External libraries (dependencies) can be listed here if needed for a project
 * 
 * - Movement_Driver.h
 *   - Declares the MovementDriver class, state machine enum, and the 
 *     movement arrays for standby, ready, forward, and backward (idle will use the last position)
 *   - Provides the public API:
 *     standby(), ready(), forward(), backward(), turnLeft(), turnRight(), moveLeft(), moveRight(), waveHello(), dance1(), dance2(), dance3(), idle(), update()
 * 
 * - Movement_Driver.cpp
 *   - Implements all class methods and non-blocking movement logic
 *   - Uses a lookup table (sequences[]) to map states to movement arrays, 
 *     reducing code duplication
 * 
 * - Why?:
 *   - Clean separation of interface (header) and implementation (cpp)
 *   - Keeps servo control and movement logic isolated from the main sketch
 *   - Can be easily extended with new movement patterns
 *   - Main sketch remains simple, calling only high-level commands like robot.forward()
 * 
 * NON-BLOCKING STATE MACHINE:
 * - All blocking delay() calls are removed in favor of a state machine driven by millis()
 * - The update() method is called in the main loop to process servo steps
 * - Movements are handled asynchronously, so the ESP8266 can run other tasks in parallel
 * - The state machine automatically advances:
 *   STANDBY → READY → FORWARD → TURN_LEFT → TURN_RIGHT → BACKWARD → MOVE_LEFT → MOVE_RIGHT → WAVE_HELLO → DANCE ROUTINES → FORWARD with IDLE periods
 * 
 * - How/Why?:
 *   - Each step’s duration is defined in the last column of the position arrays
 *   - update() checks elapsed time with millis() and moves to the next step when ready
 *   - Because timing is non-blocking, other tasks (sensors, displays, comms) 
 *     continue to run while the robot is moving
 *   - Easy to add extra movement steps to the movement driver and main sketch
 */