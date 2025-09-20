/*
 * Reworked app control project making use of a custom movement driver.
 */


// INCLUDES
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "Movement_Driver.h"


// DEFINES
#define CMD_RUN       1   // Identifier for the forward command
#define CMD_STANDBY   3   // Identifier for the standby command
#define CMD_SLEEP     5   // Identifier for the sleep command
#define CMD_LIEDOWN   6   // Identifier for the lie down command
#define CMD_WAVEHELLO 7   // Identifier for the wave hello command
#define CMD_PUSHUPS   8   // Identifier for the push ups command
#define CMD_FIGHTING  9   // Identifier for the fighting command
#define CMD_DANCE1    10  // Identifier for the dance routine 1 command
#define CMD_DANCE2    11  // Identifier for the dance routine 2 command
#define CMD_DANCE3    12  // Identifier for the dance routine 3 command


// GLOBAL VARIABLES
const char* ssid = "QuadBot";         // Wi-Fi name of the quadruped robot
const char* password = "12345678";    // Wi-Fi password for the quadruped robot

WiFiServer server(100);               // Create a Wi-Fi server object with port 100
WiFiClient client;                    // Create a Wi-Fi client object
String sendBuffer;                    // Send buffer

// Movement driver instance
MovementDriver robot;

// Command callback packages
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

// Communication variables
byte dataLength = 0;
byte bufferIndex = 0;
char receiveBuffer[52];
unsigned char previousChar = 0;
bool isStartReceiving = false;
bool isClientConnected = true;
bool isWiFiActive = false;
bool isStandbyTriggered = false;


// HELPER FUNCTIONS
unsigned char readBuffer(int index) {   // Read the characters in the buffer with index and return
  return receiveBuffer[index]; 
}

void writeBuffer(int index, unsigned char character) {
  receiveBuffer[index] = character;    // Write the character c to the buffer at index index_w position
}

void runMovementModule(int device) {
  switch(device) {
    case 0x0C: {   
      int movementType = readBuffer(12);
      switch (movementType) {
        case 0x01:       
          robot.forward();
          client.write(callbackForwardPackage, 5);
          break;
        case 0x02:
          robot.backward();
          client.write(callbackBackPackage, 5);
          break;
        case 0x03:
          robot.moveLeft();
          client.write(callbackLeftMovePackage, 5);
          break;
        case 0x04:
          robot.moveRight();
          client.write(callbackRightMovePackage, 5);
          break;
        case 0x05:
          robot.turnLeft();
          client.write(callbackTurnLeftPackage, 5);
          break;
        case 0x06:
          robot.turnRight();
          client.write(callbackTurnRightPackage, 5);
          break;
        default:
          break;
      }
    }
    break;
  }   
}

void parseReceivedData() { 
  isStartReceiving = false;
  int action = readBuffer(9);
  int device = readBuffer(10);
  
  switch(action) {
    case CMD_RUN: {
      runMovementModule(device);
    }
    break;
    case CMD_STANDBY: {
      robot.standby();
      client.write(callbackStandbyPackage, 5);
    }
    break;
    case CMD_SLEEP: {
      robot.sleep();
      client.write(callbackSleepPackage, 5);
    }    
    break;
    case CMD_LIEDOWN: {
      robot.lieDown();
      client.write(callbackLiePackage, 5);
    }
    break;
    case CMD_WAVEHELLO: {
      robot.waveHello();
      client.write(callbackHelloPackage, 5);
    }
    break;
    case CMD_PUSHUPS: {
      robot.pushUps();
      client.write(callbackPushupPackage, 5);
    }
    break;
    case CMD_FIGHTING: {
      robot.fighting();
      client.write(callbackFightingPackage, 5);
    }
    break;
    case CMD_DANCE1: {
      robot.dance1();
      client.write(callbackDance1Package, 5);
    }
    break;
    case CMD_DANCE2: {
      robot.dance2();
      client.write(callbackDance2Package, 5);
    }
    break;
    case CMD_DANCE3: {
      robot.dance3();
      client.write(callbackDance3Package, 5);
    }
    break;
  }
}


// SETUP
void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("\n=================================================================");
  Serial.println("Starting ACEBOTT QD020 Quadruped Bionic Spider Robot App Control.");
  Serial.println("=================================================================");

  // Initialize Wi-Fi
  Serial.println("\nInitializing Wi-Fi...");
  WiFi.mode(WIFI_AP);           // Set Wi-Fi mode to access point mode
  WiFi.softAP(ssid,password,5); // Create a Wi-Fi access point with the network name and password, and use channel 5
  server.begin();               // Start the Wi-Fi server
  delay(100);
  Serial.println("Wi-Fi ready, initializing movement driver...");

  // Initialize movement driver
  robot.begin();
  
  // Start in standby mode
  robot.standby();
  Serial.println("Robot initialized in standby mode");
  Serial.println("Setup Complete!");
  Serial.print("\nConnect to SSID: ");
  Serial.print(ssid);
  Serial.print(" with password: ");
  Serial.print(password);
  Serial.print(" to control the robot\n");
}

// MAIN LOOP
void loop() {
  client = server.accept();

  if (client) {
    isWiFiActive = true;
    isClientConnected = true;
    Serial.println("[Client connected]");
    
    unsigned long previousMillis = millis();  
    const unsigned long timeoutDuration = 3000;

    while (client.connected()) {
      // Handle timeout
      if ((millis() - previousMillis) > timeoutDuration && 
          client.available() == 0 && 
          isStandbyTriggered == true) {
        break;
      }

      // Process incoming data
      if (client.available()) {
        previousMillis = millis();
        unsigned char receivedChar = client.read() & 0xff;
        Serial.write(receivedChar);
        isStandbyTriggered = false;

        if (receivedChar == 200) {
          isStandbyTriggered = true;
        }

        // Protocol parsing
        if(receivedChar == 0x55 && isStartReceiving == false) {
          if(previousChar == 0xff) {
            bufferIndex = 1; 
            isStartReceiving = true;
          }
        }
        else {
          previousChar = receivedChar;
          if(isStartReceiving) {
            if(bufferIndex == 2) {
              dataLength = receivedChar; 
            }
            else if(bufferIndex > 2) {
              dataLength--;
            }
            writeBuffer(bufferIndex, receivedChar);
          }
        }

        bufferIndex++;

        if(bufferIndex > 120) {
          bufferIndex = 0; 
          isStartReceiving = false;
        }

        if(isStartReceiving && dataLength == 0 && bufferIndex > 3) { 
          isStartReceiving = false;
          parseReceivedData();
          bufferIndex = 0;
        }
      }

      // Handle serial data to send to client
      if (Serial.available()) {
        char serialChar = Serial.read();
        sendBuffer += serialChar;
        client.print(sendBuffer);
        Serial.print(sendBuffer);
        sendBuffer = "";
      }

      // Check WiFi status periodically
      static unsigned long wifiCheckTime = 0;
      if (millis() - wifiCheckTime > 1000) {
        wifiCheckTime = millis();
        if (0 == (WiFi.softAPgetStationNum())) {
          break;
        }
      }

      // Update robot movements (non-blocking)
      robot.update();
    }

    // Client disconnected
    client.stop();
    robot.standby();  // Return to standby when client disconnects
    Serial.println("[Client disconnected]");
  }
  else {
    if (isClientConnected == true) {
      isClientConnected = false;
    }
  }

  // Always update robot movements in main loop
  robot.update();
}

