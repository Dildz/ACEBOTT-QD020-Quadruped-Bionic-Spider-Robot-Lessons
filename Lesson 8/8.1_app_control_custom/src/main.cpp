/*
 * Reworked app control project making use of a custom Movement_Driver and a custom WiFi_Driver.
 *
 * HOW IT WORKS:
 * - The robot starts up and creates its own Wi-Fi network
 * - It waits for a device to connect and sends movement commands via the control app
 * - When a command arrives, it figures out which movement to run, tells the movement system
 *   to perform the requested action and sends a response back to the control app
 * - The update() function keeps the movements smooth and continuous
 */


// INCLUDES
#include <Arduino.h>
#include "Movement_Driver.h"
#include "WiFi_Driver.h"

// DEFINES
#define CMD_RUN       1   // Movement command (walk, turn, etc.)
#define CMD_STANDBY   3   // Go to standby position
#define CMD_SLEEP     5   // Go to sleep position
#define CMD_LIEDOWN   6   // Lie down
#define CMD_WAVEHELLO 7   // Wave hello
#define CMD_PUSHUPS   8   // Do push-ups
#define CMD_FIGHTING  9   // Fighting pose
#define CMD_DANCE1    10  // Dance routine 1
#define CMD_DANCE2    11  // Dance routine 2
#define CMD_DANCE3    12  // Dance routine 3

// GLOBAL VARIABLES
const char* ssid = "QuadBot";
const char* password = "12345678";

// Create driver instances
MovementDriver robot;
WiFiDriver wifi;

// Response messages to send back to the app - Format: {0xFF, 0x55, length, device, action}
byte callbackForwardPackage[5]    =  {0xff, 0x55, 0x02, 0x01, 0x01};
byte callbackBackPackage[5]       =  {0xff, 0x55, 0x02, 0x01, 0x02};
byte callbackLeftMovePackage[5]   =  {0xff, 0x55, 0x02, 0x01, 0x03};
byte callbackRightMovePackage[5]  =  {0xff, 0x55, 0x02, 0x01, 0x04};
byte callbackTurnLeftPackage[5]   =  {0xff, 0x55, 0x02, 0x01, 0x05};
byte callbackTurnRightPackage[5]  =  {0xff, 0x55, 0x02, 0x01, 0x06};
byte callbackStandbyPackage[5]    =  {0xff, 0x55, 0x02, 0x01, 0x07};
byte callbackSleepPackage[5]      =  {0xff, 0x55, 0x02, 0x01, 0x08};
byte callbackLiePackage[5]        =  {0xff, 0x55, 0x02, 0x01, 0x09};
byte callbackHelloPackage[5]      =  {0xff, 0x55, 0x02, 0x01, 0x0a};
byte callbackPushupPackage[5]     =  {0xff, 0x55, 0x02, 0x01, 0x0b};
byte callbackFightingPackage[5]   =  {0xff, 0x55, 0x02, 0x01, 0x0c};
byte callbackDance1Package[5]     =  {0xff, 0x55, 0x02, 0x01, 0x0d};
byte callbackDance2Package[5]     =  {0xff, 0x55, 0x02, 0x01, 0x0e};
byte callbackDance3Package[5]     =  {0xff, 0x55, 0x02, 0x01, 0x0f};


// SETUP
void setup() {
  Serial.begin(115200);
  delay(100);

  Serial.println("\n=================================================================");
  Serial.println("Starting ACEBOTT QD020 Quadruped Bionic Spider Robot App Control.");
  Serial.println("=================================================================");

  // Initialize Wi-Fi
  wifi.begin(ssid, password);

  // Initialize movement driver
  Serial.println("\nInitializing movement driver...");
  robot.begin();

  // Start in standby mode
  robot.standby();
  Serial.println("Robot initialized in standby mode");
  Serial.println("Setup Complete!");
}

// MAIN LOOP
void loop() {
  // Check the client for a new command
  WiFiDriver::CommandData cmd = wifi.handleClient();

  // If we received a valid command, process it
  if (cmd.isValid) {
    switch(cmd.action) {
      case CMD_RUN:
        // // Movement commands (walking, turning)
        switch(cmd.movementType) {
          case 0x01:
            robot.forward();
            wifi.sendData(callbackForwardPackage, 5);
            break;
          case 0x02:
            robot.backward();
            wifi.sendData(callbackBackPackage, 5);
            break;
          case 0x03:
            robot.moveLeft();
            wifi.sendData(callbackLeftMovePackage, 5);
            break;
          case 0x04:
            robot.moveRight();
            wifi.sendData(callbackRightMovePackage, 5);
            break;
          case 0x05:
            robot.turnLeft();
            wifi.sendData(callbackTurnLeftPackage, 5);
            break;
          case 0x06:
            robot.turnRight();
            wifi.sendData(callbackTurnRightPackage, 5);
            break;
        }
        break;
        
      // Action commands (poses, dances)
      case CMD_STANDBY:
        robot.standby();
        wifi.sendData(callbackStandbyPackage, 5);
        break;
      case CMD_SLEEP:
        robot.sleep();
        wifi.sendData(callbackSleepPackage, 5);
        break;
      case CMD_LIEDOWN:
        robot.lieDown();
        wifi.sendData(callbackLiePackage, 5);
        break;
      case CMD_WAVEHELLO:
        robot.waveHello();
        wifi.sendData(callbackHelloPackage, 5);
        break;
      case CMD_PUSHUPS:
        robot.pushUps();
        wifi.sendData(callbackPushupPackage, 5);
        break;
      case CMD_FIGHTING:
        robot.fighting();
        wifi.sendData(callbackFightingPackage, 5);
        break;
      case CMD_DANCE1:
        robot.dance1();
        wifi.sendData(callbackDance1Package, 5);
        break;
      case CMD_DANCE2:
        robot.dance2();
        wifi.sendData(callbackDance2Package, 5);
        break;
      case CMD_DANCE3:
        robot.dance3();
        wifi.sendData(callbackDance3Package, 5);
        break;
    }
  }
  
  // Update robot movements
  robot.update();
}
