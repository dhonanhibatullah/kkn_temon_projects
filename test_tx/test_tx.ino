#include <SPI.h>
#include <LoRa.h>

#define LORA_FREQUENCY  433E6
#define LORA_PERIOD     1500
#define LORA_TEST_MSG   "HELLO WORLD!"
#define BUZZER_PIN      3


void setup() {
  /* Pinmoding */
  pinMode(BUZZER_PIN, OUTPUT);

  /* Begin LoRa */
  while(!LoRa.begin(LORA_FREQUENCY)) {}
}


void loop() {
  /* Send LoRa packet */
  LoRa.beginPacket();
  LoRa.print(LORA_TEST_MSG);
  LoRa.endPacket();

  /* Wait for 1500ms */
  delay(LORA_PERIOD);
}