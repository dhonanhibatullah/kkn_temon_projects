#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define LORA_FREQUENCY  433E6
#define LORA_PERIOD     1000
#define LORA_TEST_MSG   "HELLO WORLD!"
#define OLED_ADDR       0x3c
#define OLED_RESET      -1
#define SCREEN_WIDTH    128
#define SCREEN_HEIGHT   64
#define BUZZER_PIN      3

Adafruit_SH1106G display = Adafruit_SH1106G(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);
String lora_test_msg = LORA_TEST_MSG;


void setup() {
  /* Pinmoding */
  pinMode(BUZZER_PIN, OUTPUT);

  /* Begin display */
  display.begin(OLED_ADDR, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(1);

  /* Begin LoRa */
  while(!LoRa.begin(LORA_FREQUENCY)) {}
}

void loop() {
  /* Try to parse incoming packet */
  int     packet_size = LoRa.parsePacket();
  String  packet_msg  = "";

  if(packet_size) {
    /* Check while buffer exists */
    while(LoRa.available()) {
      packet_msg = packet_msg + (String)((char)LoRa.read());
    }

    /* Indicate the incoming message */
    digitalWrite(BUZZER_PIN, HIGH);
    display.setCursor(10, 20); display.print(packet_msg); display.display();
    delay(LORA_PERIOD/2);

    digitalWrite(BUZZER_PIN, LOW);
    display.clearDisplay(); display.display();
    delay(LORA_PERIOD/2);
  }
}
