/*
 * Display_SSD1306.cpp - SSD1306 display driver implementation
 *
 * Handles all display hardware and eye animation rendering.
 * FluxGarage_RoboEyes.h is included HERE only - not in the header.
 * This prevents its global variables (BGCOLOR, MAINCOLOR) from being
 * defined more than once across translation units.
 *
 * I2C PINS :
 * - using serial pins - do not use serial monitoring when display is connected
 * - SDA = GPIO3 (RxD0 on NodeMCU)
 * - SCL = GPIO1 (TxD0 on NodeMCU)
 * - Speed: 400kHz for faster screen updates
 *
 * DISPLAY CONFIGURATION:
 * - Rotation: 2 (180 degrees) - adjust if display is mounted differently
 * - Address:  0x3D for 128x64, 0x3C for 128x32
 */

#include "Display_SSD1306.h"
#include <FluxGarage_RoboEyes.h>

// Custom bitmap animation headers — included only here to keep all frame data
// in one translation unit and avoid multiple-definition issues.
#include "converted_gifs/eyes_sad.h"
#include "converted_gifs/eyes_sleep.h"
#include "converted_gifs/eyes_love.h"
#include "converted_gifs/eyes_curious.h"
#include "converted_gifs/eyes_angry.h"
#include "converted_gifs/eyes_wow.h"

// Display dimensions for drawBitmap — defined here, not in the animation headers.
#define ANIM_W 128
#define ANIM_H  64

// Lookup table mapping CUSTOM_ANIM_* ids → frame/delay arrays.
// Index 0 is unused (CUSTOM_ANIM_NONE = RoboEyes mode).
struct AnimDef {
  const uint8_t* const* frames;  // PROGMEM pointer array
  const uint16_t*       delays;  // PROGMEM delay array (ms per frame)
  uint8_t               count;
};

static const AnimDef customAnims[] = {
  { nullptr,        nullptr,         0                        },  // 0 = NONE
  { sad_frames,     sad_delays,      ANIM_SAD_FRAME_COUNT     },  // 1 = SAD
  { sleep_frames,   sleep_delays,    ANIM_SLEEP_FRAME_COUNT   },  // 2 = SLEEP
  { love_frames,    love_delays,     ANIM_LOVE_FRAME_COUNT    },  // 3 = LOVE
  { curious_frames, curious_delays,  ANIM_CURIOUS_FRAME_COUNT },  // 4 = CURIOUS
  { angry_frames,   angry_delays,    ANIM_ANGRY_FRAME_COUNT   },  // 5 = ANGRY
  { wow_frames,     wow_delays,      ANIM_WOW_FRAME_COUNT     },  // 6 = WOW
};

#define I2C_SDA_PIN     3
#define I2C_SCL_PIN     1
#define I2C_CLOCK_SPEED 400000  // 400kHz

// Constructor
DisplayDriver::DisplayDriver(int width, int height, TwoWire *wire, int resetPin, uint8_t i2cAddress)
  : _display(width, height, wire, resetPin),
    _roboEyes(nullptr),
    _eyesActive(false),
    _i2cAddress(i2cAddress),
    _ssid(nullptr),
    _password(nullptr),
    _customAnimId(0),
    _customFrame(0),
    _customNextFrame(0) {
}

// Render the WiFi credentials screen
void DisplayDriver::showConnectionScreen() {
  _display.clearDisplay();
  _display.setTextSize(1);
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(0, 0);  _display.println("WiFi Connection Info");
  _display.setCursor(0, 15); _display.println("SSID:");
  _display.setCursor(0, 25); _display.println(_ssid);
  _display.setCursor(0, 40); _display.println("Password:");
  _display.setCursor(0, 50); _display.println(_password);
  _display.display();
}

// Store WiFi credentials - must be called before begin()
void DisplayDriver::setWiFiCredentials(const char* ssid, const char* password) {
  _ssid     = ssid;
  _password = password;
}

// Initialise display hardware and show startup screens
bool DisplayDriver::begin() {
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  Wire.setClock(I2C_CLOCK_SPEED);

  _display.setRotation(0);  // 0=0°, 1=90°, 2=180°, 3=270°

  bool success = _display.begin(SSD1306_SWITCHCAPVCC, _i2cAddress);
  if (success) {
    // Show Adafruit splash screen (built into library)
    _display.display();
    delay(2000);

    // Show robot startup message
    _display.clearDisplay();
    _display.setTextSize(1);
    _display.setTextColor(SSD1306_WHITE);
    _display.setCursor(0, 10); _display.println("ACEBOTT Quadroped");
    _display.setCursor(0, 30); _display.println("Robot is AWAKE and");
    _display.setCursor(0, 50); _display.println("READY for ACTION!");
    _display.display();
    delay(5000);

    // Show WiFi credentials so the user can connect the app
    showConnectionScreen();
  }
  return success;
}

// Initialise RoboEyes when WiFi client connects
void DisplayDriver::beginEyes() {
  if (_eyesActive) return;
  _eyesActive = true;

  _roboEyes = new RoboEyes<Adafruit_SSD1306>(_display);
  _roboEyes->begin(128, 64, 50);  // screen width, height, target framerate (fps)
  _roboEyes->open();              // eyes animate open from closed - no blocking delay needed
}

// Stop eye animations and re-show WiFi credentials when client disconnects
void DisplayDriver::showWiFiInfo() {
  if (_roboEyes) {
    delete _roboEyes;
    _roboEyes = nullptr;
  }
  _eyesActive = false;
  showConnectionScreen();
}

// Apply an eye expression state - called by main.cpp when eyes.hasStateChanged()
void DisplayDriver::applyEyeState(EyeState state) {
  if (!_eyesActive) return;

  if (state.customAnim != CUSTOM_ANIM_NONE) {
    // Switching to (or staying in) custom bitmap animation mode.
    // Reset to frame 0 only when the animation changes.
    if (state.customAnim != _customAnimId) {
      _customAnimId   = state.customAnim;
      _customFrame    = 0;
      _customNextFrame = millis();  // start immediately
    }
    return;  // RoboEyes settings are irrelevant while in custom mode
  }

  // Switching back to RoboEyes mode
  _customAnimId = CUSTOM_ANIM_NONE;
  if (!_roboEyes) return;

  _roboEyes->setMood(state.mood);
  _roboEyes->setPosition(state.position);
  _roboEyes->setAutoblinker(state.autoblink);
  _roboEyes->setCuriosity(state.curious);
  _roboEyes->setSweat(state.sweat);

  if (state.closed) {
    _roboEyes->close();
  } else {
    _roboEyes->open();
  }

  if (state.triggerLaugh) {
    _roboEyes->anim_laugh();  // one-shot animation, self-resets when done
  }
}

// Render the current eye animation frame - call every loop()
void DisplayDriver::update() {
  if (!_eyesActive) return;

  if (_customAnimId != CUSTOM_ANIM_NONE) {
    // Custom bitmap animation: advance frames using per-frame delay timing
    unsigned long now = millis();
    if (now < _customNextFrame) return;

    const AnimDef& anim = customAnims[_customAnimId];
    const uint8_t* frame = (const uint8_t*) pgm_read_ptr(&anim.frames[_customFrame]);
    uint16_t delay_ms    = pgm_read_word(&anim.delays[_customFrame]);

    _display.clearDisplay();
    _display.drawBitmap(0, 0, frame, ANIM_W, ANIM_H, SSD1306_WHITE);
    _display.display();

    _customFrame     = (_customFrame + 1) % anim.count;  // loop continuously
    _customNextFrame = now + delay_ms;

  } else if (_roboEyes) {
    _roboEyes->update();
  }
}
