# ACEBOTT QD020 Quadruped Bionic Spider Robot Lessons for VSCode & PlatformIO

All ACEBOTT QD020 Quadruped Bionic Spider Robot Arduino IDE lessons reworked for VSCode & PlatformIO. The original .ino lesson files are included for reference.

Follow the PDF instruction documentation for assembling the robot, installing the driver software & following along with the lessons.

## 📝 Initial Notes
- During the build process, use the 1.3_zero_0 project to calibrate your robot's zero positions as these will differ from build to build due to mechanical differences. As a result, all the position array values will need to be adjusted based off of the calibrated values.
- Methods used in the 'expand' lessons have been modified in favour of non-blocking state machine logic.
- In the custom app control project the code has been further reworked to make use of custom drivers for movement & Wi-Fi related functions.

## 📋 Table of Contents
- [Introduction](#introduction)
- [Prerequisites](#prerequisites)
- [Installation & Setup](#installation--setup)
- [Lesson Projects](#lesson-projects)
- [Project Structure](#project-structure)
- [WiFi Control](#wifi-control)
- [Movement Library](#movement-library)
- [License](#license)

## 🤖 Introduction

The [ACEBOTT QD020](https://acebott.com/product/qd020-bionic-spider-kit/) is a quadruped robot using an ESP8266 that you can program & control via Wi-Fi using the ACEBOTT smartphone app. This repository contains lessons that teach you how to:
- Calibrate the servo motor zero positions according to your build
- Program the robot's movements and behaviors
- Set up Wi-Fi communication between your phone and the robot
- Easily create & experiment with custom movement sequences
- Understand servo control and movement coordination

## 📚 Prerequisites

Before starting, you'll need:
- ACEBOTT QD020 Quadruped Spider Robot
- A computer with Windows, macOS, or Linux
- CP210x driver installed
- USB-C cable to connect the robot to your computer

## 💻 Installation & Setup

### 1. Install Visual Studio Code
Download and install VSCode from [code.visualstudio.com](https://code.visualstudio.com/)

### 2. Install PlatformIO Extension
1. Open VSCode
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "PlatformIO IDE"
4. Click Install

### 3. Download this Repository
1. Click on the Green 'Code' button & select Download ZIP - OR - use this [direct link](https://github.com/Dildz/ACEBOTT-QD020-Quadruped-Bionic-Spider-Robot-Lessons/archive/refs/heads/main.zip)
2. Unzip & move the ACEBOTT-QD020-Quadruped-Bionic-Spider-Robot-Lessons-main folder to your desired location.

### 4. Open Lesson Project in VSCode
1. File → Open Folder
2. Select the a lesson folder you wish to open (i.e: "~\Lesson 1\1.1_hello_esp8266")
3. Wait a moment for PlatformIO to initialize, it will automatically configure the project based off of the 'platformio.ini' config file.

### 5. Connect Your Robot
1. Connect the robot to your computer via USB, but do not turn it ON yet...
2. Check your computer's Device Manager → Ports (COM & LTP) to make sure the Silicon Labs CP210x USB to UART Bridge is showing a port number

### 6. Upload Code
1. Click the PlatformIO extension icon in the left sidebar
2. Under "Project Tasks" → "General" → click "Upload" - OR - click the '→' icon in the VSCode status bar (bottom left)
   - this will build (compile) the code & upload to the robot.

## 🎓 Lesson Projects List

### Lesson 1
- **1.1_hello_esp8266**
- **1.2_servo_test**
- **1.3_zero_0**
- **1.4_storage**
### Lesson 4
- **4.1_standby**
- **4.2_forward**
- **4.3_backward**
- **4.4_expand**
### Lesson 5
- **5.1_turn_left**
- **5.2_turn_right**
- **5.3_expand2**
### Lesson 6
- **6.1_move_left**
- **6.1_move_right**
- **6.3_say_hello**
- **6.4_expand**
### Lesson 7
- **7.1_dance1**
- **7.2_dance2**
- **7.3_dance3**
- **7.4_expand**
### Lesson 8
- **8.0.1_center_point**
- **8.0.2_initial_pos**
- **8.0.3_lie_down**
- **8.0.4_fighting**
- **8.0.5_push_up**
- **8.0.6_sleep**
- **8.0.7_expand**
- **8.1_app_control**
- **8.1_app_control_custom**

Each lesson includes the original ArduinoIDE .ino file for reference.

## 📶 WiFi Control (lesson 8.1 projects)

The robot creates its own WiFi network that your mobile device can connect to:

- **SSID:** QuadBot
- **Password:** 12345678

Feel free to change the SSID & password in the code if you wish.

After connecting to the network, you can use the ACEBOTT control app to connect to the robot to send commands.

## 🦵 Movement Library

The robot has 8 servos controlling its movement:

| Servo | GPIO   | Function              |
|-------|--------|-----------------------|
| D0    | GPIO16 | Upper Left Paw (ULP)  |
| D1    | GPIO5  | Upper Left Arm (ULA)  |
| D2    | GPIO4  | Lower Left Arm (LLA)  |
| D4    | GPIO2  | Lower Left Paw (LLP)  |
| D5    | GPIO14 | Upper Right Paw (URP) |
| D6    | GPIO12 | Upper Right Arm (URA) |
| D7    | GPIO13 | Lower Right Arm (LRA) |
| D8    | GPIO15 | Lower Right Paw (LRP) |

The movement library includes pre-programmed sequences for:
- Standing and ready positions
- Walking forward and backward
- Turning left and right
- Sideways (strafe) movement
- Waving hello
- 3 Dance routines
- Push-ups and fighting
- Sleeping and lying down

Custom movements & actions can be easily created & added to any of the 'expand' projects. We can get creative with these projects to run our own moves routines & is where most of the learning happens. 

Custom movements can be added to the 'add_control' projects but we don't have access to the control app source code to modify the names in the app.

## 💡 Future Plans

There are 2 additional (empty) pins available on the header board - A0 & D3.

I'd like to use these pins or unused pins on the ESP8266 itself to drive a 0.96inch OLED I2C display and 3D print a mouning solution.

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Future Plans
