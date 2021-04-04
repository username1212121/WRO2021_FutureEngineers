void camSetup() {
/*TrackingCamDxlUartInit(CAM_ID, Serial_port, CAM_BaudRate, PC_BaudRate, TrackingCamTimeout);
 * CAM_ID - default 51
 * Serial_port (10\11) - 1
 * CAM_BaudRate - default 115200
 * PC_BaudRate - default 115200
 * TrackingCamTimeout - in ms - default 30  */
  trackingCam.TrackingCamDxlUartInit(51, 1, 19200, 19200, 30);
}

void camRead() {
  cam_blobs_count = trackingCam.TrackingCamDxl_ReadBlobs();
  if (DEBUG) debugAllBlobs();
}


void camDetectTurn() {
  bool see_yellow_line = false;
  int yellow_line_cy = 0;
  bool see_blue_line = false;
  int blue_line_cy = 0;
  for(int i = 0; i < cam_blobs_count; i++) {
    if (trackingCam.blob[i].type == 0) {
      yellow_line_cy = trackingCam.blob[i].cy;
      see_yellow_line = true;
    }
    if (trackingCam.blob[i].type == 1) {
      blue_line_cy = trackingCam.blob[i].cy;
      see_blue_line = true;
    }
  }
  turn = see_yellow_line * see_blue_line;
  if (turn) {
    if (yellow_line_cy > blue_line_cy) clockwise = true;
    else clockwise = false;
  }
  if (DEBUG) debugTurn();
}

