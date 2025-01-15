#include <Arduino.h>

#define AD_PIN 33

int i;
int n;
int mv_max, mv_min;
bool isMeasuring = false;  // 計測中フラグ

void setup() {
  Serial.begin(115200);
  pinMode(AD_PIN, INPUT);
  analogSetPinAttenuation(AD_PIN, ADC_11db);
  i = 0;
  n = 0;
  mv_max = 0;
  mv_min = 100000;
}

void loop() {
  if (isMeasuring) {
    i += 1;
    uint32_t analog_mv = analogReadMilliVolts(AD_PIN);
    if (n < 200) {
      if (mv_max < analog_mv) {
        mv_max = analog_mv;
      }
      if (mv_min > analog_mv) {
        mv_min = analog_mv;
      }

      if (i % 1000 == 0) {
        n++;
        int amplitude = (mv_max - mv_min) / 2;
        Serial.printf("%d,%4d,%4d,%4d\n", n, mv_max, mv_min, amplitude);
        mv_max = 0;
        mv_min = 100000;
      }
      delay(1);
    } else {
      isMeasuring = false;
      Serial.println("end");
    }
  } else {
    // 計測停止中の処理 (例: シリアルモニターから入力待ち)
    if (Serial.available() > 0) {
      char input = (char)Serial.read();
      if (input == 's') {  // 's'が入力されたら計測開始
        Serial.println("start");
        isMeasuring = true;
        n = 0;  // カウンタをリセット
        mv_max = 0;
        mv_min = 100000;
      }
    }
  }

  // 計測中でも'q'を受信したら計測を停止してリセット
  if (Serial.available() > 0) {
    char input = (char)Serial.read();
    if (input == 'q') {
      isMeasuring = false;
      Serial.println("reset");
      n = 0;
      mv_max = 0;
      mv_min = 100000;
    }
  }
}
