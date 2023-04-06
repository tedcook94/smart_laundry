#include "accelerometer.h"
#include "adc.h"
#include "config.h"
#include "cycle.h"
#include "oled.h"
#include "wifi.h"
#include <Arduino.h>

void setup() {
  Serial.begin(115200);

  setupOled();
  setupConfig();
  setupAccelerometer();
  setupAdc();
  setupWifi();
}

void loop() {
  checkForConfigPortal();

  updateCycleStatus();
}