# Lesson 8.1 - App Control (Custom)

This project lets you control the robot from a phone app over WiFi. The robot creates its own WiFi network — you connect the app to it and send commands. When the robot walks, turns, or dances, its OLED eyes change expression to match. Everything runs smoothly without any `delay()` calls pausing the program.

---

## Files you'll care about

| File                                                    | What it does                                                                                  |
|---------------------------------------------------------|-----------------------------------------------------------------------------------------------|
| `src/main.cpp`                                          | Wires everything together. Handles commands from the app and calls into the 4 drivers.        |
| `lib/Movement_Driver/src/Movement_Driver.cpp`           | All the servo movement sequences. This is where walk, dance, and other movements are defined. |
| `lib/EyeAnimations_Driver/src/EyeAnimations_Driver.cpp` | Maps each movement to an eye expression. Change or add expressions here.                      |
| `lib/EyeAnimations_Driver/src/EyeAnimations_Driver.h`   | Defines the `EyeState` struct and the list of available moods, positions, and animations.     |

The other two libraries (`WiFi_Driver` and `Display_SSD1306`) are infrastructure — the robot uses them automatically and you don't need to touch them.

---

## Where to make changes

### Change the WiFi name or password
Open `src/main.cpp` and look near the top for:
```cpp
const char* ssid     = "QuadBot";
const char* password = "12345678";
```
Change those strings and re-upload.

### Change an eye expression
Open `lib/EyeAnimations_Driver/src/EyeAnimations_Driver.cpp` and find `applyStateExpression()`. Each `case` is a different movement. Change the `mood`, `position`, `autoblink`, or `customAnim` fields — there's a guide comment right above the function explaining all the options.

### Add a new movement sequence
Open `lib/Movement_Driver/src/Movement_Driver.cpp`. There's a guide comment block at the top of the movement arrays section explaining the format step by step. Copy an existing array, adjust the values, and follow the steps in the guide to register it.

---

## Storage movement warning

When the robot powers on and when a connected device disconnects from the WiFi, the robot automatically runs the storage sequence — folding its legs in to a compact resting position.

After the storage sequence duration expires (1 second), all servos are automatically detached so they can't hold torque against an obstacle. This prevents damage if a leg is blocked mid-fold.

Note that during that 1-second window the servos are still actively driving. Make sure all legs/paws are clear before powering on or disconnecting. When a device reconnects and standby is triggered, the servos re-attach automatically.

---

## Why no delay()?

Most simple Arduino sketches use `delay(ms)` to wait between steps. The problem is that `delay()` freezes *everything* — the robot can't check for new commands, update the display, or respond to anything while it's waiting.

This project uses a different approach: instead of waiting, it checks the clock (`millis()`) every loop to see if enough time has passed. The robot moves one step at a time, only advancing when the timer says it's ready. Everything else keeps running in between.

That's what `robot.update()` and `display.update()` do every loop — they each check "is it time for my next step?" and act if so. This keeps the robot responsive and the eye animations smooth while movements are happening.
