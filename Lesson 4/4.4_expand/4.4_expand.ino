#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]


//Create the standby array
int array1[2][9] = {
//G14  G12  G13  G15  G16  G5  G4  G2  MS
{90,  90,  90,  90,  90,  90,  90,  90,  500},//Center point position
  {70,  90,  90,  110, 110,  90,  90,  70,  500},//Standby mode
};

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

//Creates a backward-moving action decomposition array
int array3[11][9] = {
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Ready for standby
 {90, 90, 90, 110, 110, 90, 90, 90, 200},//Right upper paw and left lower paw are raised
 {90, 60, 90, 110, 110, 90, 120,90, 200},//Right upper arm and left lower arm back
 {70, 60, 90, 110, 110, 90, 120,70, 200},//Right upper paw and left lower paw drop 
 {70, 60, 90, 90, 90, 90, 120,70, 200},//The left upper paw and right lower paw are raised
 {70, 90, 90, 90, 90, 90, 90, 70, 200},//Right upper arm and left lower arm forward 
 {70, 90, 60, 90, 90, 120,90, 70, 200},//The upper left arm and lower right arm back
 {70, 90, 60,110, 110, 120,90, 70, 200},//Left upper paw and right lower paw drop
 {90, 90, 60,110, 110, 120,90, 90, 200},//Right upper paw and left lower paw are raised
 {90, 90, 90, 110, 110, 90, 90, 90, 200},//Left upper arm and right lower arm forward
 {70, 90, 90, 110, 110, 90, 90, 70, 200},//Right upper paw and left lower paw drop
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

//Sets the backoff function
void back() {
 for (int i = 0; i <= 10; i = i + 1) {
servo_14.write(array3[i][0]);
servo_12.write(array3[i][1]);
servo_13.write(array3[i][2]);
servo_15.write(array3[i][3]);
servo_16.write(array3[i][4]);
servo_5.write(array3[i][5]);
servo_4.write(array3[i][6]);
servo_2.write(array3[i][7]);
delay(array3[i][8]);
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

void loop() {
  for (int i = 0; i <= 2; i = i + 1){
    forward();//Move forward three times
  }
  for (int i = 0; i <= 1; i = i + 1){
    back();//Move backwards twice
  }
  standby();//Standby mode
  }



