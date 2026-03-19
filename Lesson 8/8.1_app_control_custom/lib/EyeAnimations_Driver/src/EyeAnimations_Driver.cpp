/*
 * EyeAnimations_Driver.cpp - Movement to eye expression mapping
 *
 * Translates robot movement states into EyeState values.
 * No display or RoboEyes code lives here - this is pure logic.
 *
 * ADDING NEW EXPRESSIONS:
 * - Add a case to applyStateExpression() and set the EyeState fields.
 * - Mood:     EYES_DEFAULT, EYES_TIRED, EYES_ANGRY, EYES_HAPPY
 * - Position: EYES_CENTER, EYES_N, EYES_E, EYES_S, EYES_W
 */

#include "EyeAnimations_Driver.h"

EyeAnimations::EyeAnimations()
  : _lastState(IDLE), _active(false), _stateChanged(false),
    _nextRoamTime(0), _roamPhase(ROAM_CENTER) {
  // _currentState initialises to EyeState defaults
}

// Activate on first WiFi client connection
void EyeAnimations::begin() {
  if (_active) return;
  _active = true;
  _lastState = IDLE;
  _roamPhase    = ROAM_CENTER;
  _nextRoamTime = millis() + 3000;  // blink at centre for 3s before first look
  applyStateExpression(STANDBY);
  _stateChanged = true;  // signal display to apply initial state
}

// --- State query methods (called by main.cpp) ---

EyeState EyeAnimations::getState() const {
  return _currentState;
}

bool EyeAnimations::hasStateChanged() const {
  return _stateChanged;
}

void EyeAnimations::acknowledgeChange() {
  _stateChanged = false;
}

// --- Movement state update ---

void EyeAnimations::setMovementState(MovementState state) {
  if (!_active) return;
  if (state == _lastState) return;  // no change, nothing to do

  bool wasIdle = (_lastState == STANDBY || _lastState == READY || _lastState == IDLE);
  bool isIdle  = (state == STANDBY || state == READY || state == IDLE);

  _lastState = state;
  applyStateExpression(state);
  _stateChanged = true;

  // When returning to idle from a movement state, restart roam from centre
  if (isIdle && !wasIdle) {
    _roamPhase    = ROAM_CENTER;
    _nextRoamTime = millis() + 3000;
  }
}

// --- Idle roaming update (call every loop()) ---

// Positions to look at — centre is excluded so the eyes always visibly move
static const uint8_t lookPositions[] = { EYES_N, EYES_E, EYES_S, EYES_W };

void EyeAnimations::update() {
  if (!_active) return;

  // Only roam during idle states
  bool isIdle = (_lastState == STANDBY || _lastState == READY || _lastState == IDLE);
  if (!isIdle) return;

  unsigned long now = millis();
  if (now < _nextRoamTime) return;

  if (_roamPhase == ROAM_CENTER) {
    // Leave centre: pick a random direction, disable blinking while looking
    uint8_t pos            = lookPositions[random(4)];
    _currentState.position = pos;
    _currentState.autoblink = false;
    _currentState.curious  = (pos == EYES_E || pos == EYES_W);
    _stateChanged = true;
    _roamPhase    = ROAM_LOOKING;
    _nextRoamTime = now + 1000 + random(1001);  // hold look for 1–2 seconds

  } else {  // ROAM_LOOKING
    // Return to centre: re-enable blinking, wait 5–10s before next look
    _currentState.position  = EYES_CENTER;
    _currentState.autoblink = true;
    _currentState.curious   = false;
    _stateChanged = true;
    _roamPhase    = ROAM_CENTER;
    _nextRoamTime = now + 5000 + random(5001);  // blink at centre for 5–10 seconds
  }
}

// --- Expression mapping table ---

void EyeAnimations::applyStateExpression(MovementState state) {
  _currentState = EyeState();  // reset all fields to defaults first

  switch (state) {

    // Idle / resting - default look, gentle blinking
    case STANDBY:
    case READY:
    case IDLE:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.autoblink = true;
      break;

    // Walking forward - look up ahead, curious
    case FORWARD:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_N;
      _currentState.autoblink = true;
      _currentState.curious   = true;
      break;

    // Walking backward - look down
    case BACKWARD:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_S;
      _currentState.autoblink = true;
      break;

    // Turning - look in direction of turn, curious
    case TURN_LEFT:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_W;
      _currentState.autoblink = true;
      _currentState.curious   = true;
      break;

    case TURN_RIGHT:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_E;
      _currentState.autoblink = true;
      _currentState.curious   = true;
      break;

    // Stepping sideways - look in direction of movement
    case MOVE_LEFT:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_W;
      _currentState.autoblink = true;
      break;

    case MOVE_RIGHT:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.position  = EYES_E;
      _currentState.autoblink = true;
      break;

    // Waving hello - love animation
    case WAVE_HELLO:
      _currentState.customAnim = CUSTOM_ANIM_LOVE;
      break;

    // Dancing - each dance uses a different custom animation
    case DANCE1:
      _currentState.customAnim = CUSTOM_ANIM_WOW;
      break;

    case DANCE2:
      _currentState.customAnim = CUSTOM_ANIM_LOVE;
      break;

    case DANCE3:
      _currentState.customAnim = CUSTOM_ANIM_CURIOUS;
      break;

    // Lying down - sad animation
    case LIE_DOWN:
      _currentState.customAnim = CUSTOM_ANIM_SAD;
      break;

    // Sleeping - sleep animation
    case SLEEP:
      _currentState.customAnim = CUSTOM_ANIM_SLEEP;
      break;

    // Fighting - angry animation
    case FIGHTING:
      _currentState.customAnim = CUSTOM_ANIM_ANGRY;
      break;

    // Push-ups - curious animation
    case PUSH_UPS:
      _currentState.customAnim = CUSTOM_ANIM_CURIOUS;
      break;

    default:
      _currentState.mood      = EYES_DEFAULT;
      _currentState.autoblink = true;
      break;
  }
}
