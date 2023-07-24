#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>


#define OLED_ADDR           0x3c
#define SCREEN_WIDTH        128
#define SCREEN_HEIGHT       64
#define OLED_RESET          -1
#define PH_SENSOR_PIN       A0
#define COMP_FILTER_CONST   0.14
#define REFRESH_DURATION    500


/* CALIBARTION DATA 
[Analog -> pH]

850.5   ->  4.01

753.5   ->  6.86

673     ->  9.18
*/


Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const float calib_data[2][3] = {
  {850.5, 753.5, 673},
  {4.01, 6.86, 9.18}
};

float acid_grad, 
      base_grad,
      prev_ph_val = 7.0;


void setup() {
  /* Begin OLED display */
  display.begin(OLED_ADDR, true);
  display.setTextColor(SH110X_WHITE);
  display.setTextSize(2);

  /* Set analog input pin */
  pinMode(PH_SENSOR_PIN, INPUT);

  /* Calculate gradients */
  acid_grad = (calib_data[1][0] - calib_data[1][1])/(calib_data[0][0] - calib_data[0][1]);
  base_grad = (calib_data[1][2] - calib_data[1][1])/(calib_data[0][2] - calib_data[0][1]);
}


void loop() {
  /* Read analog input value */
  float analog_val = (float)analogRead(PH_SENSOR_PIN);

  /* Convert the analog value to pH via calibration value */
  float ph_val  = (analog_val < calib_data[0][1]) ?
                  base_grad*(analog_val - calib_data[0][1]) + calib_data[1][1] :
                  acid_grad*(analog_val - calib_data[0][1]) + calib_data[1][1];

  /* Apply complementary filter */
  ph_val        = (COMP_FILTER_CONST)*ph_val + (1. - COMP_FILTER_CONST)*prev_ph_val;
  prev_ph_val   = ph_val;

  /* Display the pH */
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(ph_val);
  display.display();
  delay(REFRESH_DURATION);
}
