/*
 * Modifications to the original ArduinoIDE project:
 * - text.h copied into the 'include' folder; copied .ino file to 'src' & renamed to main.cpp
 * - cleaned, translated & repositioned code sections
 * - removed interpolation logic in the Servo_PROGRAM_Run function in favour of
 *   direct writing to the servos as this was causing movements to be incredibly slow!
 * - substituted movement arrays from the previous lessons
 * - SERVOMIN & SERVOMAX changed from 400 / 2400 -to- 500 / 2500
 * 
 * The intention was to get it workig in PlatformIO as close to 'as-is' as possible, with QoL fixes.
 * Another version (8.1_app_control_custom) will use the methods in the reworked 'expand' lessons
 * (custom driver libraries & non-blocking state machine logic)
 */


// INCLUDES
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "text.h"


// DEFINES
#define CMD_RUN       1   // Forward command identity
#define CMD_standby   3   // Flag for standby command
#define CMD_sleep     5   // Identifier for the sleep command
#define CMD_liedown   6   // The flag of the planking command
#define CMD_wavehello 7   // Flag for the say hello command
#define CMD_pushpus   8   // The flag for the push up command
#define CMD_fighting  9   // Battle command identifier
#define CMD_dance1    10  // The identity of the Dance 1 command
#define CMD_dance2    11  // The identity of the Dance 2 command
#define CMD_dance3    12  // The identity of the Dance 3 command


// GLOBAL VARIABLES
const char* ssid = "QuadBot";         // Wi-Fi name of the quadruped robot
const char* password = "12345678";    // Wi-Fi password for the quadruped robot

WiFiServer server(100);   // Create a Wi-Fi server object with port 100
WiFiClient client;        // Create a Wi-Fi client object
String sendBuff;          // Send buffer

byte RX_package[17] = {0};                                        // array of incoming packets
byte callback_forward_package[5] = {0xff,0x55,0x02,0x01,0x01};    // forward command packet
byte callback_back_package[5] = {0xff,0x55,0x02,0x01,0x02};       // back command packet
byte callback_leftmove_package[5] = {0xff,0x55,0x02,0x01,0x03};   // move command packet left
byte callback_rightmove_package[5] = {0xff,0x55,0x02,0x01,0x04};  // right shift command packet
byte callback_turnleft_package[5] = {0xff,0x55,0x02,0x01,0x05};   // left turn command packet
byte callback_turnright_package[5] = {0xff,0x55,0x02,0x01,0x06};  // right turn command packet
byte callback_standby_package[5] = {0xff,0x55,0x02,0x01,0x07};    // standby command packet
byte callback_sleep_package[5] = {0xff,0x55,0x02,0x01,0x08};      // sleep command packet
byte callback_lie_package[5] = {0xff,0x55,0x02,0x01,0x09};        // lie command packet
byte callback_hello_package[5] = {0xff,0x55,0x02,0x01,0x0a};      // hello command packet
byte callback_pushup_package[5] = {0xff,0x55,0x02,0x01,0x0b};     // push-up command packet
byte callback_fighting_package[5] = {0xff,0x55,0x02,0x01,0x0c};   // fighting command packet
byte callback_dance1_package[5] = {0xff,0x55,0x02,0x01,0x0d};     // dance1 command packet
byte callback_dance2_package[5] = {0xff,0x55,0x02,0x01,0x0e};     // dance2 command packet
byte callback_dance3_package[5] = {0xff,0x55,0x02,0x01,0x0f};     // dance3 command packet

byte dataLen, index_a = 0;    // Data length and index variable
char buffer[52];              // Buffer
unsigned char prevc=0;        // previous character
bool isStart = false;         // Flag if receive has started
bool ED_client = true;        // Flag if the client is connected
bool WA_en = false;           // Mark if WA is enabled
bool st = false;


// HELPER FUNCTIONS
unsigned char readBuffer(int index_r) {   // Read the characters in the buffer with index index_r and return
  return buffer[index_r]; 
}

void writeBuffer(int index_w, unsigned char c) {
  buffer[index_w] = c;    // Write the character c to the buffer at index index_w position
}

void runModule(int device) {
  switch(device) {
    case 0x0C: {   
      int val = readBuffer(12);
      switch (val) {
        case 0x01:       
          forward();
          client.write(callback_forward_package,5);
          break;
        case 0x02:
          back();
          client.write(callback_back_package,5);
          break;
        case 0x03:
          leftmove();
          client.write(callback_leftmove_package,5);
          break;
        case 0x04:
          rightmove();
          client.write(callback_rightmove_package,5);
          break;
        case 0x05:
          turnleft();
          client.write(callback_turnleft_package,5);
          break;
        case 0x06:
          turnright();
          client.write(callback_turnright_package,5);
          break;
        default:
          break;
      }
    }
    break;
  }   
}

void parseData() { 
  isStart = false;
  int action = readBuffer(9);
  int device = readBuffer(10);
  switch(action) {
    case CMD_RUN: {
      runModule(device);
    }
    break;

    case CMD_standby: {
      standby();
      client.write(callback_standby_package,5);
    }
    break;

    case CMD_sleep: {
      sleep();
      client.write(callback_sleep_package,5);
    }    
    break;

    case CMD_liedown: {
      lie();
      client.write(callback_lie_package,5);
    }
    break;

    case CMD_wavehello: {
      hello();
      client.write(callback_hello_package,5);
    }
    break;

    case CMD_pushpus: {
      pushup();
      client.write(callback_pushup_package,5);
    }
    break;

    case CMD_fighting: {
      fighting();
      client.write(callback_fighting_package,5);
    }
    break;

    case CMD_dance1: {
      dance1();
      client.write(callback_dance1_package,5);
    }
    break;

    case CMD_dance2: {
      dance2();
      client.write(callback_dance2_package,5);
    }
    break;

    case CMD_dance3: {
      dance3();
      client.write(callback_dance3_package,5);
    }
    break;
  }
}


// SETUP
void setup() {
  Serial.setTimeout(10);    // Set the serial timeout to 10ms
  Serial.begin(115200);     // Initiate serial communication with baud rate of 115200
  delay(100);

  WiFi.mode(WIFI_AP);           // Set Wi-Fi mode to access point mode
  WiFi.softAP(ssid,password,5); // Create a Wi-Fi access point, specify the network name and password, and set the channel to 5
  server.begin();               // Start the Wi-Fi server
  delay(100);

  servo_14.attach(14, SERVOMIN, SERVOMAX);  // Connect the servo to pin 14
  servo_12.attach(12, SERVOMIN, SERVOMAX);  // Connect the servo to pin 12
  servo_13.attach(13, SERVOMIN, SERVOMAX);  // Connect the servo to pin 13
  servo_15.attach(15, SERVOMIN, SERVOMAX);  // Connect the servo to pin 15
  servo_16.attach(16, SERVOMIN, SERVOMAX);  // Connect the servo to pin 16
  servo_5.attach(5, SERVOMIN, SERVOMAX);    // Connect the servo to pin 5
  servo_4.attach(4, SERVOMIN, SERVOMAX);    // Connect the servo to pin 4
  servo_2.attach(2, SERVOMIN, SERVOMAX);    // Connect the servo to pin 2

  Servo_PROGRAM_Zero();  // Reset the servo program to zero
}

// MAIN LOOP
void loop() {
  client = server.accept();
  if (client) {
    WA_en = true;
    ED_client = true;
    Serial.println("[Client connected]");
    unsigned long previousMillis = millis();  
    const unsigned long timeoutDuration = 3000;

    while (client.connected()) {
      if ((millis() - previousMillis) > timeoutDuration && client.available() == 0 && st == true) {
        break;
      }

      if (client.available()) {
        previousMillis = millis();
        unsigned char c = client.read() & 0xff;
        Serial.write(c);
        st = false;

        if (c == 200) {
          st = true;
        }

        if(c == 0x55 && isStart == false) {
          if(prevc == 0xff) {
            index_a = 1; 
            isStart = true;
          }
        }
        else {
          prevc = c;
          if(isStart) {
            if(index_a == 2) {
            dataLen = c; 
            }
            else if(index_a > 2) {
              dataLen--;
            }
            writeBuffer(index_a, c);
          }
        }

        index_a++;

        if(index_a > 120) {
          index_a = 0; 
          isStart = false;
        }

        if(isStart && dataLen == 0 && index_a > 3) { 
          isStart = false;
          parseData();
          index_a = 0;
        }
      }

      if (Serial.available()) {
        char c = Serial.read();
        sendBuff += c;
        client.print(sendBuff);
        Serial.print(sendBuff);
        sendBuff = "";
      }

      static unsigned long Test_time = 0;

      if (millis() - Test_time > 1000) {
        Test_time = millis();
        if (0 == (WiFi.softAPgetStationNum())) {
          break;
        }
      }
    }

    client.stop();
    Servo_PROGRAM_Zero();
    Serial.println("[Client disconnected]");
  }
  else {
    if (ED_client == true) {
      ED_client = false;
    }
  }
}
