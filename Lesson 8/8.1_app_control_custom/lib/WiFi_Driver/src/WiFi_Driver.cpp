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
  
  // Extract command data from buffer
  cmd.action = readBuffer(9);
  cmd.device = readBuffer(10);
  
  // For movement commands, extract the movement type
  if (cmd.action == 1) { // CMD_RUN
    cmd.movementType = readBuffer(12);
    
    // Debug output for movement commands using Serial.print only
    Serial.print("Received Movement Command: Action 0x");
    Serial.print(cmd.action, HEX);
    Serial.print(", Device 0x");
    Serial.print(cmd.device, HEX);
    Serial.print(", MovementType 0x");
    if (cmd.movementType < 0x10) Serial.print("0"); // Pad with leading zero
    Serial.println(cmd.movementType, HEX);
  }
  else {
    cmd.movementType = 0;
    
    // Debug output for action commands using Serial.print only
    Serial.print("Received Action Command: Action 0x");
    Serial.print(cmd.action, HEX);
    Serial.print(", Device 0x");
    if (cmd.device < 0x10) Serial.print("0"); // Pad with leading zero
    Serial.println(cmd.device, HEX);
  }
  
  return cmd;
}

// PUBLIC METHODS
void WiFiDriver::begin(const char* ssid, const char* password) {
  Serial.println("\nInitializing Wi-Fi...");
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 5);
  server.begin();
  delay(100);

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
    const unsigned long timeoutDuration = 3000;

    while (client.available()) {
      previousMillis = millis();
      unsigned char receivedChar = client.read() & 0xff;
      
      isStandbyTriggered = false;
      if (receivedChar == 200) {
        isStandbyTriggered = true;
      }

      // Protocol parsing
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
            dataLength = receivedChar;
          }
          else if (bufferIndex > 2) {
            dataLength--;
          }
          writeBuffer(bufferIndex, receivedChar);
        }
      }

      bufferIndex++;

      if (bufferIndex > 120) {
        bufferIndex = 0;
        isStartReceiving = false;
      }

      if (isStartReceiving && dataLength == 0 && bufferIndex > 3) {
        cmd = parseReceivedData();
        isStartReceiving = false;
        bufferIndex = 0;
        return cmd;
      }
    }

    // Handle timeout
    if ((millis() - previousMillis) > timeoutDuration && client.available() == 0 && isStandbyTriggered == true) {
      client.stop();
      cmd.action = 3; // CMD_STANDBY
      cmd.isValid = true;
      return cmd;
    }

    // Check if client is still associated with AP
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