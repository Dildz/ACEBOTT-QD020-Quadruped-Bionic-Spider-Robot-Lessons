/*
 * WiFi_Driver.cpp - Implementation of the WiFiDriver library
 * 
 * This file manages Wi-Fi AP setup, client connection handling,
 * and protocol parsing for incoming command data.
 * 
 * IMPLEMENTATION:
 * - begin(): Initializes Wi-Fi in AP mode with specified credentials
 * - handleClient(): Main loop for client connection management and data parsing
 * - parseReceivedData(): Extracts command data from received protocol packets
 * - sendData(): Sends data back to connected client
 * - isClientConnected(): Checks if client is still connected
 * 
 * PROTOCOL PARSING:
 * - Looks for 0xFF 0x55 preamble to start receiving
 * - Second byte indicates total data length
 * - Times out after 3 seconds of inactivity
 * - Automatically returns to standby if client disconnects
 */


// INCLUDES
#include "WiFi_Driver.h"

// HELPER METHODS
unsigned char WiFiDriver::readBuffer(int index) {
  return receiveBuffer[index];
}

void WiFiDriver::writeBuffer(int index, unsigned char character) {
  receiveBuffer[index] = character;
}

WiFiDriver::CommandData WiFiDriver::parseReceivedData() {
  isStartReceiving = false;
  CommandData cmd;
  cmd.isValid = true;
  
  // Extract the important information from the message
  cmd.action = readBuffer(9);        // What action to perform
  cmd.device = readBuffer(10);       // Which device to control
  
  // For movement commands, get the specific movement type
  if (cmd.action == 1) { // CMD_RUN - movement command
    cmd.movementType = readBuffer(12);
    
    // Show what we received in the Serial Monitor
    Serial.print("Movement Command: Action 0x");
    Serial.print(cmd.action, HEX);
    Serial.print(", Device 0x");
    Serial.print(cmd.device, HEX);
    Serial.print(", MovementType 0x");
    if (cmd.movementType < 0x10) Serial.print("0"); // Add leading zero for formatting
    Serial.println(cmd.movementType, HEX);
  }
  else {
    cmd.movementType = 0; // Not a movement command
    
    // Show what we received in the Serial Monitor
    Serial.print("Action Command: Action 0x");
    Serial.print(cmd.action, HEX);
    Serial.print(", Device 0x");
    if (cmd.device < 0x10) Serial.print("0"); // Add leading zero for formatting
    Serial.println(cmd.device, HEX);
  }
  
  return cmd;
}

// PUBLIC METHODS
void WiFiDriver::begin(const char* ssid, const char* password) {
  Serial.println("\nInitializing Wi-Fi...");
  
  // Set up as Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 5);
  server.begin();
  delay(100);
  
  // Show connection information
  Serial.println("Wi-Fi AP ready and server started.");
  Serial.print("Connect to SSID: ");
  Serial.print(ssid);
  Serial.print(" with password: ");
  Serial.print(password);
  Serial.println(" to control the robot.");
}

WiFiDriver::CommandData WiFiDriver::handleClient() {
  CommandData cmd;
  cmd.isValid = false;

  // Check for new client connection
  if (!client || !client.connected()) {
    client = server.accept();
    if (client) {
      Serial.println("[Client connected]");
      // Reset state for new client
      bufferIndex = 0;
      isStartReceiving = false;
      dataLength = 0;
      previousChar = 0;
      isStandbyTriggered = false;
    }
  }

  // Process incoming data from connected client
  if (client && client.connected()) {
    unsigned long previousMillis = millis();
    const unsigned long timeoutDuration = 3000; // 3 second timeout

    while (client.available()) {
      previousMillis = millis();
      unsigned char receivedChar = client.read() & 0xff;
      
      isStandbyTriggered = false;
      if (receivedChar == 200) {
        isStandbyTriggered = true;
      }

      // Look for the special start sequence: 0xFF 0x55
      if (receivedChar == 0x55 && !isStartReceiving) {
        if (previousChar == 0xff) {
          bufferIndex = 1;
          isStartReceiving = true;
        }
      }
      else {
        previousChar = receivedChar;
        if (isStartReceiving) {
          if (bufferIndex == 2) {
            dataLength = receivedChar; // Second byte tells us how long the message is
          }
          else if (bufferIndex > 2) {
            dataLength--;
          }
          writeBuffer(bufferIndex, receivedChar); // Store the data
        }
      }

      bufferIndex++;

      // Prevent buffer overflow
      if (bufferIndex > 120) {
        bufferIndex = 0;
        isStartReceiving = false;
      }

      // If we received a complete message, figure out what it means
      if (isStartReceiving && dataLength == 0 && bufferIndex > 3) {
        cmd = parseReceivedData();
        isStartReceiving = false;
        bufferIndex = 0;
        return cmd;
      }
    }

    // If we don't hear from the client for 3 seconds with a standby flag, go to standby
    if ((millis() - previousMillis) > timeoutDuration && client.available() == 0 && isStandbyTriggered == true) {
      client.stop();
      cmd.action = 3; // CMD_STANDBY
      cmd.isValid = true;
      return cmd;
    }

    // If the client disconnects from Wi-Fi, go to standby
    if (WiFi.softAPgetStationNum() == 0) {
      client.stop();
      cmd.action = 3; // CMD_STANDBY
      cmd.isValid = true;
      return cmd;
    }
  }

  return cmd;
}

void WiFiDriver::sendData(byte* data, size_t len) {
  if (client && client.connected()) {
    client.write(data, len);
  }
}

bool WiFiDriver::isClientConnected() {
  return client && client.connected();
}
