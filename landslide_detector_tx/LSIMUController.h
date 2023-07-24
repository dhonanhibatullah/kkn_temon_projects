#ifndef _LSIMUCONTROLLER_H_
#define _LSIMUCONTROLLER_H_

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define MPU_BEGIN_TRIAL         100
#define ACCEL_COMP_FILTER_CONST 0.3
#define GYRO_COMP_FILTER_CONST  0.3
#define COMB_COMP_FILTER_CONST  0.0
#define MODIFIED_GRAVITY_CONST  9.48
#define BEGIN_COUNT             1000

#define LOW_MOV_ACC 0.18
#define MED_MOV_ACC 0.35
#define HI_MOV_ACC  0.75

#define LOW_MOV_VEL 1.3
#define MED_MOV_VEL 2.8
#define HI_MOV_VEL  4.9

#define ALARM_STANDBY 3000


class LSIMUController {


  private:
    Adafruit_MPU6050  mpu_imu;
  
    sensors_event_t   accel_event,
                      gyro_event,
                      temp;

    float             accel[3]    = {0},
                      gyro[3]     = {0},
                      orn[3]      = {0};

    float             accel_orn_x = 0.01,
                      accel_orn_y = 0.01,
                      gyro_orn_x  = 0.01, 
                      gyro_orn_y  = 0.01, 
                      gyro_orn_z  = 0.01;

    float             abs_accel               = 0.01,
                      abs_vel                 = 0.01;

    unsigned long     elapsed_t     = 0,
                      prev_t        = 0,
                      move_start_t  = 0;

    uint8_t           last_movement_state = 0;

    bool              alarm_state   = false;

    float             calcCalibError();


  public:

    enum landslide_info{
      NO_MOVEMENT,
      LOW_MOVEMENT,
      MEDIUM_MOVEMENT,
      HIGH_MOVEMENT
    };

    LSIMUController();
    ~LSIMUController();

    bool begin();
    void readSensorEvent();

    float getAccelX();
    float getAccelY();
    float getAccelZ();
    float getGyroX();
    float getGyroY();
    float getGyroZ();
    float getOrnX();
    float getOrnY();
    float getOrnZ();
    float getAbsAccel();
    float getAbsVel();
    bool getAlarmState();

    uint8_t getLandslideInfo();
};


#endif