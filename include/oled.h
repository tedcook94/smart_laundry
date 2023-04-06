#ifndef OLED_H
#define OLED_H
#include <Arduino.h>

void setupOled();

void writeSerialToOled(String message);

void writeSerialToOled(String message, bool printToSerial);

void writeToCenterOfOled(String message);

void writeToCenterOfOled(String message, bool printToSerial);

#endif