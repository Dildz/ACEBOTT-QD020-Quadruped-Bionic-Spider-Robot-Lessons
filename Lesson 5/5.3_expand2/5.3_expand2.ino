#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]


//Creates an array of action decompositions for moving forward
int array2[11][9] = { //There are 11 arrays with 9 columns
//G14  G12  G13  G15  G16  G5  G4  G2  MS （The latter is uniformly sorted according to this pin）
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Ready for standby
 {90, 90, 90, 110, 110, 90, 90, 90, 200},//Right upper paw and left lower paw are raised
 {90, 120, 90, 110, 110, 90, 60, 90, 200},//Right upper arm and left lower arm forward
 {70, 120, 90, 110, 110, 90, 60, 70, 200},//Right upper paw and left lower paw drop
 {70, 120, 90, 90, 90, 90, 60, 70, 200},//The left upper paw and right lower paw are raised
 {70, 90, 90, 90, 90, 90, 90, 70, 200},//Right upper arm and left lower arm back
 {70, 90, 120,90, 90, 60, 90, 70, 200},//Left upper arm and right lower arm forward
 {70, 90, 120,110, 110, 60, 90, 70, 200},//Left upper paw and right lower paw drop
 {90, 90, 120,110, 110, 60, 90, 90, 200},//Right upper paw and left lower paw are raised
 {90, 90, 90, 110, 110, 90, 90, 90, 200},//The upper left arm and lower right arm back
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Right upper paw and left lower paw drop
};



//Create an array of turns to the left. By default, the robot rotates 22.5 degrees
int array6[8][9] = {
//G14  G12  G13  G15  G16  G5  G4  G2  MS 
{70, 90, 90, 110, 110, 90, 90, 70, 200},//Ready for standby
{90, 90, 90, 110, 110, 90, 90, 90, 200},//Right upper paw and left lower paw are raised
{90, 135, 90, 110, 110, 90,135, 90, 200},//Right upper arm forward, left lower arm back
{70, 135, 90, 110, 110, 90,135, 70, 200},//Right upper paw and left lower paw drop
{70, 135, 90, 90, 90, 90, 135, 70, 200},//The left upper paw and right lower paw are raised
{70, 135, 135, 90, 90, 135,135, 70, 200},//Put your upper left arm back and your lower right arm forward
{70, 135, 135, 110, 110, 135,135, 70, 200},//Left upper paw and right lower paw drop
{70, 90, 90, 110, 110, 90, 90, 70, 200},//Four arms rotation
};

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

//Sets the forward function
void forward() {
for (int i = 0; i <= 10; i = i + 1) {
servo_14.write(array2[i][0]);
servo_12.write(array2[i][1]);
servo_13.write(array2[i][2]);
servo_15.write(array2[i][3]);
servo_16.write(array2[i][4]);
servo_5.write(array2[i][5]);
servo_4.write(array2[i][6]);
servo_2.write(array2[i][7]);
delay(array2[i][8]);
}
}


//A function that sets the left turn
void turnleft() {
 for (int i = 0; i <= 7; i = i + 1) {
  servo_14.write(array6[i][0]);
  servo_12.write(array6[i][1]);
  servo_13.write(array6[i][2]);
  servo_15.write(array6[i][3]);
  servo_16.write(array6[i][4]);
  servo_5.write(array6[i][5]);
  servo_4.write(array6[i][6]);
  servo_2.write(array6[i][7]);
  delay(array6[i][8]);
 }
}

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

//Four edges need to repeat forward and rotation actions four times, preferentially using the traversal loop.
void loop() {
 for (int i = 0; i <= 3; i = i + 1){
   for (int i = 0; i <= 2; i = i + 1){
     forward(); //Three times forward
  }
   for (int i = 0; i <= 3; i = i + 1){
     turnright(); //One rotate it four times to the right, which is about 90 degrees
  }
 }
}



