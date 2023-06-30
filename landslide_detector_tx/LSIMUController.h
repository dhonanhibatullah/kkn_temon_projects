#ifndef _LSIMUCONTROLLER_H_
#define _LSIMUCONTROLLER_H_

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define MPU_BEGIN_TRIAL         100
#define ACCEL_COMP_FILTER_CONST 0.15
#define GYRO_COMP_FILTER_CONST  0.15
#define COMB_COMP_FILTER_CONST  0.72
#define MODIFIED_GRAVITY_CONST  9.48

#define LOW_MOV_ACC 0.25
#define MED_MOV_ACC 0.42
#define HI_MOV_ACC  0.74

#define LOW_MOV_VEL 1.6
#define MED_MOV_VEL 3.0
#define HI_MOV_VEL  5.1

#define ALARM_STANDBY 2000


class LSIMUController {


  private:
    Adafruit_MPU6050  mpu_imu;
  
    sensors_event_t   accel_event,
                      gyro_event,
                      temp;

    float             accel[3]    = {0},
                      gyro[3]     = {0},
                      orn[3]      = {0};

    float             accel_orn_x = 0,
                      accel_orn_y = 0,
                      gyro_orn_x  = 0, 
                      gyro_orn_y  = 0, 
                      gyro_orn_z  = 0;

    float             abs_accel               = 0,
                      abs_vel                 = 0;

    unsigned long     elapsed_t     = 0,
                      prev_t        = 0,
                      move_start_t  = 0;

    uint8_t           last_movement_state = 0;

    bool              alarm_state = false;


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