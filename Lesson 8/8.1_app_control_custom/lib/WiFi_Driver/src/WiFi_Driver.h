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
    struct CommandData {  // Holds parsed command data extracted from incoming messages
      int action;         // Main command action (e.g., CMD_RUN, CMD_STANDBY)
      int device;         // Device identifier
      int movementType;   // Movement type (for CMD_RUN commands)
      bool isValid;       // Whether the command is valid
    };

    // Public methods
    void begin(const char* ssid, const char* password);
    CommandData handleClient();
    void sendData(byte* data, size_t len);
    bool isClientConnected();

  private:
    // Network objects
    WiFiServer server = WiFiServer(100);
    WiFiClient client;

    // Communication variables for parsing incoming data
    byte dataLength = 0;
    byte bufferIndex = 0;
    char receiveBuffer[52];
    unsigned char previousChar = 0;
    bool isStartReceiving = false;
    bool isStandbyTriggered = false;

    // Helper methods
    unsigned char readBuffer(int index);
    void writeBuffer(int index, unsigned char character);
    CommandData parseReceivedData();
};

#endif
