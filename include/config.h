#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

struct config {
    String deviceName = "";
    bool motionEnabled = false;
    bool currentEnabled = false;
    String pushoverAppToken = "";
    String pushoverUserToken = "";
};

void setupConfig();

struct config getConfig();

void saveConfig(struct config config);

#endif