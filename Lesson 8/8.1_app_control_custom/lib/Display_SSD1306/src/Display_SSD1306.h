/*
 * Display_SSD1306.h - Owns and controls the SSD1306 OLED display
 *
 * This is the only library that touches the display hardware.
 * It owns both the Adafruit_SSD1306 driver and the RoboEyes animation engine.
 *
 * RESPONSIBILITIES:
 * - Hardware init and startup screens (begin)
 * - Eye animation initialisation when a WiFi client connects (beginEyes)
 * - Applying eye expression states from EyeAnimations_Driver (applyEyeState)
 * - Driving the RoboEyes render loop each frame (update)
 *
 * NOTE: EyeAnimations_Driver.h is included only for the EyeState struct.
 *       DisplayDriver has no knowledge of the EyeAnimations class itself.
 */

#ifndef DISPLAY_SSD1306_H
#define DISPLAY_SSD1306_H

#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include "EyeAnimations_Driver.h"  // for EyeState struct

// Forward declaration - RoboEyes is a header-only template library.
// Including it only in the .cpp prevents BGCOLOR/MAINCOLOR from being defined in multiple translation units (which would cause a linker error).
template<typename T> class RoboEyes;

class DisplayDriver {
public:
  // Constructor - matches Adafruit_SSD1306 parameters
  DisplayDriver(int width, int height, TwoWire *wire, int resetPin, uint8_t i2cAddress = 0x3C);  // < See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

  // Store WiFi credentials - call before begin()
  void setWiFiCredentials(const char* ssid, const char* password);

  // Initialise display hardware and show startup screens - call once in setup()
  bool begin();

  // Initialise RoboEyes - call when first WiFi client connects
  void beginEyes();

  // Stop RoboEyes and re-show the WiFi credentials screen - call on client disconnect
  void showWiFiInfo();

  // Apply eye expression from EyeAnimations_Driver - call when eyes.hasStateChanged()
  void applyEyeState(EyeState state);

  // Render the current eye animation frame - call every loop()
  void update();

private:
  Adafruit_SSD1306              _display;
  RoboEyes<Adafruit_SSD1306>*   _roboEyes;
  bool                          _eyesActive;
  uint8_t                       _i2cAddress;
  const char*                   _ssid;
  const char*                   _password;

  // Custom bitmap animation playback
  uint8_t       _customAnimId;    // active CUSTOM_ANIM_* id, 0 = RoboEyes mode
  uint16_t      _customFrame;     // current frame index
  unsigned long _customNextFrame; // millis() timestamp for next frame advance

  void showConnectionScreen();    // renders the WiFi credentials screen
};

#endif
