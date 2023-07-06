#include "LSDisplay.h"


LSDisplay::LSDisplay() {}


LSDisplay::~LSDisplay() {}


void LSDisplay::begin() {
  /* Begin OLED SH1106 */
  display.begin(OLED_ADDR, true);
  display.clearDisplay();

  /* Splash screen */
  display.drawBitmap(0, 0, kkn_temon_logo, 128, 64, SH110X_WHITE);
  this->setTextConfig(60, 20, 1, false); display.print("KKN-PPM UGM");
  this->setTextConfig(60, 35, 1, false); display.print("Teman Temon");
  display.display();
  delay(SPLASH_SCREEN_DURATION);
  display.clearDisplay();

  this->idleDisplay();
  delay(SPLASH_SCREEN_DURATION);
  display.clearDisplay();
}


void LSDisplay::setTextConfig(int16_t cursor_x, int16_t cursor_y, uint8_t size, bool invert) {
  display.setCursor(cursor_x, cursor_y);
  display.setTextSize(size);
  if(invert)  display.setTextColor(SH110X_BLACK, SH110X_WHITE);
  else        display.setTextColor(SH110X_WHITE);
}


void LSDisplay::idleDisplay() {
  this->setTextConfig(34, 17, 1, false); display.print("EWS Longsor");
  this->setTextConfig(5, 35, 2, false); display.print("Desa Temon");
  display.display();
}


void LSDisplay::displayLoRaInfo(const char* str) {
  /* Get the informations */
  String  lora_info       = str;
  int     at_idx          = lora_info.indexOf('@'); if(at_idx == -1) return;
  int     movement_level  = lora_info.substring(0, at_idx).toInt();
  String  msg_id          = lora_info.substring(at_idx+1),
          level;
  switch(movement_level) {
    case 1:
      level = "1-SLIGHT";
      break;
    case 2:
      level = "2-WARNING";
      break;
    case 3:
      level = "3-DANGER";
      break;
    default:
      level = "UNKNOWN";
      break;
  }

  /* Display! */
  this->setTextConfig(0, 0, 1, false);
  display.print("PERGESERAN TERDETEKSI");
  this->setTextConfig(0, 18, 1, false);
  display.print((String)">> LOKASI: " + msg_id);
  this->setTextConfig(0, 36, 1, false);
  display.print((String)">> LEVEL : " + level);
  display.display();
}