/*
 * Hello World with LED blink:
 * Prints "Hello World!" to the serial monitor, turns on an LED on for one second & then off for one second, repeatedly.
 */

 
// INCLUDES
#include "Arduino.h"


// SETUP
void setup() {
  Serial.begin(115200);                   // initialize serial communication at 115200 board rate
  delay(100);                             // wait for 100 milliseconds

  pinMode(LED_BUILTIN, OUTPUT);           // initialize LED digital pin as an output.
  Serial.println("");                     // blank line
  Serial.println("Setup completed\n");    // '\n' makes a new line
}

// MAIN LOOP
void loop() {
  Serial.println("Hello, world!");    // print to serial monitor

  digitalWrite(LED_BUILTIN, LOW);     // turn the LED on by making the voltage LOW (low = on)
  delay(1000);                        // wait for a second
  digitalWrite(LED_BUILTIN, HIGH);    // turn the LED off by making the voltage HIGH (HIGH = off)
  delay(1000);                        // wait for a second
}


/*
 * LESSONS LEARNED:
 * - The ESP8266 board has a built-in LED that can be controlled using the LED_BUILTIN constant.
 * - pinMode() sets the digital pin as an output.
 * 
 * - setup() function runs once when the program starts.
 * - Serial.begin() initializes the serial communication at a specified board rate.
 * - Serial.println() prints a message to the serial monitor. The '\n' character is used to make a new line.
 * 
 * - loop() function runs repeatedly as long as the board is powered on.
 * - digitalWrite() writes a HIGH or LOW value to a digital pin. On the ESP8266 board, LOW turns the LED on and HIGH turns it off.
 * - delay() function pauses the program for a specified number of milliseconds.
 */
