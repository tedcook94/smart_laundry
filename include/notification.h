#ifndef NOTIFICATION_H
#define NOTIFICATION_H
#include <Arduino.h>

void sendStartupNotification();

void sendCycleNotification(String cycleStatus);

void sendDebugCycleNotification(
    String cycleStatus, String currentReadout, bool currentDetected, 
    String accelerometerReadout, bool motionDetected);

#endif