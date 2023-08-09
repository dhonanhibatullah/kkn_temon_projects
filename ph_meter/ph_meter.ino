#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "LSBitmaps.h"


#define OLED_ADDR               0x3c
#define SCREEN_WIDTH            128
#define SCREEN_HEIGHT           64
#define OLED_RESET              -1
#define PH_SENSOR_PIN           A0
#define COMP_FILTER_CONST       0.14
#define STD_DEV                 0.025
#define SPLASH_SCREEN_DURATION  3000


/* CALIBARTION DATA 
[Analog -> pH]
857.6   ->  4.01
760.5   ->  6.86
678.6   ->  9.18
*/


Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const float calib_data[2][3] = {
  {857.6, 760.5, 678.6},
  {4.01, 6.86, 9.18}
};

float acid_grad, 
      base_grad,
      prev_ph_val = 7.0,
      show_ph_val = 7.0;


void setTextConfig(int16_t cursor_x, int16_t cursor_y, uint8_t size, bool invert) {
  display.setCursor(cursor_x, cursor_y);
  display.setTextSize(size);
  if(invert)  display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  else        display.setTextColor(SH110X_WHITE);
}


void setup() {
  /* Begin OLED display */
  display.begin(OLED_ADDR, true);
  display.setTextColor(SH110X_WHITE);

  /* Set analog input pin */
  pinMode(PH_SENSOR_PIN, INPUT);

  /* Calculate gradients */
  acid_grad = (calib_data[1][0] - calib_data[1][1])/(calib_data[0][0] - calib_data[0][1]);
  base_grad = (calib_data[1][2] - calib_data[1][1])/(calib_data[0][2] - calib_data[0][1]);

  /* Splash screen */
  display.clearDisplay();
  display.drawBitmap(0, 0, kkn_temon_logo, 128, 64, SH110X_WHITE);
  setTextConfig(60, 20, 1, false); display.print("KKN-PPM UGM");
  setTextConfig(60, 35, 1, false); display.print("Teman Temon");
  display.display();
  delay(SPLASH_SCREEN_DURATION);
  display.clearDisplay();
}


void loop() {
  /* Read analog input value */
  float analog_val = (float)analogRead(PH_SENSOR_PIN);

  /* Convert the analog value to pH via calibration value */
  float ph_val  = (analog_val < calib_data[0][1]) ?
                  base_grad*(analog_val - calib_data[0][1]) + calib_data[1][1] :
                  acid_grad*(analog_val - calib_data[0][1]) + calib_data[1][1];

  /* Apply complementary and outlier filter */
  ph_val        = (COMP_FILTER_CONST)*ph_val + (1. - COMP_FILTER_CONST)*prev_ph_val;
  show_ph_val   = ((ph_val < show_ph_val - STD_DEV) || (ph_val > show_ph_val + STD_DEV)) ? ((ph_val < show_ph_val) ? ph_val + STD_DEV : ph_val - STD_DEV) : show_ph_val;
  prev_ph_val   = ph_val;

  /* Display the pH */
  display.clearDisplay();
  setTextConfig(12, 0, 1, false); display.print("[ pH Meter IPAH ]");
  setTextConfig(10, 23, 1, false); display.print("Nilai pH: ");
  setTextConfig(72, 19, 2, false); display.print(show_ph_val);

  /* Display the water's status */
  if(5.4 < show_ph_val && show_ph_val < 8.5) {
    setTextConfig(37, 45, 2, false); display.print("AMAN");
  }
  else {
    setTextConfig(26, 45, 2, false); display.print("BAHAYA");
  }
  display.display();
}