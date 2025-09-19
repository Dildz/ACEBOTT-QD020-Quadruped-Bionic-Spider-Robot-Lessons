#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]


//Create an array of actions for turning right (for setting the rotation Angle, see instructions for turning left arrays)
int array7[8][9] = {
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Ready for standby
 {70, 90, 90, 90, 90, 90, 90, 70, 200},//The left upper paw and right lower paw are raised
 {70, 90, 45, 90, 90, 45, 90, 70, 200},//The upper left arm goes forward and the lower right arm goes back
 {70, 90, 45, 110, 110, 45, 90, 70, 200},//Left upper paw and right lower paw drop
 {90, 90, 45, 110, 110, 45, 90, 90, 200},//Right upper paw and left lower paw are raised
 {90, 45, 45, 110, 110, 45, 45, 90, 200},//Right upper arm back, left lower arm forward
 {70, 45, 45, 110, 110, 45, 45, 70, 200},//Right upper paw and left lower paw drop
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Four arms rotation
};
//A function that sets the right turn
void turnright() {
 for (int i = 0; i <= 7; i = i + 1) {
  servo_14.write(array7[i][0]);
  servo_12.write(array7[i][1]);
  servo_13.write(array7[i][2]);
  servo_15.write(array7[i][3]);
  servo_16.write(array7[i][4]);
  servo_5.write(array7[i][5]);
  servo_4.write(array7[i][6]);
  servo_2.write(array7[i][7]);
  delay(array7[i][8]);
 }
}

void setup(){
  servo_14.attach(14);
  servo_12.attach(12);
  servo_13.attach(13);
  servo_15.attach(15);
  servo_16.attach(16);
  servo_5.attach(5);
  servo_4.attach(4);
  servo_2.attach(2);
}

//Run the right turn
void loop() {
  turnright();
}
