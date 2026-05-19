/*
 * WiFi_Driver.cpp - WiFi AP setup and ACEBOTT app protocol parser
 *
 * INFRASTRUCTURE FILE — you don't need to change anything in here.
 * Starts the WiFi access point, accepts the app as a TCP client, and parses
 * its wire protocol one byte at a time using a small state machine.
 *
 * WIRE PROTOCOL:
 *   [0xFF] [0x55] [length] [body...]
 *     where length = number of body bytes that follow.
 *
 * Body field positions (offsets WITHIN the body, after the header):
 *   body[6]  = action  (CMD_* number)
 *   body[7]  = device
 *   body[9]  = movement type  (only meaningful when action == CMD_RUN)
 *
 * One special non-packet byte triggers an auto-standby on idle timeout:
 *   0xC8 anywhere in the stream arms _standbyTriggered. If the client then
 *   goes quiet for 3 seconds, we synthesise a standby command.
 */

#include "WiFi_Driver.h"


// Action codes the parser needs to know about. These mirror the CMD_*
// numbers in main.cpp; duplicated here to keep this file self-contained.
static const uint8_t ACTION_RUN     = 1;
static const uint8_t ACTION_STANDBY = 3;  // synthesised on idle timeout / disconnect

// Special non-packet marker byte. When the stream goes quiet and we last
// saw this byte, we synthesise a standby command (legacy ACEBOTT behaviour).
static const uint8_t STANDBY_TRIGGER_BYTE = 0xC8;  // 200 decimal

// Body field offsets - positions WITHIN the body (after the 0xFF 0x55 LEN header).
static const size_t BODY_OFFSET_ACTION   = 6;
static const size_t BODY_OFFSET_DEVICE   = 7;
static const size_t BODY_OFFSET_MOVEMENT = 9;

// How long the client can go quiet before we treat them as gone.
static const unsigned long CLIENT_TIMEOUT_MS = 3000;


// --- PARSER ---

void WiFiDriver::resetParser() {
  _parseState = WAIT_FF;
  _bodyLen    = 0;
  _bodyPos    = 0;
}

bool WiFiDriver::feedByte(uint8_t b) {
  // Side-channel: any 0xC8 byte in the stream arms the auto-standby behaviour
  // that fires on the next idle timeout. Any other byte disarms it.
  _standbyTriggered = (b == STANDBY_TRIGGER_BYTE);

  switch (_parseState) {
    case WAIT_FF:
      if (b == 0xFF) _parseState = WAIT_55;
      return false;

    case WAIT_55:
      if (b == 0x55) {
        _parseState = WAIT_LEN;
      } else if (b != 0xFF) {
        // Anything else aborts the preamble. Another 0xFF keeps us here.
        _parseState = WAIT_FF;
      }
      return false;

    case WAIT_LEN:
      _bodyLen = b;
      _bodyPos = 0;
      // Refuse a length we can't store (also handles length = 0).
      if (_bodyLen == 0 || _bodyLen > BODY_BUF_SIZE) {
        resetParser();
        return false;
      }
      _parseState = READ_BODY;
      return false;

    case READ_BODY:
      _body[_bodyPos++] = b;
      if (_bodyPos >= _bodyLen) {
        // Full body received — caller can pull the command out now.
        _parseState = WAIT_FF;
        return true;
      }
      return false;
  }
  return false;  // unreachable
}

WiFiDriver::CommandData WiFiDriver::buildCommand() {
  CommandData cmd;
  cmd.isValid = true;
  cmd.action  = (_bodyPos > BODY_OFFSET_ACTION) ? _body[BODY_OFFSET_ACTION] : 0;
  cmd.device  = (_bodyPos > BODY_OFFSET_DEVICE) ? _body[BODY_OFFSET_DEVICE] : 0;
  cmd.movementType =
      (cmd.action == ACTION_RUN && _bodyPos > BODY_OFFSET_MOVEMENT)
          ? _body[BODY_OFFSET_MOVEMENT]
          : 0;
  return cmd;
}


// --- PUBLIC METHODS ---

void WiFiDriver::begin(const char* ssid, const char* password) {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password, 5);
  server.begin();
  delay(100);
}

WiFiDriver::CommandData WiFiDriver::handleClient() {
  CommandData cmd;
  cmd.isValid = false;

  // Accept a new client if none is currently connected.
  if (!client || !client.connected()) {
    client = server.accept();
    if (client) {
      resetParser();
      _standbyTriggered = false;
    }
  }

  if (client && client.connected()) {
    unsigned long lastActivity = millis();

    // Drain whatever bytes are queued right now.
    while (client.available()) {
      lastActivity = millis();
      uint8_t b = client.read() & 0xff;

      if (feedByte(b)) {
        // Complete packet — return its command and stop draining.
        return buildCommand();
      }
    }

    // No complete packet this loop. If the client has gone quiet AND the
    // last byte we saw was the standby-trigger marker, synthesise standby.
    if ((millis() - lastActivity) > CLIENT_TIMEOUT_MS
        && client.available() == 0
        && _standbyTriggered) {
      client.stop();
      cmd.action  = ACTION_STANDBY;
      cmd.isValid = true;
      return cmd;
    }

    // If the AP has no stations left, the phone has dropped off entirely.
    if (WiFi.softAPgetStationNum() == 0) {
      client.stop();
      cmd.action  = ACTION_STANDBY;
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

bool WiFiDriver::isStationConnected() {
  return WiFi.softAPgetStationNum() > 0;
}
