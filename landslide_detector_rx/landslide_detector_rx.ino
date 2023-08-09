#include <SPI.h>
#include <LoRa.h>
#include "LSDisplay.h"

#define BUZZER_PIN          1
#define LOW_BEEP_PERIOD     600
#define MEDIUM_BEEP_PERIOD  300
#define HIGH_BEEP_PERIOD    100
#define LORA_FREQUENCY      433E6
#define ESP8266_LORA_SS     15
#define ESP8266_LORA_RST    16
#define ESP8266_LORA_DIO0   2


/* Initialise objects and variables */
LSDisplay oled_display = LSDisplay();


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
  digitalWrite(BUZZER_PIN, HIGH);
  LoRa.setPins(ESP8266_LORA_SS, ESP8266_LORA_RST, ESP8266_LORA_DIO0);

  /* Begin OLED */
  oled_display.begin();

  /* Begin LoRa */
  LoRa.begin(LORA_FREQUENCY);
}


void loop() {
  oled_display.displayLoRaInfo(receiveLoRaPacket().c_str());
}
