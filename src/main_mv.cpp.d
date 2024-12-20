#include <Arduino.h>

#define AD_PIN 33

void setup()
{
  Serial.begin(115200);
  pinMode(AD_PIN, INPUT);
  analogSetPinAttenuation(AD_PIN, ADC_11db);
}

void loop()
{
  uint16_t analog_adc = analogRead(AD_PIN);
  uint32_t analog_mv = analogReadMilliVolts(AD_PIN);
  // Serial.printf("analog_adc=%4d, analog_mv=%4d\n", analog_adc, analog_mv);
  Serial.println(analog_mv);
  delay(1);
}
