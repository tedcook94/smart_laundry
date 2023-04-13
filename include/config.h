#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

struct config {
    String deviceName = "";
    bool motionEnabled = false;
    float motionThreshold = 15.0;
    int motionStartDuration = 15;
    int motionStopDuration = 180;
    bool currentEnabled = false;
    float currentThreshold = 1.0;
    int currentStartDuration = 0;
    int currentStopDuration = 180;
    bool startupNotification = true;
    bool cycleStartNotification = true;
    bool cycleStopNotification = true;
    bool pushoverEnabled = false;
    String pushoverAppToken = "";
    String pushoverUserToken = "";
    bool debugMode = false;
};

void setupConfig();

struct config getConfig();

void saveConfig(struct config config);

#endif