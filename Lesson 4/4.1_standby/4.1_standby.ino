#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]

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
//Create the standby array
int array1[2][9] = {
  //G14  G12  G13  G15  G16  G5  G4  G2  MS
  {90,  90,  90,  90,  90,  90,  90,  90,  500},//Center point position
  {70,  90,  90,  110, 110,  90,  90,  70,  500},//Standby mode
};
//A function that sets the standby state
void standby() {
 for (int i = 0; i <= 1; i = i + 1) {  //There are two sets of arrays that need to be traversed twice
  servo_14.write(array1[i][0]); //The servo executes row i and column 1
  servo_12.write(array1[i][1]); //The servo executes row i and column 2
  servo_13.write(array1[i][2]); //The servo executes row i and column 3
  servo_15.write(array1[i][3]); //The servo executes row i and column 4
  servo_16.write(array1[i][4]); //The servo executes row i and column 5
  servo_5.write(array1[i][5]); //The servo executes row i and column 6
  servo_4.write(array1[i][6]); //The servo executes row i and column 7
  servo_2.write(array1[i][7]); //The servo executes row i and column 8
  delay(array1[i][8]); //Row i, column 9, execution delay time
 }
}
//Run the standby function
void loop() {
  standby();
  delay(1000);
}

