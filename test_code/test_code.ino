/* Camera settings: Blob detector, I2C, addr 50, Dynamixel API, 5V.
   Wiring:
         Camera         Arduino Camera
   1-VF >|O O|  2-+5      SCL  -  IC0
   3-Gnd |O O|  4-Gnd     SDA  -  ID1
   5-TX   O O|  6-RX      5V   -  +5
   7-SCK |O O|  8-SNS     Gnd  -  Gnd
   9-IC0 |O O| 10-ID1
*/

#include "TrackingCamI2C.h"
#include <NewPing.h>
#include <Servo.h>

#define DEBUG false

#define pwma_port 3
#define dira_port 12

#define max_distance 300
#define sonar_num 3
#define ping_interval 30

const byte front = 0; // front sonar number
const byte right = 1; // right sonar number
const byte left = 2;  // left sonar number
const byte right_back = 3; // right sonar number
const byte left_back = 4;  // left sonar number

const byte red = 1;
const byte green = 0;

unsigned long pingTimer[sonar_num];
int cm[sonar_num] = {1000, 1000, 1000};
byte currentSensor = 0;
byte read_count = 0;

const byte min_angle = 50;
const byte straight_angle = 100;
const byte max_angle = 150;

int turns = 0;
unsigned long turn_delay = 0;
unsigned long turn_time = 0;
bool turn_check = false;

int cube_shift = 0;
bool r_cube = false;
bool g_cube = false;

double kp = 2 , ki = 0, kd = 2;
double p, i, d, i_err;
double max_error, min_error;

bool clockwise = true;


/* Servo position:
   L = 150
   R = 50
   0 = 100
*/

Servo myservo;
TrackingCamI2C trackingCam;
NewPing sonar[sonar_num] = {
  NewPing(7, 5, max_distance),   // front 0
  NewPing(4, 2, max_distance),   // right 1
  NewPing(A3, A2, max_distance), // left 2
  //NewPing(10, 10, max_distance),   // right_back 3
  //NewPing(13, 13, max_distance), // left_back 4
};


int f_err = 0, old_err = 0;
double r_sens = 45, l_sens = 45; 
double angle = 100;

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
  //encoderSetup();
  delay(10000);
  pinMode(pwma_port, OUTPUT);
  pinMode(dira_port, OUTPUT);
  digitalWrite(dira_port, HIGH);
  analogWrite(pwma_port, 100);


}

void loop() {

  while (turns < 12) {
    camRead();
    Sensor_read();
    if(DEBUG){
        Serial.print("rf - ");
        Serial.print(cm[1]);
        Serial.print("  lf - ");
        Serial.print(cm[2]);
        Serial.print("  rb - ");
        Serial.print(cm[3]);
        Serial.print("  lb - ");
        Serial.print(cm[4]);
        Serial.print("     | r - ");
     
      //Serial.println();
    }
    clockwise = (cm[right] > 150);
    /* // Code for 5 sensors
    int l_max = (cm[left]>cm[left_back])?cm[left]:cm[left_back], l_min = (cm[left]<cm[left_back])?cm[left]:cm[left_back];
    int r_max = (cm[right]>cm[right_back])?cm[right]:cm[right_back], r_min = (cm[right]<cm[right_back])?cm[right]:cm[right_back];

    if(l_max = l_min){// left
      l_sens = l_min;
    } else {
      double dl = (l_min*13)/(l_max - l_min);
      l_sens = (dl*l_max)/sqrt((l_max*l_max)+((13+dl)*(13+dl)));
    }

    if(r_max = r_min){// left
      r_sens = r_min;
    } else {
      double dr = (r_min*13)/(r_max - r_min);
      r_sens = (dr*r_max)/sqrt((r_max*r_max)+((13+dr)*(13+dr)));
    }*/
     if(DEBUG){
      Serial.print(r_sens);
      Serial.print("  l - ");
      Serial.print(l_sens);
     }
    
    l_sens = (cm[left] > 100) ? 45 : cm[left];
    r_sens = (cm[right] > 100) ? 45 : cm[right];
    if ((cm[right] > 150 || cm[left] > 150) && turn_delay < millis()) {
      turns++;
      if (clockwise) r_sens = 45; else l_sens = 45;
      turn_delay = millis() + 3000;
      turn_check = true;
    }

    cube_shift = r_cube ? -5 : g_cube ? 5 : 0;
    angle = pid(l_sens - r_sens - cube_shift);
    if (DEBUG) {
      Serial.print("\t angle - ");
      Serial.println(angle);  
    }
    angle = angle > 30 ? 30 : angle;
    angle = angle < -30 ? -30 : angle;
    if (turn_check) {
      turn_time = millis() + 640;
      while (turn_time > millis()) myservo.write(100);
      turn_time = millis() + 520;
      while (turn_time > millis()) myservo.write((clockwise ? -50 : 50) + 100);
      turn_check = false;
    } else {
      myservo.write(angle + 100);
    }
  }

  digitalWrite(pwma_port, LOW);

}


void Sensor_read() {
  for (uint8_t i = 0; i < sonar_num; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += ping_interval * sonar_num;
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = sonar[currentSensor].ping_cm();
      read_count++;
    }
  }
}


double pid(int err) {
  p = err * kp;
  d = (err - old_err) * kd;
  i_err += err;
  i_err = i_err > max_error ? max_error : i_err;
  i_err = i_err < min_error ? min_error : i_err;
  i = i_err * ki;
  old_err = err;
  return p + i + d;
}


void camRead() {
  byte n = trackingCam.readBlobs(5); // read data about first 5 blobs
  bool cube_presence = false;
  int tmp_nearest_cube = -1;
  for (int i = 0; i < n; i++) { // finding nearest cube
    if (trackingCam.blob[i].type < 2) { //colors: red(1) & green(0)
      if (!cube_presence) {
        cube_presence = true;
        tmp_nearest_cube = i;
      } else if (trackingCam.blob[i].bottom > trackingCam.blob[tmp_nearest_cube].bottom) tmp_nearest_cube = i;
    }
  }
  if (cube_presence) {
    if (trackingCam.blob[tmp_nearest_cube].type == red) {
      r_cube = true;
      g_cube = false;
    }
    else if (trackingCam.blob[tmp_nearest_cube].type == green) {
      r_cube = false;
      g_cube = true;  
    } else {
      r_cube = false;
      g_cube = false;
    }
  }
}
