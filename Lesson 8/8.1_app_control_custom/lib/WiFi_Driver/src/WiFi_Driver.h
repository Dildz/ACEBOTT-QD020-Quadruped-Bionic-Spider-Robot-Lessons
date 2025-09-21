/*
 * WiFi_Driver.h - Custom library for managing ESP8266 Wi-Fi Access Point and client communication
 * 
 * This library provides an interface for setting up the robot as a Wi-Fi AP,
 * handling client connections, and parsing incoming command data.
 * 
 * IMPLEMENTATION:
 * - Sets up ESP8266 as Access Point with configurable SSID/password
 * - Listens for client connections on port 100
 * - Parses incoming data using a custom protocol format
 * - Extracts command data (action, device, movement type)
 * - Handles client timeouts and disconnections gracefully
 * 
 * PROTOCOL FORMAT:
 * - Commands start with 0xFF 0x55 preamble
 * - Second byte indicates data length
 * - Subsequent bytes contain command data:
 *   - Byte 9: Action (e.g., CMD_RUN, CMD_STANDBY)
 *   - Byte 10: Device identifier
 *   - Byte 12: Movement type (for CMD_RUN commands)
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
    bool isClientConnected();                            // Check if app is connected

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
