#include <SPI.h>
#include <LoRa.h>
#include "LSDisplay.h"

#define BUZZER_PIN          3
#define BUILTIN_LED_PIN     13
#define LOW_BEEP_PERIOD     600
#define MEDIUM_BEEP_PERIOD  300
#define HIGH_BEEP_PERIOD    100
#define LORA_FREQUENCY      433E6
#define ALARM_STANDBY       60000


/* Initialise objects and variables */
LSDisplay oled_display = LSDisplay();


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


String receiveLoRaPacket() {
  /* Try to parse packet */
  String  packet_msg  = "";
  int     packet_size = LoRa.parsePacket();
  if(packet_size) {
    while(LoRa.available()) {
      packet_msg = packet_msg + (String)((char)LoRa.read());
    }
  }
  return packet_msg;
}


void setup() {
  /* Pinmoding */
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(BUILTIN_LED_PIN, OUTPUT);

  /* Begin OLED */
  oled_display.begin();

  /* Begin LoRa */
  LoRa.begin(LORA_FREQUENCY);
}


void loop() {
  oled_display.displayLoRaInfo(receiveLoRaPacket().c_str());
}
