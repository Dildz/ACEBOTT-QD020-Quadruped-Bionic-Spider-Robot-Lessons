/*
 * ACEBOTT QD020 Quadruped Spider Robot - App Control
 *
 * The robot creates its own Wi-Fi network. Connect the control app to it,
 * then send movement commands. The OLED display shows eye animations that
 * match whatever the robot is doing.
 *
 * HOW IT WORKS:
 *   1. Robot starts up, display shows WiFi credentials
 *   2. Connect device to the robot's WiFi network, open the app and connect to the robot
 *   3. App sends movement commands (walk, turn, dance, etc.)
 *   4. Robot moves AND its eyes change expression to match
 *   5. Everything runs one step at a time — no pausing the whole program
 *
 * THE 4 DRIVERS (each handles one job):
 *   Movement_Driver  - controls the 8 servo motors
 *   WiFi_Driver      - manages the WiFi network and app connection
 *   EyeAnimations    - decides what expression the eyes should show
 *   DisplayDriver    - owns the OLED display and draws the eye animations
 *
 * DISPLAY WIRING (I2C):
 *   There are no spare pins so the Rx & Tx pins are used for I2C communication with the display.
 *   SDA = RxD0 (GPIO3), SCL = TxD0 (GPIO1)
 *   Note: these are the serial pins, so the serial monitor can't be used while the display is connected.
 */


// INCLUDES
#include <Arduino.h>
#include "WiFi_Driver.h"
#include "Display_SSD1306.h"
#include "EyeAnimations_Driver.h"
#include "Movement_Driver.h"

// COMMAND IDs - these numbers match what the control app sends
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

// WIFI CREDENTIALS - the controller app device must connect to this network
// Change these to rename the robot's WiFi or set a different password
const char* ssid     = "QuadBot";
const char* password = "12345678";

// DRIVER INSTANCES - each handles its own hardware or task independently
DisplayDriver  display(128, 64, &Wire, -1);  // width, height, I2C bus, reset pin
MovementDriver robot;
WiFiDriver     wifi;
EyeAnimations  eyes;

// Remembers whether a phone was connected last loop so we can detect changes
bool wasStationConnected = false;


// Sends a 5-byte confirmation back to the app so it knows we received the command.
// The responseId tells the app which command was confirmed.
void sendAck(byte responseId) {
  byte packet[5] = {0xFF, 0x55, 0x02, 0x01, responseId};
  wifi.sendData(packet, 5);
}


// =============================================================================
// COMMAND HANDLER TABLE
// =============================================================================
// Each row links one command from the app to: the robot move it should run,
// the eye expression to show, and the ack byte to send back.
//
// For CMD_RUN (movement commands), the app also sends a "movement type" byte
// to pick between forward/backward/turn/etc. - that's the subAction column.
// For all other commands subAction is unused (set to 0).
//
// To add a new command: define a new CMD_* number above, then add one row here.
// =============================================================================
struct CommandHandler {
  uint8_t                 action;     // top-level command id (CMD_*)
  uint8_t                 subAction;  // movement type (only used for CMD_RUN)
  void (MovementDriver::* move)();    // robot method to call
  MovementState           eyeState;   // eye expression that goes with the move
  uint8_t                 ackByte;    // the byte the app expects in the reply
};

static const CommandHandler handlers[] = {
  // CMD_RUN movement variants - matched on both action AND subAction
  { CMD_RUN,       0x01, &MovementDriver::forward,   FORWARD,    0x01 },
  { CMD_RUN,       0x02, &MovementDriver::backward,  BACKWARD,   0x02 },
  { CMD_RUN,       0x03, &MovementDriver::moveLeft,  MOVE_LEFT,  0x03 },
  { CMD_RUN,       0x04, &MovementDriver::moveRight, MOVE_RIGHT, 0x04 },
  { CMD_RUN,       0x05, &MovementDriver::turnLeft,  TURN_LEFT,  0x05 },
  { CMD_RUN,       0x06, &MovementDriver::turnRight, TURN_RIGHT, 0x06 },
  // Standalone commands - matched on action only
  { CMD_STANDBY,   0,    &MovementDriver::standby,   STANDBY,    0x07 },
  { CMD_SLEEP,     0,    &MovementDriver::sleep,     SLEEP,      0x08 },
  { CMD_LIEDOWN,   0,    &MovementDriver::lieDown,   LIE_DOWN,   0x09 },
  { CMD_WAVEHELLO, 0,    &MovementDriver::waveHello, WAVE_HELLO, 0x0a },
  { CMD_PUSHUPS,   0,    &MovementDriver::pushUps,   PUSH_UPS,   0x0b },
  { CMD_FIGHTING,  0,    &MovementDriver::fighting,  FIGHTING,   0x0c },
  { CMD_DANCE1,    0,    &MovementDriver::dance1,    DANCE1,     0x0d },
  { CMD_DANCE2,    0,    &MovementDriver::dance2,    DANCE2,     0x0e },
  { CMD_DANCE3,    0,    &MovementDriver::dance3,    DANCE3,     0x0f },
};
static const size_t handlerCount = sizeof(handlers) / sizeof(handlers[0]);


// Look up the incoming command in the handlers table and run it.
// Unknown commands are silently ignored.
static void dispatchCommand(const WiFiDriver::CommandData& cmd) {
  for (size_t i = 0; i < handlerCount; ++i) {
    const CommandHandler& h = handlers[i];
    if (h.action != cmd.action) continue;
    if (cmd.action == CMD_RUN && h.subAction != cmd.movementType) continue;
    (robot.*h.move)();
    eyes.setMovementState(h.eyeState);
    sendAck(h.ackByte);
    return;
  }
}


// SETUP - runs once when the robot powers on
void setup() {
  // Seed the random number generator (used for idle eye roaming)
  randomSeed(analogRead(A0));

  // Start the servos, fold legs in, and wait for the move to finish before proceeding.
  // Spinning update() here lets the sequence timer expire and detach the servos
  // before setup() continues — without this, servos hold position until loop() starts.
  robot.begin();
  robot.storage();
  while (robot.isBusy()) { robot.update(); }

  // Start the robot's WiFi network
  wifi.begin(ssid, password);

  // Show startup screens and WiFi credentials on the OLED
  display.setWiFiCredentials(ssid, password);
  display.begin();
}


// MAIN LOOP - runs over and over as fast as possible
void loop() {

  // --- CONNECTION CHECK ---
  // Watch for a device joining or leaving the robot's WiFi network.
  // Eyes start as soon as a device connects; go back to the info screen when it disconnects.
  bool isStationConnected = wifi.isStationConnected();
  if (isStationConnected && !wasStationConnected) {
    // A device just connected - unfold to standby and start the eye animations
    robot.standby();
    display.beginEyes();  // set up RoboEyes on the display
    eyes.begin();         // set initial eye expression (standby)
  }
  else if (!isStationConnected && wasStationConnected) {
    // The device just disconnected - fold legs in and show WiFi info
    robot.storage();
    display.showWiFiInfo();
  }
  wasStationConnected = isStationConnected;

  // --- COMMAND HANDLING ---
  // Check if the app sent a command this loop, then dispatch via the handler table.
  WiFiDriver::CommandData cmd = wifi.handleClient();
  if (cmd.isValid) {
    dispatchCommand(cmd);
  }

  // --- IDLE EYE ROAMING ---
  eyes.update();  // occasionally shifts the gaze direction while the robot is standing still

  // --- EYE ANIMATION SYNC ---
  // When the movement state changes, tell the display what expression to show.
  // acknowledgeChange() clears the flag so this only fires once per state change.
  if (eyes.hasStateChanged()) {
    display.applyEyeState(eyes.getState());
    eyes.acknowledgeChange();
  }

  // --- UPDATE ALL DRIVERS ---
  bool wasBusy = robot.isBusy();
  robot.update();    // advance the movement sequence one step at a time (no delays)

  // When a movement sequence finishes, return eyes to standby.
  // Covers directional moves (eyes back to center) and special animations.
  // Ignores STANDBY/IDLE/READY completions since they don't need a reset.
  if (wasBusy && !robot.isBusy()) {
    MovementState finished = robot.getState();
    if (finished != STANDBY && finished != IDLE && finished != READY && finished != STORAGE) {
      eyes.setMovementState(STANDBY);
    }
  }

  display.update();  // draw the current eye animation frame to the screen
}
