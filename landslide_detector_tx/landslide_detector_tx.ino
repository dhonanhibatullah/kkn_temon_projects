#include <SPI.h>
#include <LoRa.h>
#include "LSIMUController.h"

#define BUZZER_PIN          3
#define BUILTIN_LED_PIN     13
#define LOW_BEEP_PERIOD     600
#define MEDIUM_BEEP_PERIOD  300
#define HIGH_BEEP_PERIOD    100
#define LORA_FREQUENCY      433E6
#define LORA_PERIOD         1500
#define DEVICE_ID           "@TEMON-1"


/* Initialise objects and variables */
LSIMUController     imu_controller  = LSIMUController();
uint8_t             movement_level  = LSIMUController::NO_MOVEMENT;
unsigned int        beep_period_t   = 0;
unsigned long       lora_period_t   = 0;


/* Function(s) */
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
  /* Pinmoding */
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUILTIN_LED_PIN, OUTPUT);

  /* Begin IMU */
  imu_controller.begin();

  /* Begin Lora */
  LoRa.begin(LORA_FREQUENCY);
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

  /* Beeping */
  periodicBeep(beep_period_t, imu_controller.getAlarmState());

  /* LoRa send */
  unsigned long lora_time_diff = millis() - lora_period_t;
  if(lora_time_diff > LORA_PERIOD) {
    lora_period_t += lora_time_diff;
    if(movement_level != LSIMUController::NO_MOVEMENT) {
      LoRa.beginPacket();
      LoRa.print(movement_level);
      LoRa.print(DEVICE_ID);
      LoRa.endPacket();
    }
  }
}