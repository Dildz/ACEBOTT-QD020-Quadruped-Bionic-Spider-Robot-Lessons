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

    // Variables for reading and understanding incoming data
    byte dataLength = 0;                // How long the message should be
    byte bufferIndex = 0;               // Where we are in the message
    char receiveBuffer[52];             // Where we store incoming data
    unsigned char previousChar = 0;     // Remember previous character
    bool isStartReceiving = false;      // True when we find start of message
    bool isStandbyTriggered = false;    // True if standby command received

    // Helper methods (used internally)
    unsigned char readBuffer(int index);
    void writeBuffer(int index, unsigned char character);
    CommandData parseReceivedData();
};

#endif
