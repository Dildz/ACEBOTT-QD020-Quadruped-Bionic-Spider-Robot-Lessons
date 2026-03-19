/*
 * EyeAnimations_Driver.h - Maps robot movement states to eye expressions
 *
 * This library is pure logic - it has no knowledge of the display hardware.
 * It decides WHAT the eyes should look like based on what the robot is doing,
 * and stores that as an EyeState struct. main.cpp reads the state and passes
 * it to DisplayDriver to actually render it.
 *
 * DATA FLOW:
 *   MovementDriver (what robot is doing)
 *       --> main.cpp calls eyes.setMovementState()
 *       --> EyeAnimations maps movement to EyeState
 *       --> main.cpp reads eyes.getState() and calls display.applyEyeState()
 *       --> DisplayDriver renders via RoboEyes
 */

#ifndef EYEANIMATIONS_DRIVER_H
#define EYEANIMATIONS_DRIVER_H

#include "Movement_Driver.h"

// Eye mood constants - values match FluxGarage RoboEyes library
#define EYES_DEFAULT  0
#define EYES_TIRED    1
#define EYES_ANGRY    2
#define EYES_HAPPY    3

// Custom bitmap animation IDs — used when a movement state replaces RoboEyes
// with a pre-converted GIF animation. 0 = use RoboEyes as normal.
#define CUSTOM_ANIM_NONE     0
#define CUSTOM_ANIM_SAD      1  // LIE_DOWN
#define CUSTOM_ANIM_SLEEP    2  // SLEEP
#define CUSTOM_ANIM_LOVE     3  // WAVE_HELLO, DANCE2
#define CUSTOM_ANIM_CURIOUS  4  // PUSH_UPS, DANCE3
#define CUSTOM_ANIM_ANGRY    5  // FIGHTING
#define CUSTOM_ANIM_WOW      6  // DANCE1

// Eye position constants - values match FluxGarage RoboEyes library
#define EYES_CENTER  0  // middle center
#define EYES_N       1  // north, top center
#define EYES_E       3  // east, middle right
#define EYES_S       5  // south, bottom center
#define EYES_W       7  // west, middle left

/*
 * EyeState - describes the desired eye animation at any moment.
 * Passed from EyeAnimations_Driver to DisplayDriver via main.cpp.
 */
struct EyeState {
  uint8_t mood       = EYES_DEFAULT;   // expression: DEFAULT, TIRED, ANGRY, HAPPY
  uint8_t position   = EYES_CENTER;    // gaze direction: CENTER, N, E, S, W
  bool    autoblink  = false;          // automatic blinking
  bool    curious    = false;          // outer eye grows when looking sideways
  bool    sweat      = false;          // sweat drop effect
  bool    closed     = false;          // eyes fully closed (sleep)
  bool    triggerLaugh = false;        // one-shot laugh animation
  uint8_t customAnim = CUSTOM_ANIM_NONE; // non-zero: play bitmap anim, skip RoboEyes
};

class EyeAnimations {
public:
  EyeAnimations();

  // Activate eyes on first WiFi client connection
  void begin();

  // Set movement state - computes new EyeState internally
  void setMovementState(MovementState state);

  // Called by main.cpp to check if expression needs updating
  EyeState getState() const;
  bool hasStateChanged() const;
  void acknowledgeChange();  // clears the changed flag after display has applied it

  // Called every loop() - drives timed idle roaming behaviour
  void update();

private:
  // Idle roaming phases: blink at centre, then look somewhere briefly, repeat
  enum RoamPhase { ROAM_CENTER, ROAM_LOOKING };

  EyeState      _currentState;
  MovementState _lastState;
  bool          _active;
  bool          _stateChanged;
  unsigned long _nextRoamTime;  // millis() timestamp for next roam phase transition
  RoamPhase     _roamPhase;     // which phase of the roam cycle we're in

  void applyStateExpression(MovementState state);
};

#endif
