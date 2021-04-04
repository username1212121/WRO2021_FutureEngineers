void debugSetup() {
  Serial.begin(115200);
}

void debugAllBlobs() {
  Serial.print("debugAllObjects: ");
  Serial.println(cam_blobs_count);
  for(int i = 0; i < cam_blobs_count; i++) {
    Serial.print("Colour:");
    Serial.print(trackingCam.blob[i].type, DEC);
    Serial.print(" ?:");
    Serial.print(trackingCam.blob[i].dummy, DEC);
    Serial.print(" X:");
    Serial.print(trackingCam.blob[i].cx, DEC);
    Serial.print(" Y:");
    Serial.print(trackingCam.blob[i].cy, DEC);
    Serial.print(" Area:");
    Serial.print(trackingCam.blob[i].area, DEC);
    Serial.print(" Left:");
    Serial.print(trackingCam.blob[i].left, DEC);
    Serial.print(" Right:");
    Serial.print(trackingCam.blob[i].right, DEC);
    Serial.print(" Top:");
    Serial.print(trackingCam.blob[i].top, DEC);
    Serial.print(" Bottom:");
    Serial.print(trackingCam.blob[i].bottom, DEC);
    Serial.println(" ");
  }
}

void debugTurn(){
  if (turn) {
    Serial.print("debugDerection: ");
    if (clockwise) Serial.println("Clockwise");
    else Serial.println("Counterclockwise");
  } else {
    Serial.println("Not turning");
  }
}

void debugPid(float pid) {
  Serial.print("debugPid: ");
  Serial.print(pid_p);
  Serial.print(" - pid_p, ");
  Serial.print(pid_d);
  Serial.print(" - pid_d, ");
  Serial.print(pid_i);
  Serial.print(" - pid_i, ");
  Serial.print(pid);
  Serial.print(" - pid, ");
}

