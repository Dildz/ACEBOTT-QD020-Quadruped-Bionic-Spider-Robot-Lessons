#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "text.h"

const char* ssid = "QuadBot-E";//Wi-Fi name of the quadruped robot
const char* password = "12345678";//Wi-Fi password for the quadruped robot

WiFiServer server(100);//Create a Wi-Fi server object with port 100
WiFiClient client;//Create a Wi-Fi client object
String sendBuff;//Send buffer
byte RX_package[17] = {0};//An array of incoming packets
byte callback_forward_package[5] = {0xff,0x55,0x02,0x01,0x01};// Forward command packet
byte callback_back_package[5] = {0xff,0x55,0x02,0x01,0x02};// Back command packet
byte callback_leftmove_package[5] = {0xff,0x55,0x02,0x01,0x03};// Move command packet left
byte callback_rightmove_package[5] = {0xff,0x55,0x02,0x01,0x04};// Right shift command packet
byte callback_turnleft_package[5] = {0xff,0x55,0x02,0x01,0x05};// Left turn command packet
byte callback_turnright_package[5] = {0xff,0x55,0x02,0x01,0x06};// Right turn command packet
byte callback_standby_package[5] = {0xff,0x55,0x02,0x01,0x07};// standby command packet
byte callback_sleep_package[5] = {0xff,0x55,0x02,0x01,0x08};//  command packet
byte callback_lie_package[5] = {0xff,0x55,0x02,0x01,0x09};// command packet
byte callback_hello_package[5] = {0xff,0x55,0x02,0x01,0x0a};// command packet
byte callback_pushup_package[5] = {0xff,0x55,0x02,0x01,0x0b};// command packet
byte callback_fighting_package[5] = {0xff,0x55,0x02,0x01,0x0c};// command packet
byte callback_dance1_package[5] = {0xff,0x55,0x02,0x01,0x0d};// command packet
byte callback_dance2_package[5] = {0xff,0x55,0x02,0x01,0x0e};// command packet
byte callback_dance3_package[5] = {0xff,0x55,0x02,0x01,0x0f};// command packet

byte Serial_package[5] = {0};// Serial packets
byte dataLen, index_a = 0;// Data length and index variable
char buffer[52];// Buffer
unsigned char prevc=0;// previous character
bool isStart = false;// Flag if receive has started
bool ED_client = true;// Flag if the client is connected
bool WA_en = false;// Mark if WA is enabled

bool st = false;

#define CMD_RUN 1 // Forward command identity
#define CMD_daiji 3 // Flag for standby command
#define CMD_jiaozun 4 // Identity of the calibration command
#define CMD_suijiao 5 // Identifier for the sleep command
#define CMD_padi 6 // The flag of the planking command
#define CMD_dazaohu 7 // Flag for the say hello command
#define CMD_fuwoceng 8 // The flag for the push up command
#define CMD_zandou 9 // Battle command identifier
#define CMD_dance1 10 //The identity of the Dance 1 command
#define CMD_dance2 11 //The identity of the Dance 2 command
#define CMD_dance3 12 //The identity of the Dance 3 command

void setup()
{
  Serial.setTimeout(10); // Set the serial timeout to 10ms
  Serial.begin(115200);  // Initiate serial communication with baud rate of 115200

  WiFi.mode(WIFI_AP);// Set Wi-Fi mode to access point mode
  WiFi.softAP(ssid,password,5);//Create a Wi-Fi access point, specify the network name and password, and set the channel to 5
  server.begin();// Start the Wi-Fi server
  delay(100);

  servo_14.attach(14, SERVOMIN, SERVOMAX);// Connect the servo to pin 14
  servo_12.attach(12, SERVOMIN, SERVOMAX);// Connect the servo to pin 12
  servo_13.attach(13, SERVOMIN, SERVOMAX);// Connect the servo to pin 13
  servo_15.attach(15, SERVOMIN, SERVOMAX);// Connect the servo to pin 15
  servo_16.attach(16, SERVOMIN, SERVOMAX);// Connect the servo to pin 16
  servo_5.attach(5, SERVOMIN, SERVOMAX);// Connect the servo to pin 5
  servo_4.attach(4, SERVOMIN, SERVOMAX);// Connect the servo to pin 4
  servo_2.attach(2, SERVOMIN, SERVOMAX);// Connect the servo to pin 2

  Servo_PROGRAM_Zero();// Reset the servo program to zero
}
unsigned char readBuffer(int index_r)// Read the characters in the buffer with index index_r and return
{
  return buffer[index_r]; 
}
void writeBuffer(int index_w,unsigned char c)
{
  buffer[index_w]=c;// Write the character c to the buffer at index index_w position
}
// Pattern
void runModule(int device)
{
  switch(device) 
  {
    case 0x0C:
    {   
      int val = readBuffer(12);
      switch (val)
      {
        case 0x01:       
            forward();//Serial.println("ok");
            //Serial.write(callback_forward_package,5);
            client.write(callback_forward_package,5);
            break;
        case 0x02:
            back();
            client.write(callback_back_package,5);
            break;
        case 0x03:
            leftmove();
            client.write(callback_leftmove_package,5);
            break;
        case 0x04:
            rightmove();
            client.write(callback_rightmove_package,5);
            break;
        case 0x05:
            turnleft();
            client.write(callback_turnleft_package,5);
            break;
        case 0x06:
            turnright();
            client.write(callback_turnright_package,5);
            break;
        default:
            break;
      }
    }break;
  }   
}
void parseData()
{ 
  isStart = false;
  int action = readBuffer(9);
  int device = readBuffer(10);
  switch(action)
  {
    case CMD_RUN:
    {
      runModule(device);
    }
    break;
    case CMD_daiji:
    {
      standby();//Serial.println("ok");
      client.write(callback_standby_package,5);
    }
    break;
    case CMD_jiaozun:
    {
      //zero();
    }
    break;
    case CMD_suijiao:
    {
      sleep();
      client.write(callback_sleep_package,5);
    }
    break;
    case CMD_padi:
    {
      lie();
      client.write(callback_lie_package,5);
    }
    break;
    case CMD_dazaohu:
    {
      hello();
      client.write(callback_hello_package,5);
    }
    break;
    case CMD_fuwoceng:
    {
      pushup();
      client.write(callback_pushup_package,5);
    }
    break;
    case CMD_zandou:
    {
      fighting();
      client.write(callback_fighting_package,5);
    }
    break;
    case CMD_dance1:
    {
      dance1();
      client.write(callback_dance1_package,5);
    }
    break;
    case CMD_dance2:
    {
      dance2();
      client.write(callback_dance2_package,5);
    }
    break;
    case CMD_dance3:
    {
      dance3();
      client.write(callback_dance3_package,5);
    }
    break;
  }
}
void loop()
{
  client = server.available();
  if (client)
  {
    WA_en = true;
    ED_client = true;
    Serial.println("[Client connected]");
    unsigned long previousMillis = millis();  
    const unsigned long timeoutDuration = 3000;  
    while (client.connected())
    {
      if ((millis() - previousMillis) > timeoutDuration && client.available() == 0 && st==true)
      {
        break;
      }  
      if (client.available())
      {
        previousMillis = millis();
        unsigned char c = client.read()&0xff;
        Serial.write(c);
        st = false;
        if (c == 200)
        {
          st = true;
        }
        if(c==0x55&&isStart==false)
        {
          if(prevc==0xff)
          {
            index_a=1; 
            isStart = true;
          }
        }
        else
        {
          prevc = c;
          if(isStart)
          {
            if(index_a==2)
            {
            dataLen = c; 
            }
            else if(index_a>2)
            {
              dataLen--;
            }
            writeBuffer(index_a,c);
          }
        }
        index_a++;
        if(index_a>120)
        {
          index_a=0; 
          isStart=false;
        }
        if(isStart&&dataLen==0&&index_a>3)
        { 
          isStart = false;
          parseData();
          index_a=0;
        }
      }
      if (Serial.available())
      {
        char c = Serial.read();
        sendBuff += c;
        client.print(sendBuff);
        Serial.print(sendBuff);
        sendBuff = "";
      }
      static unsigned long Test_time = 0;
      if (millis() - Test_time > 1000)
      {
        Test_time = millis();
        if (0 == (WiFi.softAPgetStationNum()))
        {
          //Serial.write(Stop,17);
          break;
        }
      }
    }
    //Serial.write(Stop,17);
    client.stop();
    Servo_PROGRAM_Zero();
    Serial.println("[Client disconnected]");
  }
  else
  {
    if (ED_client == true)
    {
      ED_client = false;
      //Serial.write(Stop,17);
    }
  }
}
