#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <Arduino.h>

void sendStartupNotification();

void sendCycleNotification(String cycleStatus);

#endif