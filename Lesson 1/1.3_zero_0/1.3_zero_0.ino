#include <Servo.h>  //Import servo library
//Create eight servos variables
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

void setup(){ //Initialize the GPIO pin number of each servo
  servo_14.attach(14);
  servo_12.attach(12);
  servo_13.attach(13);
  servo_15.attach(15);
  servo_16.attach(16);
  servo_5.attach(5);
  servo_4.attach(4);
  servo_2.attach(2);
}

void zero() { //Define the zeroing function
  servo_14.write(135);//Set right upper [paw]135°
  servo_12.write(45);//Set upper right [arm]45°
  servo_13.write(135);//Set lower right [arm]135°
  servo_15.write(45);//Set right lower [paw]45°
  servo_16.write(45);//Set upper left [paw]45°
  servo_5.write(135);//Set upper left [arm]135°
  servo_4.write(45);//Set lower left [arm]45°
  servo_2.write(135);//Set left lower [paw]135°
}

void loop() {
  zero();//Call the zeroing function
}