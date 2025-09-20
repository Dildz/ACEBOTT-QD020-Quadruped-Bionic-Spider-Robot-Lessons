#include <Servo.h>


Servo servo_14;   // Upper right [paw]
Servo servo_12;   // Upper right [arm]
Servo servo_13;   // Lower right [arm]
Servo servo_15;   // Lower right [paw]
Servo servo_16;   // Upper left [paw]
Servo servo_5;    // Upper left [arm]
Servo servo_4;    // Lower left [arm]
Servo servo_2;    // Lower left [paw]
String item;      // Because serial data is in string format, the variables are also in string format


const int PWMRES_Min = 1;   // PWM Resolution 1
const int PWMRES_Max = 180; // PWM Resolution 180
const int ALLMATRIX = 9;    // GPIO14 + GPIO12 + GPIO13 + GPIO15 + GPIO16 + GPIO5 + GPIO4 + GPIO2 + Run Time
const int ALLSERVOS = 8;    // GPIO14 + GPIO12 + GPIO13 + GPIO15 + GPIO16 + GPIO5 + GPIO4 + GPIO2
const int SERVOMIN = 500;
const int SERVOMAX = 2500;

int Running_Servo_POS [ALLMATRIX];  // Backup servo value


//-----------------------------------URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
const int Servo_Act_0 [ ] PROGMEM = { 80,  112,   70,   88,   95,   65,  100,   82,  1000};  // standby array
const int Servo_Act_1 [ ] PROGMEM = {100,  132,   50,   78,   75,   45,  120,   92,  2000};  // ready array

// Standby
const int Servo_Prg_1_Step = 2;
const int Servo_Prg_1 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,  112,   70,   88,   95,   65,  100,   82,  1000},  // standby
  {100,  132,   50,   78,   75,   45,  120,   92,  2000},  // ready
};

// Forward
const int Servo_Prg_2_Step = 8;
const int Servo_Prg_2 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 2 - move URA (+30) & LLA forward (-30)
  {100,  162,   50,   53,   50,   45,   90,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   80,   53,   50,   15,  120,   92,  400}, // step 4 - move URA (-30) & LLA back (+30) | move LRA (+30) & ULA forward (-30)
  {125,  132,   80,   78,   75,   15,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  400}, // step 6 - move LRA (-30) & ULA back (+30) | move URA (+30) & LLA forward (-30)
  {100,  162,   50,   78,   75,   45,   90,   92,  200}, // step 7 - drop URP & LLP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 8 - move URA (-30) & LLA back (+30)
};

// Backward
const int Servo_Prg_3_Step = 8;
const int Servo_Prg_3 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  132,   50,   78,   75,   45,  120,  117,  200}, // step 1 - lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 2 - move URA (-30) & LLA back (+30)
  {100,  102,   50,   53,   50,   45,  150,   92,  200}, // step 3 - drop URP & LLP (-25) | lift LRP & ULP (-25)
  {100,  132,   20,   53,   50,   75,  120,   92,  400}, // step 4 - move URA (+30) & LLA forward (-30) | move LRA (-30) & ULA back (+30)
  {125,  132,   20,   78,   75,   75,  120,  117,  200}, // step 5 - drop LRP & ULP (+25) | lift URP & LLP (+25)
  {125,  102,   50,   78,   75,   45,  150,  117,  400}, // step 6 - move LRA (+30) & ULA forward (-30) | move URA (-30) & LLA back (+30)
  {100,  102,   50,   78,   75,   45,  150,   92,  200}, // step 7 - drop URP & LLP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 8 - move URA (+30) & LLA forward (-30)
};

// Left shift 
const int Servo_Prg_4_Step = 5;
const int Servo_Prg_4 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   20,   53,   50,   75,  120,   92,  200},  // step 1 - lift ULP & LRP (-25) | move ULA (+30) & LRA (-30) back
  {125,  132,   20,   78,   75,   75,  120,  117,  400},  // step 2 - drop ULP & LRP (+25) | lift URP & LLP (+25)
  {125,  162,   50,   78,   75,   45,   90,  117,  200},  // step 3 - move ULA (-30) & LRA (+30) forward | move URA (+30) & LLA (-30) forward
  {100,  162,   50,   53,   50,   45,   90,   92,  400},  // step 4 - drop URP & LLP (-25) | lift ULP & LRP (-25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400},  // step 5 - move URA (-30) & LLA (+30) back | drop ULP & LRP (+25)
};

// Right shift
const int Servo_Prg_5_Step = 5;
const int Servo_Prg_5 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  102,   50,   78,   75,   45,  150,  117,  200},  // step 1 - lift URP & LLP (+25) | move URA (-30) & LLA (+30) back
  {100,  102,   50,   53,   50,   45,  150,   92,  400},  // step 2 - drop URP & LLP (-25) | lift ULP & LRP (-25)
  {100,  132,   80,   53,   50,   15,  120,   92,  200},  // step 3 - move URA (+30) & LLA (-30) forward | move ULA (-30) & LRA (+30) forward
  {125,  132,   80,   78,   75,   15,  120,  117,  400},  // step 4 - drop ULP & LRP (+25) | lift URP & LLP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400},  // step 5 - move ULA (+30) & LRA (-30) back | drop URP & LLP (-25)
};

// Turn left
const int Servo_Prg_6_Step = 9;
const int Servo_Prg_6 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,  172,   50,   78,   75,   45,  120,   92,  200}, // step 1 - lift URP (-25) | move URA forward (+40)
  {100,  172,   50,   78,   75,   45,  120,   92,  400}, // step 2 - drop URP (+25)
  {100,  172,   90,   53,   75,   45,  120,   92,  200}, // step 3 - lift LRP (-25) | move LRA forward (+40)
  {100,  172,   90,   78,   75,   45,  120,   92,  400}, // step 4 - drop LRP (+25)
  {100,  172,   90,   78,   75,   45,  160,  117,  200}, // step 5 - lift LLP (+25) | move LLA back (+40)
  {100,  172,   90,   78,   75,   45,  160,   92,  400}, // step 6 - drop LLP (-25)
  {100,  172,   90,   78,   50,   85,  160,   92,  200}, // step 7 - lift ULP (-25) | move ULA back (+40)
  {100,  172,   90,   78,   75,   85,  160,   92,  400}, // step 8 - drop ULP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 9 - rotate arms
};

// Turn right
const int Servo_Prg_7_Step = 9;
const int Servo_Prg_7 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {125,   92,   50,   78,   75,   45,  120,   92,  200}, // step 1 - lift URP (-25) | move URA back (-40)
  {100,   92,   50,   78,   75,   45,  120,   92,  400}, // step 2 - drop URP (+25)
  {100,   92,   10,   53,   75,   45,  120,   92,  200}, // step 3 - lift LRP (-25) | move LRA back (-40)
  {100,   92,   10,   78,   75,   45,  120,   92,  400}, // step 4 - drop LRP (+25)
  {100,   92,   10,   78,   75,   45,   80,  117,  200}, // step 5 - lift LLP (+25) | move LLA forward (-40)
  {100,   92,   10,   78,   75,   45,   80,   92,  400}, // step 6 - drop LLP (-25)
  {100,   92,   10,   78,   50,    5,   80,   92,  200}, // step 7 - lift ULP (-25) | move ULA forward (-40)
  {100,   92,   10,   78,   75,    5,   80,   92,  400}, // step 8 - drop ULP (+25)
  {100,  132,   50,   78,   75,   45,  120,   92,  400}, // step 9 - rotate arms
};

// Lie down
const int Servo_Prg_8_Step = 2;
const int Servo_Prg_8 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {110,   90,   90,   70,   70,   90,   90,  110,  1000},  // lift paws
  {100,  132,   50,   78,   75,   45,  120,   92,  1000},  // drop paws
};

// Say Hi
const int Servo_Prg_9_Step = 12;
const int Servo_Prg_9 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,   50,   68,   90,   45,  120,  112,  100},  // step 1 - drop ULP (+15) | lift LRP (-10) & LLP (+20)
  {100,  132,   50,   68,   82,   35,  120,  112,   50},  // step 2 - lift ULP (-8) | move ULA forward (-10)
  {180,  132,   50,   68,   90,   35,  120,  112,  400},  // step 3 - drop ULP (+8) | lift URP (+80)
  {180,  172,   50,   68,   90,   35,  120,  112,  200},  // step 4 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  200},  // step 5 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  200},  // step 6 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,  200},  // step 7 - swing URA back (-40)
  {180,  172,   50,   68,   90,   35,  120,  112,  400},  // step 8 - swing URA forward (+40)
  {180,  132,   50,   68,   90,   35,  120,  112,   50},  // step 9 - swing URA back (-40)
  {100,  132,   50,   68,   90,   35,  120,  112,   50},  // step 10 - drop URP (-80)
  {100,  132,   50,   68,   82,   45,  120,  112,   50},  // step 11 - lift ULP (-8) | move ULA back (+10)
  {100,  132,   50,   68,   90,   45,  120,  112,  900},  // step 12 - drop ULP (+8)
};

// Fighting
const int Servo_Prg_10_Step = 15;
const int Servo_Prg_10 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 70,  132,   50,   78,  105,   45,  120,   92,  500},  // step 1 - drop URP (-30) | drop ULP (+30)
  { 70,  112,   30,   78,  105,   25,  100,   92,  300},  // step 2 - move URA & LRA back, ULA & LLA forward (-20)
  { 70,  152,   70,   78,  105,   65,  140,   92,  300},  // step 3 - move URA & LRA forward, ULA & LLA back (+40)
  { 70,  112,   30,   78,  105,   25,  100,   92,  300},  // step 4 - move URA & LRA back, ULA & LLA forward (-40)
  { 70,  152,   70,   78,  105,   65,  140,   92,  300},  // step 5 - move URA & LRA forward, ULA & LLA back (+40)
  { 70,  112,   30,   78,  105,   25,  100,   92,  300},  // step 6 - move URA & LRA back, ULA & LLA forward (-40)
  { 70,  132,   50,   78,  105,   45,  120,   92,  500},  // step 7 - move URA & LRA forward, ULA & LLA back (+20)
  {100,  132,   50,   98,   75,   45,  120,   72,  500},  // step 8 - lift URP (+30) | lift ULP (-30) | drop LRP (+20) | drop LLP (-20)
  {100,  112,   30,   98,   75,   25,  100,   72,  300},  // step 9 - move URA & LRA back, ULA & LLA forward (-20)
  {100,  152,   70,   98,   75,   65,  140,   72,  300},  // step 10 - move URA & LRA forward, ULA & LLA back (+40)
  {100,  112,   30,   98,   75,   25,  100,   72,  300},  // step 11 - move URA & LRA back, ULA & LLA forward (-40)
  {100,  152,   70,   98,   75,   65,  140,   72,  300},  // step 12 - move URA & LRA forward, ULA & LLA back (+40)
  {100,  112,   30,   98,   75,   25,  100,   72,  300},  // step 13 - move URA & LRA back, ULA & LLA forward (-40)
  {100,  132,   50,   98,   75,   45,  120,   72,  500},  // step 14 - move URA & LRA forward, ULA & LLA back (+20)  
  {100,  132,   50,   78,   75,   45,  120,   92,  500},  // step 15 - lift LRP (-20) | lift LLP (+20)
};

// Push up
const int Servo_Prg_11_Step = 21;
const int Servo_Prg_11 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  132,    2,   68,   75,   45,  120,   92,   50},  // step 1 - lift LRP (-10) | move LRA back (-48)
  {100,  132,    2,   78,   75,   45,  120,  102,   50},  // step 2 - drop LRP (+10) | lift LLP (+10)
  {100,  132,    2,   78,   75,   45,  170,   92,   50},  // step 3 - move LLA back (+50) | drop LLP (-10)
  {100,  132,    2,   58,   75,   45,  170,  112,   50},  // step 4 - lift LRP (-20) | lift LLP (+20)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 5 - move URA back (-30) | move ULA back (+30)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 6 - drop URP (-40) | drop ULP (+45)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 7 - lift URP (+40) | lift ULP (-45)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 8 - drop URP (-40) | drop ULP (+45)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 9 - lift URP (+40) | lift ULP (-45)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 10 - drop URP (-40) | drop ULP (+45)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 11 - lift URP (+40) | lift ULP (-45)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 12 - drop URP (-40) | drop ULP (+45)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 13 - lift URP (+40) | lift ULP (-45)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 14 - drop URP (-40) | drop ULP (+45)
  {100,  102,    2,   58,   75,   75,  170,  112,  500},  // step 15 - lift URP (+40) | lift ULP (-45)
  { 60,  102,    2,   58,  120,   75,  170,  112,  500},  // step 16 - drop URP (-40) | drop ULP (+45)
  { 60,  132,    2,   58,  120,   45,  170,  112,  200},  // step 17 - move URA forward (+30) | move ULA forward (-30)
  { 60,  132,   50,   48,  120,   45,  170,  112,  100},  // step 18 - lift LRP (-10) | move LRA forward (+48)
  { 60,  132,   50,   78,  120,   45,  120,  122,  100},  // step 19 - drop LRP (+30) | move LLA forward (-50)
  { 60,  132,   50,   78,  120,   45,  120,   92,  100},  // step 20 - drop LLP (-30)
  {100,  132,   50,   78,   75,   45,  120,   92,  400},  // step 21 - lift URP (+40) | lift ULP (-45)
};

// Sleep
const int Servo_Prg_12_Step = 4;
const int Servo_Prg_12 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,  178,    2,   78,   75,    3,  170,   92,  500},   // step 1 - move arms in
  {  0,  178,    2,  175,  177,    3,  170,    2,  3000},  // step 2 - move paws in
  {100,  178,    2,   78,   75,    3,  170,   92,  500},   // step 3 - move paws out
  {100,  132,   50,   78,   75,   45,  120,   92,  1000},  // step 4 - move arms out
};

// Dance 1
const int Servo_Prg_13_Step = 9;
const int Servo_Prg_13 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 60,   92,   90,   78,   90,   85,   80,   92,  400},  // step 1 - drop URP (-30)
  { 90,   92,   90,  108,   90,   85,   80,   92,  400},  // step 2 - lift URP & drop LRP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   62,  400},  // step 3 - lift LRP & drop LLP (-30)
  { 90,   92,   90,   78,  120,   85,   80,   92,  400},  // step 4 - drop ULP & lift LLP (+30)
  { 60,   92,   90,   78,   90,   85,   80,   92,  400},  // step 5 - lift ULP & drop URP (-30)
  { 90,   92,   90,  108,   90,   85,   80,   92,  400},  // step 6 - lift URP & drop LRP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   62,  400},  // step 7 - lift LRP & drop LLP (-30)
  { 90,   92,   90,   78,  120,   85,   80,   92,  400},  // step 8 - drop ULP & lift LLP (+30)
  { 90,   92,   90,   78,   90,   85,   80,   92,  400},  // step 9 - lift ULP (+30)
};

// Dance 2
const int Servo_Prg_14_Step = 8;
const int Servo_Prg_14 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 1 - lift URP (+20) & LRP (-20)
  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 2 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 3 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)
  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 4 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 5 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)
  { 80,   92,   90,   88,   75,   85,   80,  102,  400},  // step 6 - drop URP & LRP (-20)       | lift ULP (-20) & LLP (+20)
  {100,   92,   90,   68,   95,   85,   80,   82,  400},  // step 7 - drop ULP (+20) & LLP (-20) | lift URP (+20) & LRP (-20)
  { 80,   92,   90,   88,   95,   85,   80,   82,  400},  // step 8 - drop URP & LRP (-20)
};

// Dance 3
const int Servo_Prg_15_Step = 16;
const int Servo_Prg_15 [][ALLMATRIX] PROGMEM = {
// URP---URA---LRA---LRP---ULP---ULA---LLA---LLP---MS
  { 80,   92,   90,   80,   95,   85,   80,   82,   50},  // step 1 - lift LRP (-8)
  { 80,   92,    2,   88,   95,   85,   80,   82,  100},  // step 2 - move LRA (-88) back | drop LRP (+8)
  { 80,   92,    2,   88,   95,   85,   80,   76,   50},  // step 3 - lift LLP (-8)
  { 80,   92,    2,   88,   95,   85,  170,   82,  100},  // step 4 - move LLA (+90) back | drop LLP (+8)
  {100,   92,    2,   63,   75,   85,  170,   82,  400},  // step 5 - lift URP (+20), ULP (-20) & LRP (-25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 6 - drop URP (-20), ULP (+20) & LRP (+25)
  {100,   92,    2,   88,   75,   85,  170,  107,  400},  // step 7 - lift URP (+20), ULP (-20) & LLP (+25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 8 - drop URP (-20), ULP (+20) & LLP (-25)
  {100,   92,    2,   63,   75,   85,  170,   82,  400},  // step 9 - lift URP (+20), ULP (-20) & LRP (-25)
  { 80,   92,    2,   88,   95,   85,  170,   82,  400},  // step 10 - drop URP (-20), ULP (+20) & LRP (+25)
  {100,   92,    2,   88,   75,   85,  170,  107,  400},  // step 11 - lift URP (+20), ULP (-20) & LLP (+25)
  { 80,   92,    2,   88,   95,   85,  170,   82,   50},  // step 12 - drop URP (-20), ULP (+20) & LLP (-25)
  { 80,   92,    2,   80,   95,   85,  170,   82,   50},  // step 13 - lift LRP (-8)
  { 80,   92,   90,   88,   95,   85,  170,   82,  100},  // step 14 - move LRA (+88) forward | drop LRP (+8)
  { 80,   92,   90,   80,   95,   85,  170,   76,   50},  // step 15 - lift LLP (-8)
  { 80,   92,   90,   88,   95,   85,   80,   82,  500},  // step 16 - move LLA (-90) forward | drop LLP (+8)
};


void Set_PWM_to_Servo(int iServo, int iValue) {
  int NewPWM = map(iValue, PWMRES_Min, PWMRES_Max, SERVOMIN, SERVOMAX);

	if (iServo >= 7) {
		servo_2.write(NewPWM);
	}
  else if (iServo >= 6) {
		servo_4.write(NewPWM);
	}
  else if (iServo >= 5) {
		servo_5.write(NewPWM);
	}
  else if (iServo >= 4) {
		servo_16.write(NewPWM);
	}
  else if (iServo >= 3) {
		servo_15.write(NewPWM);
	}
  else if (iServo >= 2) {
		servo_13.write(NewPWM);
	}
  else if (iServo >= 1) {
		servo_12.write(NewPWM);
	}
  else if (iServo == 0) {
		servo_14.write(NewPWM);
	}
}

void Servo_PROGRAM_Run(const int iMatrix[][ALLMATRIX], int iSteps) {
  for (int MainLoopIndex = 0; MainLoopIndex < iSteps; MainLoopIndex++) {
    // Move servos directly to target positions
    for (int ServoIndex = 0; ServoIndex < ALLSERVOS; ServoIndex++) {
      Set_PWM_to_Servo(ServoIndex, iMatrix[MainLoopIndex][ServoIndex]);
    }
    
    // Use the step delay time directly
    delay(iMatrix[MainLoopIndex][ALLMATRIX - 1]);
    
    // Backup current positions
    for (int Index = 0; Index < ALLSERVOS; Index++) {
      Running_Servo_POS[Index] = iMatrix[MainLoopIndex][Index];
    }
  }
}

void Servo_PROGRAM_Zero() {
	for (int Index = 0; Index < ALLMATRIX; Index++) {
		Running_Servo_POS[Index] = Servo_Act_0[Index];
	}

  for (int iServo = 0; iServo < ALLSERVOS; iServo++) {
		Set_PWM_to_Servo(iServo, Running_Servo_POS[iServo]);
		delay(10);
	}

	for (int Index = 0; Index < ALLMATRIX; Index++) {
		Running_Servo_POS[Index] = Servo_Act_1[Index];
	}

  for (int iServo = 0; iServo < ALLSERVOS; iServo++) {
		Set_PWM_to_Servo(iServo, Running_Servo_POS[iServo]);
		delay(10);
	}
}

void standby() {
  Servo_PROGRAM_Run(Servo_Prg_1, Servo_Prg_1_Step);
}

void sleep() {
  Servo_PROGRAM_Run(Servo_Prg_1, Servo_Prg_1_Step);
	Servo_PROGRAM_Run(Servo_Prg_12, Servo_Prg_12_Step);
}

void lie() {
  Servo_PROGRAM_Run(Servo_Prg_8, Servo_Prg_8_Step);
}

void forward() {
  Servo_PROGRAM_Run(Servo_Prg_2, Servo_Prg_2_Step);
}

void back() {
  Servo_PROGRAM_Run(Servo_Prg_3, Servo_Prg_3_Step);
}

void turnleft() {
  Servo_PROGRAM_Run(Servo_Prg_6, Servo_Prg_6_Step);
}

void turnright() {
  Servo_PROGRAM_Run(Servo_Prg_7, Servo_Prg_7_Step);
}

void leftmove() {
 Servo_PROGRAM_Run(Servo_Prg_4, Servo_Prg_4_Step);
}

void rightmove() {
 Servo_PROGRAM_Run(Servo_Prg_5, Servo_Prg_5_Step);
}

void hello() {
  Servo_PROGRAM_Run(Servo_Prg_9, Servo_Prg_9_Step);
	Servo_PROGRAM_Run(Servo_Prg_1, Servo_Prg_1_Step);
}

void dance1() {
 Servo_PROGRAM_Run(Servo_Prg_13, Servo_Prg_13_Step);
}

void dance2() {
 Servo_PROGRAM_Run(Servo_Prg_14, Servo_Prg_14_Step);
}

void dance3() {
 Servo_PROGRAM_Run(Servo_Prg_15, Servo_Prg_15_Step);
}

void pushup() {
  Servo_PROGRAM_Run(Servo_Prg_11, Servo_Prg_11_Step);
}

void fighting() {
  Servo_PROGRAM_Run(Servo_Prg_10, Servo_Prg_10_Step);
}
