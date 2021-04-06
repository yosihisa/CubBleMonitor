//シフトポジションの読み取り
uint8_t read_shift() {
  uint8_t s;
  if (digitalRead(SHIFT_1_PIN) == LOW) {
    s = 1;
  } else if (digitalRead(SHIFT_2_PIN) == LOW) {
    s = 2;
  } else if (digitalRead(SHIFT_3_PIN) == LOW) {
    s = 3;
  } else if (digitalRead(SHIFT_4_PIN) == LOW) {
    s = 4;
  } else {
    s = 0;
  }
  return s;
}

//LEDの表示
void led_rgb(uint8_t r, uint8_t g, uint8_t b){
  ledcWrite(LED_R_CHANNEL, r);
  ledcWrite(LED_G_CHANNEL, g);
  ledcWrite(LED_B_CHANNEL, b);
}

//シフトインジケータの表示
void indicate_LED() {
  if(rpm < 8400) {
    if (shift == 0) led_rgb(  0,   0,   0);
    if (shift == 1) led_rgb(255,   0, 255);
    if (shift == 2) led_rgb(255, 255,   0);
    if (shift == 3) led_rgb(  0, 255,   0);
    if (shift == 4) {
        if(rpm >= 5300) led_rgb(  0, 200, 255);
        else            led_rgb(  0,   0, 255);
    }
  } else {
      led_rgb(255,   0,   0);
  }
}

//GNSS NMEAをカンマで分割するやつ
int split(String data, char delimiter, String *dst) {
  int index = 0;
  int arraySize = (sizeof(data) / sizeof((data)[0]));
  int datalength = data.length();
  for (int i = 0; i < datalength; i++) {
    char tmp = data.charAt(i);
    if ( tmp == delimiter ) {
      index++;
      if ( index > (arraySize - 1)) return -1;
    }
    else dst[index] += tmp;
  }
  return (index + 1);
}
