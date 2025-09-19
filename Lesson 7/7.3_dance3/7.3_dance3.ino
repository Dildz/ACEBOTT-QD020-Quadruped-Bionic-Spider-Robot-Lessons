#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

//Create an action array for advanced steps
int array11[10][9] = {
{70, 45, 45, 110, 110, 135, 135, 70, 400},//All four arms back
{90, 45, 45, 60, 90, 135, 135, 70, 400},//Right upper paw, left upper paw, right lower paw up
{70, 45, 45, 110, 110, 135, 135, 70, 400},//Right upper paw, left upper paw and right lower paw down
{90, 45, 45, 110, 90, 135, 135, 120, 400},//Right upper paw, left upper paw, left lower paw up
{70, 45, 45, 110, 110, 135, 135, 70, 400},//Right upper paw, left upper paw and left lower paw down
{90, 45, 45, 60, 90, 135, 135, 70, 400},//Right upper paw, left upper paw, right lower paw up
{70, 45, 45, 110, 110, 135, 135, 70, 400},//Right upper paw, left upper paw and right lower paw down
{90, 45, 45, 110, 90, 135, 135, 120, 400},//Right upper paw, left upper paw, left lower paw up
{70, 45, 45, 110, 110, 135, 135, 70, 400},//Right upper paw, left upper paw and left lower paw down
{70, 90, 90, 110, 110, 90, 90, 70, 400},//Standby mode
};

//A function to set advanced steps
void dance3() {
 for (int i = 0; i <= 9; i = i + 1) {
  servo_14.write(array11[i][0]);
  servo_12.write(array11[i][1]);
  servo_13.write(array11[i][2]);
  servo_15.write(array11[i][3]);
  servo_16.write(array11[i][4]);
  servo_5.write(array11[i][5]);
  servo_4.write(array11[i][6]);
  servo_2.write(array11[i][7]);
  delay(array11[i][8]);
 }
}

void setup() {
  servo_14.attach(14);
  servo_12.attach(12);
  servo_13.attach(13);
  servo_15.attach(15);
  servo_16.attach(16);
  servo_5.attach(5);
  servo_4.attach(4);
  servo_2.attach(2);

}
//Run the advanced step program
void loop() {
  dance3();
  delay(1000);
}

