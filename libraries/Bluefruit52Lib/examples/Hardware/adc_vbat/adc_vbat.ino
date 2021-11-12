#include <Arduino.h>

#define VBAT_DIVIDER      (0.332888F)   // 1M + 0.499M voltage divider on VBAT
#define VBAT_DIVIDER_COMP (3.004008F)        // Compensation factor for the VBAT divider

int adcvalue = 0;
float mv_per_lsb = 3600.0F/1024.0F; // 10-bit ADC with 3.6V input range

void setup() {
  pinMode(VBAT_ENABLE, OUTPUT);
  digitalWrite(VBAT_ENABLE, LOW);
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb
}

void loop() {
  // Get a fresh ADC value
  adcvalue = analogRead(PIN_VBAT);

  // Display the results
  Serial.print(adcvalue);
  Serial.print(" [");
  Serial.print((float)adcvalue * mv_per_lsb*VBAT_DIVIDER_COMP);
  Serial.println(" mV]");

  delay(100);
}
