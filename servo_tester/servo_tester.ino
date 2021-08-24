#include <Servo.h>

const byte min_angle = 30;                         //?
const byte straeght_angle = 90;                    //?
const byte max_angle = 150;                        //?

Servo myservo; 

void setup() {
  Serial.begin(9600);
  myservo.attach(9); 
}

int tmp = 0;
int angle = 0;

void loop() {
  // test angle
  if (Serial.available() > 0) {
    //Serial.println(Serial.read());
    tmp = Serial.read()- 48;
    if (tmp != -38){
      if (tmp == 71) {
        Serial.println("");
        Serial.print("@ngle: ");
        Serial.println(angle);
        if (angle > 180) angle = 180;
        myservo.write(angle);
        angle = 0;
      } else {
        angle = angle*10 + tmp;
        Serial.println(angle);
      }
    }
  }
}
