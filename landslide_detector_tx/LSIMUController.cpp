#include "LSIMUController.h"


LSIMUController::LSIMUController() {}


LSIMUController::~LSIMUController() {}


bool LSIMUController::begin() {
  /* Initialize MPU */
  uint8_t try_cnt = 0;
  while(!mpu_imu.begin()) {
    if(try_cnt >= MPU_BEGIN_TRIAL) {
      return false;
    }
    ++try_cnt;
  }

  /* Set parameters */
  mpu_imu.setAccelerometerRange(MPU6050_RANGE_16_G);
  mpu_imu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu_imu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  return true;
}


void LSIMUController::readSensorEvent() {
  /* Get elapsed time */
  unsigned long current_t = millis();
  elapsed_t               = current_t - prev_t;
  prev_t                  = current_t;
  float f_elapsed_t       = ((float)elapsed_t)/1000.;

  /* Get sensor values */
  mpu_imu.getEvent(&accel_event, &gyro_event, &temp);

  /* Process the values */
  accel[0]  = accel[0]*ACCEL_COMP_FILTER_CONST + accel_event.acceleration.x*(1.0 - ACCEL_COMP_FILTER_CONST);
  accel[1]  = accel[1]*ACCEL_COMP_FILTER_CONST + accel_event.acceleration.y*(1.0 - ACCEL_COMP_FILTER_CONST);
  accel[2]  = accel[2]*ACCEL_COMP_FILTER_CONST + accel_event.acceleration.z*(1.0 - ACCEL_COMP_FILTER_CONST);
  gyro[0]   = gyro[0]*GYRO_COMP_FILTER_CONST + gyro_event.gyro.x*(1.0 - GYRO_COMP_FILTER_CONST);
  gyro[1]   = gyro[1]*GYRO_COMP_FILTER_CONST + gyro_event.gyro.y*(1.0 - GYRO_COMP_FILTER_CONST);
  gyro[2]   = gyro[2]*GYRO_COMP_FILTER_CONST + gyro_event.gyro.z*(1.0 - GYRO_COMP_FILTER_CONST);

  /* Calculate orientation */
  accel_orn_x = atan2(accel[1], sqrt(pow(accel[0], 2) + pow(accel[2], 2)));
  accel_orn_y = atan2(-1.0*accel[0], sqrt(pow(accel[1], 2) + pow(accel[2], 2)));
  gyro_orn_x  += gyro[0]*f_elapsed_t;
  gyro_orn_y  += gyro[1]*f_elapsed_t;
  gyro_orn_z  += gyro[2]*f_elapsed_t;
  orn[0]      = fmod(gyro_orn_x*COMB_COMP_FILTER_CONST + accel_orn_x*(1.0 - COMB_COMP_FILTER_CONST), M_PI_2);
  orn[1]      = fmod(gyro_orn_y*COMB_COMP_FILTER_CONST + accel_orn_y*(1.0 - COMB_COMP_FILTER_CONST), M_PI_2);
  orn[2]      = fmod(gyro_orn_z, M_PI_2);

  /* Calculate absolute acceleration and velocity */
  abs_accel = sqrt(pow(accel[0], 2) + pow(accel[1], 2) + pow(accel[2], 2));
  abs_vel   = sqrt(pow(gyro[0], 2) + pow(gyro[1], 2) + pow(gyro[2], 2));
}


float LSIMUController::getAccelX() {
  return accel[0];
}


float LSIMUController::getAccelY() {
  return accel[1];
}


float LSIMUController::getAccelZ() {
  return accel[2];
}


float LSIMUController::getGyroX() {
  return gyro[0];
}


float LSIMUController::getGyroY() {
  return gyro[1];
}


float LSIMUController::getGyroZ() {
  return gyro[2];
}


float LSIMUController::getOrnX() {
  return orn[0];
}


float LSIMUController::getOrnY() {
  return orn[1];
}


float LSIMUController::getOrnZ() {
  return orn[2];
}


float LSIMUController::getAbsAccel() {
  return abs_accel;
}


float LSIMUController::getAbsVel() {
  return abs_vel;
}


bool LSIMUController::getAlarmState() {
  return alarm_state;
}

/* 
** POS_PITCH_ERR
    0.30 -> 0.13
    0.60 -> 0.20
    1.00 -> 0.23

** NEG_PITCH_ERR
    -0.30 -> 0.13
    -0.60 -> 0.28
    -1.00 -> 0.50

** POS_ROLL_ERR
    0.30 -> NEGL
    0.60 -> NEGL
    1.00 -> NEGL

** NEG_ROLL_ERR
    -0.30 -> NEGL
    -0.60 -> 0.1
    -1.00 -> 0.22
*/
float LSIMUController::calcCalibError() {
  /* Calibration data */
  static const float test_step[3] = {0.3, 0.6, 1.0},
  roll_err[2][3] = {
    {0.0, 0.0, 0.0},
    {0.1, 0.13, 0.24}
  },
  pitch_err[2][3] = {
    {0.11, 0.18, 0.20},
    {0.17, 0.28, 0.50}
  };

  /* Find roll and pitch interval */
  uint8_t roll_sign   = (orn[1] > 0.0) ? 0 : 1,
          pitch_sign  = (orn[0] > 0.0) ? 0 : 1;
  float   roll_val    = fabs(orn[0]),
          pitch_val   = fabs(orn[1]),
          err         = 0.0;

  /* Return roll and pitch error */
  if(roll_val < test_step[0]) {
    err += (roll_val/test_step[0])*roll_err[roll_sign][0];
  }
  else if(roll_val < test_step[1]) {
    err += ((roll_val - test_step[0])/(test_step[1] - test_step[0]))*(roll_err[roll_sign][1] - roll_err[roll_sign][0]) + roll_err[roll_sign][0];
  }
  else if(roll_val < test_step[2]) {
    err += ((roll_val - test_step[1])/(test_step[2] - test_step[1]))*(roll_err[roll_sign][2] - roll_err[roll_sign][1]) + roll_err[roll_sign][1];
  }
  else {
    err += roll_err[roll_sign][2];
  }

  if(pitch_val < test_step[0]) {
    err += (pitch_val/test_step[0])*pitch_err[pitch_sign][0];
  }
  else if(pitch_val < test_step[1]) {
    err += ((pitch_val - test_step[0])/(test_step[1] - test_step[0]))*(pitch_err[pitch_sign][1] - pitch_err[pitch_sign][0]) + pitch_err[pitch_sign][0];
  }
  else if(pitch_val < test_step[2]) {
    err += ((pitch_val - test_step[1])/(test_step[2] - test_step[1]))*(pitch_err[pitch_sign][2] - pitch_err[pitch_sign][1]) + pitch_err[pitch_sign][1];
  }
  else {
    err += pitch_err[pitch_sign][2];
  }

  return err;
}


uint8_t LSIMUController::getLandslideInfo() {
  float   accel_diff  = fabs(fabs(MODIFIED_GRAVITY_CONST - abs_accel) - calcCalibError()),
          vel_diff    = abs_vel;
  uint8_t accel_arg, vel_arg, res;

  // Serial.print("roll       : "); Serial.println(orn[0]);
  // Serial.print("pitch      : "); Serial.println(orn[1]);
  // Serial.print("accel_diff : "); Serial.println(accel_diff);
  // Serial.print("vel_diff   : "); Serial.println(vel_diff);

  /* Determine acceleration-based movement level */
  if(accel_diff < LOW_MOV_ACC)      accel_arg = NO_MOVEMENT;
  else if(accel_diff < MED_MOV_ACC) accel_arg = LOW_MOVEMENT;
  else if(accel_diff < HI_MOV_ACC)  accel_arg = MEDIUM_MOVEMENT;
  else                              accel_arg = HIGH_MOVEMENT;
  
  /* Determine velocity-based movement level */
  if(vel_diff < LOW_MOV_VEL)      vel_arg = NO_MOVEMENT;
  else if(vel_diff < MED_MOV_VEL) vel_arg = LOW_MOVEMENT;
  else if(vel_diff < HI_MOV_VEL)  vel_arg = MEDIUM_MOVEMENT;
  else                            vel_arg = HIGH_MOVEMENT;

  /* Result */
  if(millis() < BEGIN_COUNT) return NO_MOVEMENT;
  res = (accel_arg > vel_arg) ? accel_arg : vel_arg;
  last_movement_state = (res > last_movement_state) ? res : last_movement_state;
  if(res != NO_MOVEMENT) {
    move_start_t = millis();
    alarm_state  = true;
  }
  else if(millis() - move_start_t > ALARM_STANDBY) {
    alarm_state = false;
    last_movement_state = NO_MOVEMENT;
  }
  return last_movement_state;
}