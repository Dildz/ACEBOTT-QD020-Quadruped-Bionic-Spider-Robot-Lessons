#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

//Create an action array for the elementary step
int array9[10][9] = {
{90, 90, 90, 90, 90, 90, 90, 90, 400},//Lift all four paws
{50, 90, 90, 90, 90, 90, 90, 90, 400},//Right upper paw down
{90, 90, 90, 130, 90, 90, 90, 90, 400},//Right upper paw up, right lower paw down
{90, 90, 90, 90, 90, 90, 90, 50, 400},//Right lower paw up , left lower paw down
{90, 90, 90, 90, 130, 90, 90, 90, 400},//Left upper paw down, left lower paw up
{50, 90, 90, 90, 90, 90, 90, 90, 400},//Left upper paw up, right upper paw down
{90, 90, 90, 130, 90, 90, 90, 90, 400},//Right upper paw up, right lower paw down
{90, 90, 90, 90, 90, 90, 90, 50, 400},//Right lower paw up,left lower paw down
{90, 90, 90, 90, 130, 90, 90, 90, 400},//Left upper paw down, left lower paw up
{90, 90, 90, 90, 90, 90, 90, 90, 400},//Left upper paw up
};

//A function to set the elementary steps
void dance1() {
 for (int i = 0; i <= 9; i = i + 1) {
  servo_14.write(array9[i][0]);
  servo_12.write(array9[i][1]);
  servo_13.write(array9[i][2]);
  servo_15.write(array9[i][3]);
  servo_16.write(array9[i][4]);
  servo_5.write(array9[i][5]);
  servo_4.write(array9[i][6]);
  servo_2.write(array9[i][7]);
  delay(array9[i][8]);
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

//Run the program for the elementary step
void loop() {
  dance1();
  delay(1000);
}
