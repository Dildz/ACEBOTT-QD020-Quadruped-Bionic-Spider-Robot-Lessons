#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

//Create an action array for intermediate steps
int array10[9][9] = {
{70, 45, 135, 110, 110, 135, 45, 70, 400},//The four arms are left and right together
{115, 45, 135, 65, 110, 135, 45, 70, 400},//Right upper paw and right lower paw are raised
{70, 45, 135, 110, 65, 135, 45, 115, 400},//Right upper and lower paws down,left upper and lower paws up
{115, 45, 135, 65, 110, 135, 45, 70, 400},//Left upper and lower paws down,right upper and lower paws up
{70, 45, 135, 110, 65, 135, 45, 115, 400},//Right upper and lower paws down,left upper and lower paws up
{115, 45, 135, 65, 110, 135, 45, 70, 400},//Left upper and lower paws down,right upper and lower paws up
{70, 45, 135, 110, 65, 135, 45, 115, 400},//Right upper and lower paws down,left upper and lower paws up
{115, 45, 135, 65, 110, 135, 45, 70, 400},//Left upper and lower paws down,right upper and lower paws up
{75, 45, 135, 105, 110, 135, 45, 70, 400},//Right upper paw and right lower paw down
};

//A function to set intermediate steps
void dance2() {
 for (int i = 0; i <= 8; i = i + 1) {
  servo_14.write(array10[i][0]);
  servo_12.write(array10[i][1]);
  servo_13.write(array10[i][2]);
  servo_15.write(array10[i][3]);
  servo_16.write(array10[i][4]);
  servo_5.write(array10[i][5]);
  servo_4.write(array10[i][6]);
  servo_2.write(array10[i][7]);
  delay(array10[i][8]);
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

//Run the intermediate step program
void loop() {
  dance2();
  delay(1000);
}