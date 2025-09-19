#include <Servo.h>
Servo servo_14;//Upper right [paw]
Servo servo_12;//Upper right [arm]
Servo servo_13;//Lower right [arm]
Servo servo_15;//Lower right [paw]
Servo servo_16;//Upper left [paw]
Servo servo_5;//Upper left [arm]
Servo servo_4;//Lower left [arm]
Servo servo_2;//Lower left [paw]
String item;//Because serial data is in string format, the variables are also in string format

//Create the standby array
int array1[2][9] = {
//G14  G12  G13  G15  G16  G5  G4  G2  MS
{90,  90,  90,  90,  90,  90,  90,  90,  500},//Center point position
  {70,  90,  90,  110, 110,  90,  90,  70,  500},//Standby mode
};

//dance1
int array9[10][9] = {
{90, 90, 90, 90, 90, 90, 90, 90, 400},
{50, 90, 90, 90, 90, 90, 90, 90, 400},
{90, 90, 90, 130, 90, 90, 90, 90, 400},
{90, 90, 90, 90, 90, 90, 90, 50, 400},
{90, 90, 90, 90, 130, 90, 90, 90, 400},
{50, 90, 90, 90, 90, 90, 90, 90, 400},
{90, 90, 90, 130, 90, 90, 90, 90, 400},
{90, 90, 90, 90, 90, 90, 90, 50, 400},
{90, 90, 90, 90, 130, 90, 90, 90, 400},
{90, 90, 90, 90, 90, 90, 90, 90, 400},
};

//dance2
int array10[9][9] = {
{70, 45, 135, 110, 110, 135, 45, 70, 400},
{115, 45, 135, 65, 110, 135, 45, 70, 400},
{70, 45, 135, 110, 65, 135, 45, 115, 400},
{115, 45, 135, 65, 110, 135, 45, 70, 400},
{70, 45, 135, 110, 65, 135, 45, 115, 400},
{115, 45, 135, 65, 110, 135, 45, 70, 400},
{70, 45, 135, 110, 65, 135, 45, 115, 400},
{115, 45, 135, 65, 110, 135, 45, 70, 400},
{75, 45, 135, 105, 110, 135, 45, 70, 400},
};

//dance3
int array11[10][9] = {
{70, 45, 45, 110, 110, 135, 135, 70, 400},
{90, 45, 45, 60, 90, 135, 135, 70, 400},
{70, 45, 45, 110, 110, 135, 135, 70, 400},
{90, 45, 45, 110, 90, 135, 135, 120, 400},
{70, 45, 45, 110, 110, 135, 135, 70, 400},
{90, 45, 45, 60, 90, 135, 135, 70, 400},
{70, 45, 45, 110, 110, 135, 135, 70, 400},
{90, 45, 45, 110, 90, 135, 135, 120, 400},
{70, 45, 45, 110, 110, 135, 135, 70, 400},
{70, 90, 90, 110, 110, 90, 90, 70, 400},
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
 standby();
 Serial.begin(115200);
 item = "";//Initialize the variable as an empty string
}

// Don't select \r\n in serial port, just select no
void loop() {   // Run the serial control dance program
  //Determine whether there is data in the serial port
  if (Serial.available()) { 
    item = Serial.readString(); //The variable is assigned to the data read by the serial port
    Serial.println(item); //The serial port prints out the input data

  //According to the input data, the condition is judged and the corresponding function is executed
  if (item == "1") { //When 1 is input to the serial port, dance1 is executed
    dance1();
    }
  if (item == "2") { //When 2 is input to the serial port, dance2 is executed
    dance2();
    }
  if (item == "3") { //When 3 is input to the serial port, dance3 is executed
    dance3();
    }
  }
}


