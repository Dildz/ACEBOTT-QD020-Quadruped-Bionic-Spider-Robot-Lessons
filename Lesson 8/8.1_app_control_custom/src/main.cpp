/*
 * ACEBOTT QD020 Quadruped Spider Robot - App Control
 *
 * The robot creates its own Wi-Fi network. Connect the control app to it,
 * then send movement commands. The OLED display shows eye animations that
 * match whatever the robot is doing.
 *
 * HOW IT WORKS:
 *   1. Robot starts up, display shows WiFi credentials
 *   2. App connects to the robot's WiFi network
 *   3. App sends movement commands (walk, turn, dance, etc.)
 *   4. Robot moves AND its eyes change expression to match
 *   5. Everything runs without blocking so movements stay smooth
 *
 * DRIVER RESPONSIBILITIES:
 *   Movement_Driver  - controls the 8 servo motors
 *   WiFi_Driver      - manages the WiFi network and app connection
 *   EyeAnimations    - decides what expression the eyes should show
 *   DisplayDriver    - owns the OLED display and renders the eye animations
 *
 * DISPLAY WIRING (I2C):
 *   There are no spare pins so the Rx & Tx pins are used for I2C communication with the display.
 *   SDA = RxD0 (GPIO3), SCL = TxD0 (GPIO1)
 *   Note: these pins are the serial pins, the serial monitor cannot be used with the display is connected.
 */


// INCLUDES
#include <Arduino.h>
#include "WiFi_Driver.h"
#include "Display_SSD1306.h"
#include "EyeAnimations_Driver.h"
#include "Movement_Driver.h"

// COMMAND IDs - match the control app protocol
#define CMD_RUN       1   // movement command (walk, turn, strafe)
#define CMD_STANDBY   3   // go to standby position
#define CMD_SLEEP     5   // go to sleep position
#define CMD_LIEDOWN   6   // lie down
#define CMD_WAVEHELLO 7   // wave hello
#define CMD_PUSHUPS   8   // do push-ups
#define CMD_FIGHTING  9   // fighting pose
#define CMD_DANCE1    10  // dance routine 1
#define CMD_DANCE2    11  // dance routine 2
#define CMD_DANCE3    12  // dance routine 3

// WIFI CREDENTIALS - app must connect to this network
const char* ssid     = "QuadBot";
const char* password = "12345678";

// DRIVER INSTANCES - each handles its own hardware/task independently
DisplayDriver  display(128, 64, &Wire, -1);  // width, height, I2C bus, reset pin
MovementDriver robot;
WiFiDriver     wifi;
EyeAnimations  eyes;

// WIFI CONNECTION STATE
bool wasStationConnected = false;  // tracks AP station presence (not TCP client)

// APP RESPONSE PACKETS - sent back to confirm each command was received
// Format: {0xFF, 0x55, length, device, action}
byte callbackForwardPackage[5]    = {0xff, 0x55, 0x02, 0x01, 0x01};
byte callbackBackPackage[5]       = {0xff, 0x55, 0x02, 0x01, 0x02};
byte callbackLeftMovePackage[5]   = {0xff, 0x55, 0x02, 0x01, 0x03};
byte callbackRightMovePackage[5]  = {0xff, 0x55, 0x02, 0x01, 0x04};
byte callbackTurnLeftPackage[5]   = {0xff, 0x55, 0x02, 0x01, 0x05};
byte callbackTurnRightPackage[5]  = {0xff, 0x55, 0x02, 0x01, 0x06};
byte callbackStandbyPackage[5]    = {0xff, 0x55, 0x02, 0x01, 0x07};
byte callbackSleepPackage[5]      = {0xff, 0x55, 0x02, 0x01, 0x08};
byte callbackLiePackage[5]        = {0xff, 0x55, 0x02, 0x01, 0x09};
byte callbackHelloPackage[5]      = {0xff, 0x55, 0x02, 0x01, 0x0a};
byte callbackPushupPackage[5]     = {0xff, 0x55, 0x02, 0x01, 0x0b};
byte callbackFightingPackage[5]   = {0xff, 0x55, 0x02, 0x01, 0x0c};
byte callbackDance1Package[5]     = {0xff, 0x55, 0x02, 0x01, 0x0d};
byte callbackDance2Package[5]     = {0xff, 0x55, 0x02, 0x01, 0x0e};
byte callbackDance3Package[5]     = {0xff, 0x55, 0x02, 0x01, 0x0f};


// SETUP
void setup() {
  // Seed the random number generator (used for idle eye roaming)
  randomSeed(analogRead(A0));

  // Initialise servos and move to standby position
  robot.begin();
  robot.standby();

  // Start the robot's WiFi network
  wifi.begin(ssid, password);

  // Show startup screens and WiFi credentials on the OLED
  display.setWiFiCredentials(ssid, password);
  display.begin();
}


// MAIN LOOP
void loop() {

  // --- CONNECTION CHECK ---
  // Track AP station presence (phone on WiFi), not TCP client (app connected).
  // Eyes start as soon as a device joins the AP; revert when the AP is empty.
  bool isStationConnected = wifi.isStationConnected();
  if (isStationConnected && !wasStationConnected) {
    // Device joined the AP - start eye animations
    display.beginEyes();  // initialise RoboEyes on the display
    eyes.begin();         // set initial eye expression (standby)
  }
  else if (!isStationConnected && wasStationConnected) {
    // Device left the AP - return to standby and show WiFi credentials
    robot.standby();
    display.showWiFiInfo();
  }
  wasStationConnected = isStationConnected;

  // --- COMMAND HANDLING ---
  // Check for an incoming command from the app
  WiFiDriver::CommandData cmd = wifi.handleClient();

  if (cmd.isValid) {
    switch (cmd.action) {

      case CMD_RUN:  // movement commands - walk, turn, strafe
        switch (cmd.movementType) {
          case 0x01:
            robot.forward();
            eyes.setMovementState(FORWARD);
            wifi.sendData(callbackForwardPackage, 5);
            break;
          case 0x02:
            robot.backward();
            eyes.setMovementState(BACKWARD);
            wifi.sendData(callbackBackPackage, 5);
            break;
          case 0x03:
            robot.moveLeft();
            eyes.setMovementState(MOVE_LEFT);
            wifi.sendData(callbackLeftMovePackage, 5);
            break;
          case 0x04:
            robot.moveRight();
            eyes.setMovementState(MOVE_RIGHT);
            wifi.sendData(callbackRightMovePackage, 5);
            break;
          case 0x05:
            robot.turnLeft();
            eyes.setMovementState(TURN_LEFT);
            wifi.sendData(callbackTurnLeftPackage, 5);
            break;
          case 0x06:
            robot.turnRight();
            eyes.setMovementState(TURN_RIGHT);
            wifi.sendData(callbackTurnRightPackage, 5);
            break;
        }
        break;

      case CMD_STANDBY:
        robot.standby();
        eyes.setMovementState(STANDBY);
        wifi.sendData(callbackStandbyPackage, 5);
        break;

      case CMD_SLEEP:
        robot.sleep();
        eyes.setMovementState(SLEEP);
        wifi.sendData(callbackSleepPackage, 5);
        break;

      case CMD_LIEDOWN:
        robot.lieDown();
        eyes.setMovementState(LIE_DOWN);
        wifi.sendData(callbackLiePackage, 5);
        break;

      case CMD_WAVEHELLO:
        robot.waveHello();
        eyes.setMovementState(WAVE_HELLO);
        wifi.sendData(callbackHelloPackage, 5);
        break;

      case CMD_PUSHUPS:
        robot.pushUps();
        eyes.setMovementState(PUSH_UPS);
        wifi.sendData(callbackPushupPackage, 5);
        break;

      case CMD_FIGHTING:
        robot.fighting();
        eyes.setMovementState(FIGHTING);
        wifi.sendData(callbackFightingPackage, 5);
        break;

      case CMD_DANCE1:
        robot.dance1();
        eyes.setMovementState(DANCE1);
        wifi.sendData(callbackDance1Package, 5);
        break;

      case CMD_DANCE2:
        robot.dance2();
        eyes.setMovementState(DANCE2);
        wifi.sendData(callbackDance2Package, 5);
        break;

      case CMD_DANCE3:
        robot.dance3();
        eyes.setMovementState(DANCE3);
        wifi.sendData(callbackDance3Package, 5);
        break;
    }
  }

  // --- IDLE EYE ROAMING ---
  eyes.update();  // periodically shifts gaze direction during standby

  // --- EYE ANIMATION SYNC ---
  // When the movement state changes, tell the display what expression to show.
  // acknowledgeChange() clears the flag so this only fires once per state change.
  if (eyes.hasStateChanged()) {
    display.applyEyeState(eyes.getState());
    eyes.acknowledgeChange();
  }

  // --- UPDATE ALL DRIVERS ---
  bool wasBusy = robot.isBusy();
  robot.update();    // progress movement sequences (non-blocking step timing)

  // When a movement sequence finishes, return eyes to standby.
  // Covers both directional moves (eyes back to center) and movement sets
  // (custom bitmap animation stops). Guards against STANDBY/IDLE/READY
  // completions which don't need a reset.
  if (wasBusy && !robot.isBusy()) {
    MovementState finished = robot.getState();
    if (finished != STANDBY && finished != IDLE && finished != READY) {
      eyes.setMovementState(STANDBY);
    }
  }

  display.update();  // render the current eye animation frame
}
