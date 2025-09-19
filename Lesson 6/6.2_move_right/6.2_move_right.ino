#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]


//Create an array of actions to move to the right
int array5[11][9] = {
{70, 90, 90, 110, 110, 90, 90, 70, 200},//Ready for standby
{90, 90, 90, 110, 110, 90, 90, 90, 200},//Right upper paw and left lower paw are raised
{90, 60, 90, 110, 110, 90, 120,90, 200},//Right upper arm and left lower arm back
{70, 60, 90, 110, 110, 90, 120,70, 200},//Right upper paw and left lower paw drop
{70, 60, 90, 90, 90, 90, 120,70, 200},//The left upper paw and right lower paw are raised
{70, 90, 90, 90, 90, 90, 90, 70, 200},//Right upper arm and left lower arm forward
{70, 90, 120,90, 90, 60, 90, 70, 200},//Left upper arm and right lower arm forward
{70, 90, 120,110, 110, 60, 90, 70, 200},//Left upper paw and right lower paw drop
{90, 90, 120,110, 110, 60, 90, 90, 200},//Right upper paw and left lower paw are raised
{90, 90, 90, 110, 110, 90, 90, 90, 200},//The upper left arm and lower right arm back
{70, 90, 90, 110, 110, 90, 90, 70, 200},//Right upper paw and left lower paw drop
};

//Sets the function to move to the right
void rightmove() {
 for (int i = 0; i <= 10; i = i + 1) {
  servo_14.write(array5[i][0]);
  servo_12.write(array5[i][1]);
  servo_13.write(array5[i][2]);
  servo_15.write(array5[i][3]);
  servo_16.write(array5[i][4]);
  servo_5.write(array5[i][5]);
  servo_4.write(array5[i][6]);
  servo_2.write(array5[i][7]);
  delay(array5[i][8]);
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

// Run the program moved to the right
void loop() {
rightmove();
}


