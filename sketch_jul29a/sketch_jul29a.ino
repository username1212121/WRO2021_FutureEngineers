#include <Servo.h>
#include <Wire.h>

#define DEBUG true

#define IN3_port 6
#define IN4_port 5
#define SERVO_port 9
#define BUTTON_port 16
#define r_led A0
#define g_led A1
#define b_led A2

#define True = true
#define TRUE = true
#define t = true
#define T = true
#define False = false
#define FALSE = false
#define f = false
#define F = false

const byte right_angle = 40;                         
const byte straeght_angle = 90;                   
const byte left_angle = 140; 

bool button_state = false;
bool STOP = true;
bool CUBE = false;
bool cube_bypassing = false;
byte turns = 0;
unsigned int robot_speed = 110; 
unsigned long long start_time = 0;
unsigned long long finish_time = 0;
unsigned long long cube_bypassing_time = 200; 
unsigned long long last_click_time = 0; 
unsigned long long stop_time = 0; 
unsigned long long previous_turning_time = 0;

float WK = 1.6;
float CK = 1.35;

Servo myservo;

void setup() {
  delay(2000);
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(b_led, OUTPUT);
  Wire.begin(2);                // join i2c bus with address #8
  Wire.onReceive(receiveEvent); // register event
  if (DEBUG) Serial.begin(9600);           // start serial for output
  pinMode(IN3_port, OUTPUT);
  pinMode(IN4_port, OUTPUT);
  pinMode(BUTTON_port, INPUT);
  myservo.attach(SERVO_port);
  while (button_state == false){
    if (DEBUG) {
      Serial.print("BTN: ");
      Serial.println(button_state);
    }
    button_state = digitalRead(BUTTON_port);
  }
  STOP = false;
  
  //start_time = millis();
  finish_time = 5000000 + millis();
 // Вперёд      
 //0 -> max speed
 //255 -> stop
}


void loop() {
  //delay(100);
  button_state = digitalRead(BUTTON_port);
  if (turns == 11) finish_time = millis() + 3000;
  if (millis() > finish_time) {
    STOP = true; 
  }else{
    if ((button_state)&&(millis() > last_click_time)){
      STOP = !STOP;
      if (STOP) {
        stop_time = millis();
      } else {
        finish_time += millis() - stop_time;
        stop_time = 0;
      }
      last_click_time = millis() + 1000;
      //Serial.println(STOP);
    }
  }
  if ((DEBUG)&&(!STOP)) {
    Serial.print("Time until the end: ");
    unsigned long a = finish_time-millis();
    Serial.println(a);
  }
}


// function that executes whenever data is received from master
// this function is registered as an event, see setup()
void receiveEvent(int howMany) {
  int value = 0;
  bool negative_value = false;
  while (1 < Wire.available()) { // loop through all but the last
    char c = Wire.read(); // receive byte as a character
    Serial.println(c);         // print the character
    if (c == 't'){
      if (previous_turning_time < millis()) {
        previous_turning_time = millis() + 2000;
        turns += 1;
        digitalWrite(r_led, LOW);
        digitalWrite(b_led, LOW);
        digitalWrite(g_led, HIGH);
        delay(300);
        digitalWrite(g_led, LOW);
      }
    } else if (c == 'c') CUBE = true;
    else if (c == 'w') CUBE = false;
    else if (c == '-') {
      negative_value = true;
    } else {
      value = value * 10 + c - 48; 
    }
  }
  char x = Wire.read();    // receive byte as an integer
  value = value * 10 + x - 48;
  int new_value = 100;
  if (!CUBE) {
    if (DEBUG) {
      Serial.print("Walls error: ");
      if (negative_value) Serial.print("-");
      Serial.println(value);         // print the integer
    }
    int sqrt_value = int(sqrt(value));
    if (sqrt_value > 65) sqrt_value = 65;
    if (negative_value) {
      sqrt_value = -sqrt_value;
      negative_value = false; 
    }
    if (DEBUG) {
      Serial.println("@");
      Serial.println(sqrt_value);
    }
    if ((!cube_bypassing)||(cube_bypassing_time > millis())){
      new_value = map(sqrt_value * WK, -70, 70, left_angle, right_angle);
      //digitalWrite(b_led, LOW);
      cube_bypassing = false;
    }
  } else {    
    //digitalWrite(b_led, HIGH);
    cube_bypassing = true;
    cube_bypassing_time = millis() + 4000;
    if (DEBUG) {
      Serial.print("Cube error: ");
      if (negative_value) Serial.print("-");
      Serial.println(value);         // print the integer
    }    

    value = int(value * CK);
    if (value > 160) value = 160;
    if (negative_value) {
      value = -value;
      negative_value = false; 
    }
    new_value = map(value, -160, 160, left_angle, right_angle);
    if (new_value > left_angle) new_value = left_angle;
    if (new_value < right_angle) new_value = right_angle;
    
  }
  if (DEBUG) {
    Serial.print("Servo: ");
    Serial.println(new_value);
  }
  drive(new_value);
}

void drive(int angle) {
  if (STOP) {
    myservo.write(90); 
    digitalWrite(IN3_port, LOW);   
    digitalWrite(IN4_port, LOW);
    
    digitalWrite(r_led, HIGH);
    digitalWrite(g_led, LOW);
    digitalWrite(b_led, LOW);
  } else {
    digitalWrite(r_led, LOW);
    myservo.write(angle);
    analogWrite(IN3_port, robot_speed);
    digitalWrite(IN4_port, HIGH);
  }
}
