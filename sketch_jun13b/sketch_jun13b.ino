/* TODO:
  ✓  - void camRead -> find cube poss by it's color
  ✓  - void turn
  ✓  - void forward
  ✓  - void loop
  ✓  - void finish
    - void pid
    - void go -> add encoders code
    - variables value
    - try to connect MPU-92/65
    - GitHub repo
*/

/* Camera settings: Blob detector, I2C, addr 50, Dynamixel API, 5V.
   Wiring:
         Camera         Arduino Camera
   1-VF >|O O|  2-+5      SCL  -  IC0
   3-Gnd |O O|  4-Gnd     SDA  -  ID1
   5-TX   O O|  6-RX      5V   -  +5
   7-SCK |O O|  8-SNS     Gnd  -  Gnd
   9-IC0 |O O| 10-ID1
*/
#include "PinChangeInterrupt.h"
#include "TrackingCamI2C.h"
#include <NewPing.h>
#include <Servo.h>

#define DEBUG true

#define pwma_port 3
#define dira_port 12

#define max_distance 300
#define sonar_num 3
#define ping_interval 30

const byte front = 0; // front sonar number
const byte right = 1; // right sonar number
const byte left = 2;  // left sonar number

const byte red = 1;
const byte green = 0;

unsigned long pingTimer[sonar_num];
int cm[sonar_num] = {1000, 1000, 1000};
byte currentSensor = 0;
byte read_count = 0;

int turn_front_dist = 90;
int norm_outer_dist = 0;
int norm_inner_dist = 0;
int previous_front_dist = 150;
long long turning_dist = 3000;
long long forword_turning_dist[2] = {0, 0};

const byte min_angle = 0;
const byte straight_angle = 25;
const byte max_angle = 50;
byte turn_angle = 25;
int forword_turning_angle[5] = { -25, -13, 0, 13, 25};


bool cube_presence = false;
int tmp_nearest_cube = -1;
int cube_pos = -1;
byte cube_color = 0;
/* Cube color:
     red cube 0
     green cube 1
   Cube poss:
     no cube -1
          ↓
    ---00--01---
        |  |
    ---10--11---
        |  |
    ---20--21---
*/
byte new_poss = 2;
byte old_poss = 2;
byte robot_poss = 2;
/* Robot poss:
          ↓
    |---|---|---|
    | 3 | 2 | 1 |
    |---|---|---|
*/

bool clockwise = true;
long long turn_dist[3] = {0, 0, 0}; // 1900

const char table[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
byte old_enc = (PINB << 2) & 0b00001100;
volatile long encoder_value = 0;

byte count_turns = 0;

Servo myservo;
TrackingCamI2C trackingCam;
NewPing sonar[sonar_num] = {
  NewPing(7, 5, max_distance),   // front 0
  NewPing(4, 2, max_distance),   // right 1
  NewPing(A3, A2, max_distance), // left 2
};

void setup() {
  /* TrackingCamI2C::init(uint8_t cam_id, uint32_t speed);
       cam_id - 1..127, default 51
       speed - 100000/400000, cam enables auto detection of master clock
  */
  trackingCam.init(52, 100000);
  myservo.attach(6);
  myservo.write(straight_angle);
  if (DEBUG) Serial.begin(19200);
  pingTimer[0] = millis() + 75;
  for (uint8_t i = 1; i < sonar_num; i++) pingTimer[i] = pingTimer[i - 1] + ping_interval;
  encoderSetup();
  pinMode(pwma_port, OUTPUT);
  pinMode(dira_port, OUTPUT);
  digitalWrite(dira_port, HIGH);
  digitalWrite(pwma_port, HIGH);
}

void loop() {
  for (uint8_t i = 0; i < sonar_num; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += ping_interval * sonar_num;
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = sonar[currentSensor].ping_cm();
      read_count++;
      if (cm[0] == 0) cm[currentSensor] = previous_front_dist;
      if (cm[1] == 0) cm[currentSensor] = max_distance;
      if (cm[2] == 0) cm[currentSensor] = max_distance;
      previous_front_dist = cm[0];
    }
  }
  if (read_count == 3) {
    if (DEBUG) {
      for (int i = 0; i < 3; i++) {
//        Serial.print("\t");
        Serial.print("Sonar");
        Serial.print(i);
        Serial.print(" ");
        Serial.println(cm[i]);
      }
    }
    read_count = 0;
    oneSensorCycle();
  }

}

void oneSensorCycle() {
  if (DEBUG) Serial.println("Running oneSensorCycle()");
  if (count_turns == 12) finish();
  if (cm[0] <= turn_front_dist) turn();
  else forward();
}

void turn() {
  digitalWrite(pwma_port, LOW);
  if (DEBUG) {
    Serial.println("Running turn()");
    Serial.print("   -> ");
  }
  camRead();
  if (DEBUG) {
    Serial.print("   ");
    Serial.println("back to turn()");
  }
  digitalWrite(pwma_port, HIGH);
  count_turns++;
  new_poss = (1 * (trackingCam.blob[tmp_nearest_cube].type == green) +
              3 * (trackingCam.blob[tmp_nearest_cube].type == red)) * cube_presence +
             2 * (!cube_presence);
  old_poss = (4 - robot_poss) * clockwise + robot_poss * (!clockwise);
  if (DEBUG) {
    Serial.print("   ");
    Serial.print("turns: ");
    Serial.println(count_turns);
    Serial.print("   ");
    Serial.print("cube: ");
    Serial.println(cube_presence);
    if (cube_presence) {
      Serial.print("   ");
      Serial.print("color: ");
      Serial.println(trackingCam.blob[tmp_nearest_cube].type);
      Serial.print("   ");
      if (trackingCam.blob[tmp_nearest_cube].type == green)Serial.println("Green");
      if (trackingCam.blob[tmp_nearest_cube].type == red)Serial.println("Red");
    }
    Serial.print("   ");
    Serial.print("old_poss: ");
    Serial.println(old_poss);
    Serial.print("   ");
    Serial.print("new_poss: ");
    Serial.println(new_poss);
  }
  myservo.write(straight_angle);
  go(turn_dist[new_poss - 1]);
  if (clockwise) myservo.write(straight_angle - turn_angle);
  else myservo.write(straight_angle + turn_angle);
  go(turning_dist); //3000
  myservo.write(straight_angle);
  go(turn_dist[old_poss - 1]);
  robot_poss = new_poss;
}

void forward() {
  digitalWrite(pwma_port, LOW);
  if (DEBUG) {
    Serial.println("Running forward()");
    Serial.print("   -> ");
  }
  camRead();
  if (DEBUG) {
    Serial.print("   ");
    Serial.println("back to forward()");
  }
  digitalWrite(pwma_port, HIGH);
  new_poss = (1 * (trackingCam.blob[tmp_nearest_cube].type == green) +
              3 * (trackingCam.blob[tmp_nearest_cube].type == red)) * cube_presence +
              2 * (!cube_presence);
  old_poss = robot_poss;
  if (DEBUG) {
    Serial.print("   ");
    Serial.print("cube: ");
    Serial.println(cube_presence);
    if (cube_presence) {
      Serial.print("   ");
      Serial.print("color: ");
      Serial.println(trackingCam.blob[tmp_nearest_cube].type);
      Serial.print("   ");
      if (trackingCam.blob[tmp_nearest_cube].type == green)Serial.println("Green");
      if (trackingCam.blob[tmp_nearest_cube].type == red)Serial.println("Red");
    }
    Serial.print("   ");
    Serial.print("old_poss: ");
    Serial.println(old_poss);
    Serial.print("   ");
    Serial.print("new_poss: ");
    Serial.println(new_poss);
  }
  if ((new_poss - robot_poss) == 0) {
    pid();
  } else {
    myservo.write(straight_angle + forword_turning_angle[2 - (new_poss - robot_poss)]);
    if (DEBUG) {
      Serial.print("   ");
      Serial.print("forword_turning_angle: ");
      Serial.print(forword_turning_angle[2 - (new_poss - robot_poss)]);
    }
    go(forword_turning_dist[abs(new_poss - robot_poss) - 1]);
    myservo.write(straight_angle - forword_turning_angle[2 - (new_poss - robot_poss)]);
    go(forword_turning_dist[abs(new_poss - robot_poss) - 1]);
  }
  robot_poss = new_poss;
}

void go(long long dist) {
  if (DEBUG) {
    Serial.print("   -> ");
    Serial.println("Running go()");
  }
  volatile long tmp = encoder_value + dist;
  if (DEBUG) {
    Serial.print("\t");
    Serial.print("encoder_value: ");
    Serial.println(encoder_value);
    Serial.print("\t");
    Serial.print("tmp: ");
    Serial.print(tmp);
  }  
  while (encoder_value <= tmp) {
    Serial.print("\t");
    Serial.print("encoder_value: ");
    Serial.println(encoder_value);
    
  }
  if (DEBUG) {
    Serial.print("\t");
    Serial.print("encoder_value: ");
    Serial.println(encoder_value);
    Serial.print("\t");
    Serial.print("tmp: ");
    Serial.println(tmp);
  }  
}

void pid() {
  if (DEBUG) {
    Serial.print("   -> ");
    Serial.println("Running pid()");
  }
  myservo.write(straight_angle);
  // ... pid code
}

void camRead() {
  byte n = trackingCam.readBlobs(5); // read data about first 5 blobs
  if (DEBUG) {
    Serial.println("Running camRead()");
    Serial.print("\t");
    Serial.print("All blobs: ");
    Serial.println(n);
    for (int i = 0; i < n; i++) { // print information about all blobs
      Serial.print("\t");
      Serial.print(trackingCam.blob[i].type, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].dummy, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].cx, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].cy, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].area, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].left, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].right, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].top, DEC);
      Serial.print(" ");
      Serial.print(trackingCam.blob[i].bottom, DEC);
      Serial.println(" ");
    }
  }
  cube_presence = false;
  tmp_nearest_cube = -1;
  for (int i = 0; i < n; i++) { // finding nearest cube
    if (trackingCam.blob[i].type < 2) { // red(0) & green colors(1)
      if (!cube_presence) {
        cube_presence = true;
        tmp_nearest_cube = i;
      } else if (trackingCam.blob[i].bottom > trackingCam.blob[tmp_nearest_cube].bottom) tmp_nearest_cube = i;
    }
  }
  if ((DEBUG) && (cube_presence)) {
    Serial.print("\t");
    if (trackingCam.blob[tmp_nearest_cube].type == red) Serial.print("Nearest cube: color - red, y - ");
    if (trackingCam.blob[tmp_nearest_cube].type == green) Serial.print("Nearest cube: color - green, y - ");
    Serial.print(trackingCam.blob[tmp_nearest_cube].bottom);
    Serial.print(", x - ");
    Serial.println(trackingCam.blob[tmp_nearest_cube].cx);
  }
}

void finish() {
  if (DEBUG) Serial.println("Running finish()");
  digitalWrite(pwma_port, LOW); // stop
  while (true) {
    digitalWrite(13, HIGH);
    delay(1000);
    digitalWrite(13, LOW);
    delay(1000);
  }
}


void encoderSetup() {
    attachPCINT(digitalPinToPCINT(8), encoderInterrupt, CHANGE);
    attachPCINT(digitalPinToPCINT(9), encoderInterrupt, CHANGE);
}

void encoderInterrupt() {
  encoder_value++;
//    encoder_value += table[old_enc ^ (PINB & 0b00000011)];
//    old_enc = (PINB << 2) & 0b00001100;
}
