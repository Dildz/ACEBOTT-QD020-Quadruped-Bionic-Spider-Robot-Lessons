/*
 * WiFi_Driver.h - Manages the robot's Wi-Fi network and app connection
 *
 * INFRASTRUCTURE FILE — you don't need to change anything in here.
 * This file handles all the WiFi networking. The robot uses it automatically.
 *
 * What it does:
 * - Creates a WiFi access point (the network your phone connects to)
 * - Listens for the control app on port 100
 * - Reads and parses commands sent by the app
 * - Sends confirmation packets back to the app
 */


#ifndef WIFI_DRIVER_H
#define WIFI_DRIVER_H

// INCLUDES
#include <Arduino.h>
#include <ESP8266WiFi.h>

// CLASSES
class WiFiDriver {
  public:
    // This structure holds the command information we get from the app
    struct CommandData {  
      int action;         // What to do (e.g., move forward, dance)
      int device;         // Which device (for future use, like lights)
      int movementType;   // How to move (for movement commands)
      bool isValid;       // True if this is a real, complete command
    };

    // Public methods - these are the main functions you can use
    void begin(const char* ssid, const char* password);  // Start Wi-Fi
    CommandData handleClient();                          // Check for new commands
    void sendData(byte* data, size_t len);               // Send data back to app
    bool isClientConnected();                            // Check if TCP app client is connected
    bool isStationConnected();                           // Check if any device is on the WiFi AP

  private:
    // Network setup - server runs on port 100
    WiFiServer server = WiFiServer(100);
    WiFiClient client;

    // --- Protocol parser state ---
    // Packets from the app look like: 0xFF 0x55 <length> <body...>
    // feedByte() walks one byte at a time through these states.
    enum ParseState {
      WAIT_FF,    // looking for the first preamble byte 0xFF
      WAIT_55,    // saw 0xFF, looking for 0x55
      WAIT_LEN,   // saw preamble, next byte is body length
      READ_BODY,  // reading body bytes until done
    };

    static const size_t BODY_BUF_SIZE = 32;  // ample for legit ACEBOTT commands

    ParseState _parseState     = WAIT_FF;
    uint8_t    _body[BODY_BUF_SIZE];
    uint8_t    _bodyLen        = 0;     // total body bytes expected
    uint8_t    _bodyPos        = 0;     // body bytes received so far
    bool       _standbyTriggered = false;  // app sent the magic 0xC8 byte

    // Returns true if `b` just completed a packet (caller then calls buildCommand()).
    bool        feedByte(uint8_t b);
    CommandData buildCommand();
    void        resetParser();
};

#endif
