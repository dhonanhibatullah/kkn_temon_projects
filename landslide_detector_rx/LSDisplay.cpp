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


void LSDisplay::periodicBeep(unsigned int beep_period, bool state) {
  static unsigned long  last_t    = 0;
  static bool           alarm     = true;
  unsigned long         current_t = millis();
  if(state) {
    if(current_t - last_t > beep_period/2) {
      alarm  = !alarm;
      last_t = current_t;
    }
    digitalWrite(BUZZER_PIN, alarm);
  }
  else {
    digitalWrite(BUZZER_PIN, HIGH);
  }
}


void LSDisplay::displayLoRaInfo(const char* str) {
  static int            last_movement_level = 0;
  static unsigned long  last_display_t = 0;

  /* Get the informations */
  String  lora_info       = str;
  int     at_idx          = lora_info.indexOf('@'); 

  /* Periodic beep sound */
  switch(last_movement_level) {
    case 0:
      digitalWrite(BUZZER_PIN, LOW);
      break;

    case 1:
      this->periodicBeep(LOW_BEEP_PERIOD, true);
      break;

    case 2:
      this->periodicBeep(MEDIUM_BEEP_PERIOD, true);
      break;

    case 3:
      this->periodicBeep(HIGH_BEEP_PERIOD, true);
      break;

    default:
      this->periodicBeep(HIGH_BEEP_PERIOD, true);
      break;
  }

  /* Reset */
  if(at_idx == -1) {
    if(last_movement_level != 0 && (millis() - last_display_t >= ALARM_STANDBY)) {
      last_movement_level = 0;
      digitalWrite(BUZZER_PIN, LOW);
      display.clearDisplay();
      this->idleDisplay();
    }
    return;
  }

  /* Categorize */
  int     movement_level  = lora_info.substring(0, at_idx).toInt();
  String  msg_id          = lora_info.substring(at_idx+1),
          level;
  switch(movement_level) {
    case 1:
      level = "1-KECIL";
      break;
    case 2:
      level = "2-SIAGA";
      break;
    case 3:
      level = "3-BAHAYA";
      break;
    default:
      level = "UNKNOWN";
      break;
  }

  /* Process movement level */
  last_movement_level = movement_level;
  last_display_t      = millis();

  /* Display! */
  display.clearDisplay();
  this->setTextConfig(0, 10, 1, false);
  display.print("PERGESERAN TERDETEKSI");
  this->setTextConfig(0, 28, 1, false);
  display.print((String)">> LOKASI: " + msg_id);
  this->setTextConfig(0, 46, 1, false);
  display.print((String)">> LEVEL : " + level);
  display.display();
}