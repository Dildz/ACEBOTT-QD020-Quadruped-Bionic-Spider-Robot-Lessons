/*
 * This sketch combines the robot's standby, ready, move forward and backward functions into one program.
 * We introduce the creation & use of a custom library and handle movements via a state machine.
 * 
 * Program flow:
 *   - Start in standby()
 *   - Transition to ready()
 *   - Then alternate between forward() and backward() movements indefinitely with idle periods
 * 
 * All servo control logic and position arrays are in the Movement_Driver,
 * so the main sketch only controls movement transitions and can run other tasks.
 * 
 * NOTES:
 * - To simplify the main sketch, all servo objects, position arrays, and movement logic
 *   are encapsulated inside the Movement_Driver library.
 * - The Servo.h include is moved into Movement_Driver.h, so the implementation file (Movement_Driver.cpp)
 *   can access it directly.
 * - The main sketch only needs to include Movement_Driver.h and call the high-level movement methods
 *   ( robot.standby(), (robot.ready(), robot.forward(), (robot.backward() ).
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
      // After ready, start the forward/backward cycle
      case READY:
        Serial.println("Moving forward");
        robot.forward();
        break;
      // After forward, check if we need to do it again or move to idle
      case FORWARD:
        forwardCount++;
        if (forwardCount < repeatAmount) {   // move forward by the reapeat amount
          Serial.println("Continuing forward");
          robot.forward();
        }
        else {
          Serial.println("Pausing in IDLE - backward queued");
          forwardCount = 0;   // reset count for the next loop
          robot.idle(2000, BACKWARD);
        }
        break;
      // After backward, check if we need to do it again or move to idle
      case BACKWARD:
        backwardCount++;
        if (backwardCount < repeatAmount) {   // move forward by the reapeat amount
          Serial.println("Continuing backward");
          robot.backward();
        }
        else {
          Serial.println("Pausing in IDLE - forward queued");
          backwardCount = 0;   // reset count for the next loop
          robot.idle(2000, FORWARD);
        }
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
 *   - Provides the public API: standby(), ready(), forward(), backward(), idle(), update()
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
 * - The state machine automatically advances: STANDBY → READY → FORWARD ↔ BACKWARD with IDLE periods
 * 
 * - How/Why?:
 *   - Each step’s duration is defined in the last column of the position arrays
 *   - update() checks elapsed time with millis() and moves to the next step when ready
 *   - Because timing is non-blocking, other tasks (sensors, displays, comms) 
 *     continue to run while the robot is moving
 */