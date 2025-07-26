/*
   -- New project --
   
   This source code of graphical user interface 
   has been generated automatically by RemoteXY editor.
   To compile this code using RemoteXY library 3.1.13 or later version 
   download by link http://remotexy.com/en/library/
   To connect using RemoteXY mobile app by link http://remotexy.com/en/download/                   
     - for ANDROID 4.15.01 or later version;
     - for iOS 1.12.1 or later version;
    
   This source code is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.    
*/

//////////////////////////////////////////////
//        RemoteXY include library          //
//////////////////////////////////////////////

// you can enable debug logging to Serial at 115200
//#define REMOTEXY__DEBUGLOG    

// RemoteXY select connection mode and include library 
#define REMOTEXY_MODE__WIFI_POINT

#include <ESP8266WiFi.h>

// RemoteXY connection settings 
#define REMOTEXY_WIFI_SSID "ARKABOT_NANO"
#define REMOTEXY_WIFI_PASSWORD "123456789"
#define REMOTEXY_SERVER_PORT 6377
#define REMOTEXY_ACCESS_PASSWORD "123456789"


#include <RemoteXY.h>
#include <NewPing.h>

// RemoteXY GUI configuration  
#pragma pack(push, 1)  
uint8_t RemoteXY_CONF[] =   // 171 bytes
  { 255,6,0,0,0,164,0,19,0,0,0,65,82,75,65,66,79,84,95,78,
  65,78,79,0,16,2,106,200,200,84,1,1,5,0,5,3,130,47,47,9,
  25,54,54,32,1,25,31,5,56,130,47,47,137,25,54,54,32,1,25,31,
  12,19,109,68,10,71,56,56,7,195,31,25,77,97,110,117,97,108,32,67,
  111,110,116,114,111,108,0,65,118,111,105,100,32,79,98,106,101,99,116,0,
  70,111,108,108,111,119,32,79,98,106,101,99,116,0,80,97,116,114,111,108,
  108,105,110,103,0,83,112,105,110,110,105,110,103,0,2,34,89,38,14,82,
  45,32,7,0,16,25,12,1,79,78,0,79,70,70,0,129,13,17,90,65,
  78,9,46,33,64,17,240,159,164,150,0 };
  
// this structure defines all the variables and events of your control interface 
struct {

    // input variables
  int8_t joystick_01_x; // from -100 to 100
  int8_t joystick_01_y; // from -100 to 100
  int8_t joystick_02_x; // from -100 to 100
  int8_t joystick_02_y; // from -100 to 100
  uint8_t selectorMode; // from 0 to 5
  uint8_t buttonLampu; // =1 if switch ON and =0 if OFF

    // other variable
  uint8_t connect_flag;  // =1 if wire connected, else =0

} RemoteXY;   
#pragma pack(pop)
 
/////////////////////////////////////////////
//           END RemoteXY include          //
/////////////////////////////////////////////

// Motor Pins
#define IN1 D1
#define IN2 D2
#define IN3 D3
#define IN4 D4
// Sensor Pins
#define PIN_BUTTONLAMPU D7
#define TRIG_PIN D5
#define ECHO_PIN D6
#define MAX_DISTANCE 200

NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);



void setup() 
{
  RemoteXY_Init (); 
  
  pinMode (PIN_BUTTONLAMPU, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(PIN_BUTTONLAMPU, LOW);
  // TODO you setup code
  unsigned long patrolLastAction = 0;
  bool isTurning = false;
}

void loop() 
{ 
  RemoteXY_Handler ();
  
  digitalWrite(PIN_BUTTONLAMPU, (RemoteXY.buttonLampu==0)?LOW:HIGH);

  // Mode Selector
  int mode = RemoteXY.selectorMode;

  if (mode == 0) {
    // MANUAL MODE
    manualControl();
  }
  else if (mode == 1) {
    // AVOID MODE
    avoidObstacle();
  }
  else if (mode == 2) {
    // FOLLOW MODE
    followObject();
  }
  else if (mode == 3) {
    // PATROL MODE
    modePatrol();
  }
  else if (mode == 4) {
    // SPINNING MODE
    spinMode();
  }
  
}

// MODE 0 - Kontrol Manual
void manualControl() {
  int y = RemoteXY.joystick_01_y;
  int x = RemoteXY.joystick_02_x;

  if (y > 30) forward();
  else if (y < -30) backward();
  else if (x > 30) turnRightSoft();
  else if (x < -30) turnLeftSoft();
  else stopMotors();
}

// MODE 1 Avoid Obstacle
void avoidObstacle() {
  int distance = sonar.ping_cm();
  if (distance > 0 && distance < 25) {
    stopMotors();
    delay(300);
    turnRight();
    delay(600);
  }
  else {
    forward();
  }
}

// MODE 2 - Following Object
void followObject() {
 int distance = sonar.ping_cm();
  if (distance > 0 && distance < 20) {
    forward();
  }
  else {
    stopMotors();
  }
}

// MODE 3 - Patrol
// === MODE PATROL ===
void modePatrol() {
  static unsigned long patrolTimer = 0;
  static bool movingForward = true;
  unsigned long currentTime = millis();

  if (currentTime - patrolTimer >= 2000) { // 2 detik sekali
    patrolTimer = currentTime;
    movingForward = !movingForward;

    if (movingForward) {
      forward();
    } else {
      // Putar balik
      turnRight();
      delay(1100); // Sesuaikan agar berbalik arah
      forward();
    }
  }
}

// MODE 4 - Spinning
void spinMode() {
  turnRight(); // atau turnLeft();
}

// Motor functions
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRightSoft() {
  analogWrite(IN1, 40);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, 240);
  digitalWrite(IN4, LOW);
}

void turnLeftSoft() {
  analogWrite(IN1, 240);
  digitalWrite(IN2, LOW);
  analogWrite(IN3, 40);
  digitalWrite(IN4, LOW);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
