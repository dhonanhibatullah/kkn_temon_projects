#ifndef _LSDISPLAY_H_
#define _LSDISPLAY_H_

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "LSBitmaps.h"

#define OLED_ADDR               0x3c
#define OLED_RESET              -1
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64
#define SPLASH_SCREEN_DURATION  2000

#define ALARM_STANDBY           3000
#define LOW_BEEP_PERIOD         600
#define MEDIUM_BEEP_PERIOD      300
#define HIGH_BEEP_PERIOD        100

#define BUZZER_PIN  3


class LSDisplay {


  private:
    Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


  public:
    LSDisplay();
    ~LSDisplay();

    void begin();
    void setTextConfig(int16_t cursor_x, int16_t cursor_y, uint8_t size, bool invert);
    void idleDisplay();
    void periodicBeep(unsigned int beep_period, bool state);
    void displayLoRaInfo(const char* str);
};


#endif