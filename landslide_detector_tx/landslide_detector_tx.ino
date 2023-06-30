#include "LSIMUController.h"
#include "LSLoraControllerTx.h"

#define BUZZER_PIN          3
#define BUILTIN_LED_PIN     13
#define LOW_BEEP_PERIOD     600
#define MEDIUM_BEEP_PERIOD  300
#define HIGH_BEEP_PERIOD    100


/* Initialise objects and variables */
LSIMUController     imu_controller  = LSIMUController();
LSLoraControllerTx  lora_controller = LSLoraControllerTx();

uint8_t       movement_level  = LSIMUController::NO_MOVEMENT;
unsigned int  beep_period_t   = 0;
unsigned long start_beep_t    = 0;


void periodicBeep(unsigned int beep_period, bool state) {
  static unsigned long  last_t    = 0;
  static bool           alarm     = false;
  unsigned long         current_t = millis();
  if(state) {
    if(current_t - last_t > beep_period/2) {
      alarm  = !alarm;
      last_t = current_t;
    }
    digitalWrite(BUZZER_PIN, alarm);
  }
  else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}


void setup() {
  /* Begin serial communication */
  Serial.begin(9600);

  /* Pinmoding */
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUILTIN_LED_PIN, OUTPUT);

  /* Begin IMU */
  imu_controller.begin();

  /* Begin Lora */
  lora_controller.begin();
}


void loop() {
  /* Read current IMU values */
  imu_controller.readSensorEvent();

  /* Detect land movement */
  movement_level = imu_controller.getLandslideInfo();
  
  /* Alarm system */
  switch(movement_level) {
    case LSIMUController::LOW_MOVEMENT:
      beep_period_t = LOW_BEEP_PERIOD;
      break;

    case LSIMUController::MEDIUM_MOVEMENT:
      beep_period_t = MEDIUM_BEEP_PERIOD;
      break;

    case LSIMUController::HIGH_MOVEMENT:
      beep_period_t = HIGH_BEEP_PERIOD;
      break;

    default:
      beep_period_t = 0;
      break;
  }
  Serial.println(movement_level);
  /* Beeping */
  periodicBeep(beep_period_t, imu_controller.getAlarmState());
}