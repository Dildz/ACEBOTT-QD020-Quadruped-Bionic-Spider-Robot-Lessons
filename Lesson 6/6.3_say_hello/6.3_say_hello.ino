#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

//Create an array of hello actions
int array8[7][9] = {
 {70, 90, 135, 90, 90, 90, 90, 90, 400},//Left upper paw, right lower paw, left lower paw down
 {170, 90, 135, 90, 90, 90, 90, 90, 400},//Right upper paw raised
 {170, 130, 135, 90, 90, 90, 90, 90, 400},//Swing your right upper arm forward
 {170, 50, 135, 90, 90, 90, 90, 90, 400},//Swing your right upper arm back
 {170, 130, 135, 90, 90, 90, 90, 90, 400},//Swing your right upper arm forward
 {170, 90, 135, 90, 90, 90, 90, 90, 400},//Swing your right upper arm back
 {70, 90, 135, 90, 90, 90, 90, 90, 400},//Right upper arm down
};

//Set the hello function
void hello() {
 for (int i = 0; i <= 6; i = i + 1) {
  servo_14.write(array8[i][0]); 
  servo_12.write(array8[i][1]);
  servo_13.write(array8[i][2]);
  servo_15.write(array8[i][3]);
  servo_16.write(array8[i][4]);
  servo_5.write(array8[i][5]);
  servo_4.write(array8[i][6]);
  servo_2.write(array8[i][7]);
  delay(array8[i][8]);
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

// Run the hello program
void loop() {
  hello();
}


