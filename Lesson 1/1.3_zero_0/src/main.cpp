/*
 * This function is used to zero the robot's leg servos to specific angles during the assembly process.
 * 
 * SERVO PIN REFERENCE:
 * - PIN D0  = GPIO16
 * - PIN D1  = GPIO5
 * - PIN D2  = GPIO4
 * - PIN D4  = GPIO2
 * - PIN D5  = GPIO14
 * - PIN D6  = GPIO12
 * - PIN D7  = GPIO13
 * - PIN D8  = GPIO15
 *
 * NOTES:
 * - The MG90S servo motors have a range of 0° to 180° - the range of motion must be declared in the attach() function as microsecond values to get the full range.
 * - This fuction is designed to be used while the robot is powered ON during the assembly process.
 * - Servo motors should be installed to the arms & paws but NOT fastened by the screws yet.
 * - Connect the servo motors to the corresponding GPIO pins and once the motors have moved to their positions, you can physically adjust the arms & paws to be perfectly aligned.
 * - Once alligned, turn the robot OFF & fasten the screws.
 */


// INCLUDES
#include <Arduino.h>
#include <Servo.h>

// GLOBAL VARIABLES
Servo servoD5_URP;    // upper right paw
Servo servoD6_URA;    // upper right arm
Servo servoD7_LRA;    // lower right arm
Servo servoD8_LRP;    // lower right paw
Servo servoD0_ULP;    // upper left paw
Servo servoD1_ULA;    // upper left arm
Servo servoD2_LLA;    // lower left arm
Servo servoD4_LLP;    // lower left paw


// HELPER FUNCTION
void zero() {   // Zeroing function for the main loop (calibrated values)
  servoD5_URP.write(140);   //   0° home | 180° extended | usable range =   0° to 180° | 140° level (zero)
  servoD6_URA.write(92);    // 180° home |  40° extended | usable range = 180° to  40° |  92° level (zero)
  servoD7_LRA.write(90);    //   0° home | 135° extended | usable range =   0° to 135° |  90° level (zero)
  servoD8_LRP.write(28);    // 175° home |   0° extended | usable range = 175° to   0° |  28° level (zero)
  servoD0_ULP.write(40);    // 177° home |   0° extended | usable range = 177° to   0° |  40° level (zero)
  servoD1_ULA.write(85);    //   0° home | 135° extended | usable range =   0° to 135° |  85° level (zero)
  servoD2_LLA.write(80);    // 175° home |  35° extended | usable range = 175° to  35° |  80° level (zero)
  servoD4_LLP.write(142);   //   2° home | 180° extended | usable range =   2° to 180° | 142° level (zero)
}


// SETUP
void setup(){
  // Initialize the GPIO pin numbers & the range of motion of each servo (500 = 0°, 2500 = 180°)
  servoD5_URP.attach(14, 500, 2500);
  servoD6_URA.attach(12, 500, 2500);
  servoD7_LRA.attach(13, 500, 2500);
  servoD8_LRP.attach(15, 500, 2500);
  servoD0_ULP.attach(16, 500, 2500);
  servoD1_ULA.attach(5, 500, 2500);
  servoD2_LLA.attach(4, 500, 2500);
  servoD4_LLP.attach(2, 500, 2500);

  Serial.println("\nSetup completed\n");
}

// MAIN LOOP
void loop() {
  zero();   // call the zeroing function
}


/*
 * LESSONS LEARNED:
 * - The Servo library is used to control servo motors.
 * - Declaring the servo motors as global variables makes it easier to use them in the zeroing function.
 * 
 * - zero() helper function is used to set the servo motors to specific angles, which gets called in the loop function.
 * 
 * - setup() function runs once when the program starts.
 * - attach() is used to connect each servo motor to a specific pin and set the range of motion.
 * 
 * - loop() function runs repeatedly as long as the board is powered on.
 * - Since the loop function calls the zero function continuously, the motors are forced to stay at the zero positions.
 * 
 * CALIBRATION:
 * - To calibrate the servo motors, you can adjust the angles in the zero function until the arms & paws are perfectly aligned.
 * - These offsets can be used in other programs to ensure the robot moves as expected.
 * - Each build will be different so these values will need to be adjusted during the build process.
 */
