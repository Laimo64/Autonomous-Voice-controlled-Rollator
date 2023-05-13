/*------------------------------------------------------------------------------
  LIDARLite Arduino Library
  v3/GetDistancePwm
  This example shows how to read distance from a LIDAR-Lite connected over the
  PWM interface.
  Connections:
  LIDAR-Lite 5 Vdc (red) to Arduino 5v
  LIDAR-Lite Ground (black) to Arduino GND
  LIDAR-Lite Mode control (yellow) to Arduino digital input (pin 3)
  LIDAR-Lite Mode control (yellow) to 1 kOhm resistor lead 1
  1 kOhm resistor lead 2 to Arduino digital output (pin 2)
  (Capacitor recommended to mitigate inrush current when device is enabled)
  680uF capacitor (+) to Arduino 5v
  680uF capacitor (-) to Arduino GND
  See the Operation Manual for wiring diagrams and more information:
  http://static.garmin.com/pumac/LIDAR_Lite_v3_Operation_Manual_and_Technical_Specifications.pdf
------------------------------------------------------------------------------*/
#include <Servo.h>   //載入函式庫，這是內建的，不用安裝
#include <SoftwareSerial.h>   // 引用程式庫
#include <MsTimer2.h>

#define MotorA     8  
#define MotorB     1  
#define MotorA_PWMA    9  //定義 ENA (PWM調速) 接腳
#define MotorB_PWMB    5  //定義 ENB (PWM調速) 接腳****

volatile int dis=0;
unsigned long pulseWidth;

void setup(){
  Serial.begin(115200); // Start serial communications
  pinMode(MotorA,OUTPUT);
  pinMode(MotorB,OUTPUT);
  pinMode(MotorA_PWMA,OUTPUT);
  pinMode(MotorB_PWMB,OUTPUT);

  pinMode(2, OUTPUT); // Set pin 2 as trigger pin
  digitalWrite(2, LOW); // Set trigger LOW for continuous read
  pinMode(3, INPUT); // Set pin 3 as monitor pin
  
  //analogWrite(MotorA_PWMA,200);    //設定馬達 (右) 轉速
  //analogWrite(MotorB_PWMB,200);    //設定馬達 (左) 轉速
  MsTimer2::set(500, Lidar_scan); 
  MsTimer2::start();
}

void Lidar_scan(){
  pulseWidth = pulseIn(3, HIGH); // Count how long the pulse is high in microseconds
  if(pulseWidth != 0){
    dis= pulseWidth / 20; // 10usec = 1 cm of distance
  }
  Serial.println(dis);
}

void advance(){               //小車前進
    analogWrite(MotorA_PWMA,200); 
    analogWrite(MotorB_PWMB,200);
    digitalWrite(MotorA,HIGH);   
    digitalWrite(MotorB,HIGH);
}
void stopp(){                 //小車停止
    analogWrite(MotorA_PWMA,0); 
    analogWrite(MotorB_PWMB,0);
    digitalWrite(MotorA,LOW);   
    digitalWrite(MotorB,LOW);
}

void loop(){
  advance();
  if(dis<50){
    stopp();
  }
}
