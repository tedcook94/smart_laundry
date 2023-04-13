#include "config.h"
#include "oled.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

const String CONFIG_FILE_LOCATION = "/config.json";

DynamicJsonDocument json(1024);
struct config configStruct;

bool configLoaded, 
    configCached = false;

void setupConfig() {
    writeSerialToOled("Loading config...");
    if (SPIFFS.begin(true)) {
        if (SPIFFS.exists(CONFIG_FILE_LOCATION)) {
            File configFile = SPIFFS.open(CONFIG_FILE_LOCATION, "r");
            if (configFile) {
                DeserializationError deserializeError = deserializeJson(json, configFile);
                if (!deserializeError) {
                    writeSerialToOled("Config loaded");
                    configLoaded = true;
                    Serial.println("JSON parsed from config file:");
                    serializeJsonPretty(json, Serial);
                    Serial.print("\n");
                } else {
                    Serial.println("Failed to load JSON");
                    configLoaded = false;
                }
                configFile.close();
            } else {
                Serial.println("Failed to open config file");
            }
        } else {
            Serial.println("Failed to find config file");
        }
    } else {
        Serial.println("Failed to mount file system");
    }
}

struct config getConfig() {
    if (configLoaded && !configCached) {
        if (json.containsKey("deviceName")) {
            configStruct.deviceName = json["deviceName"].as<String>();
        }
        if (json.containsKey("motionEnabled")) {
            configStruct.motionEnabled = json["motionEnabled"].as<bool>();
        }
        if (json.containsKey("motionThreshold")) {
            configStruct.motionThreshold = json["motionThreshold"].as<float>();
        }
        if (json.containsKey("motionStartDuration")) {
            configStruct.motionStartDuration = json["motionStartDuration"].as<int>();
        }
        if (json.containsKey("motionStopDuration")) {
            configStruct.motionStopDuration = json["motionStopDuration"].as<int>();
        }
        if (json.containsKey("currentEnabled")) {
            configStruct.currentEnabled = json["currentEnabled"].as<bool>();
        }
        if (json.containsKey("currentThreshold")) {
            configStruct.currentThreshold = json["currentThreshold"].as<float>();
        }
        if (json.containsKey("currentStartDuration")) {
            configStruct.currentStartDuration = json["currentStartDuration"].as<int>();
        }
        if (json.containsKey("currentStopDuration")) {
            configStruct.currentStopDuration = json["currentStopDuration"].as<int>();
        }
        if (json.containsKey("pushoverAppToken")) {
            configStruct.pushoverAppToken = json["pushoverAppToken"].as<String>();
        }
        if (json.containsKey("startupNotification")) {
            configStruct.startupNotification = json["startupNotification"].as<bool>();
        }
        if (json.containsKey("cycleStartNotification")) {
            configStruct.cycleStartNotification = json["cycleStartNotification"].as<bool>();
        }
        if (json.containsKey("cycleStopNotification")) {
            configStruct.cycleStopNotification = json["cycleStopNotification"].as<bool>();
        }
        if (json.containsKey("pushoverEnabled")) {
            configStruct.pushoverEnabled = json["pushoverEnabled"].as<bool>();
        }
        if (json.containsKey("pushoverUserToken")) {
            configStruct.pushoverUserToken = json["pushoverUserToken"].as<String>();
        }
        if (json.containsKey("debugMode")) {
            configStruct.debugMode = json["debugMode"].as<bool>();
        }

        configCached = true;
    }

    return configStruct;
}

void saveConfig(struct config config) {
    writeToCenterOfOled("Saving config...");

    DynamicJsonDocument json(1024);
    json["deviceName"] = config.deviceName;
    json["motionEnabled"] = config.motionEnabled;
    json["motionThreshold"] = config.motionThreshold;
    json["motionStartDuration"] = config.motionStartDuration;
    json["motionStopDuration"] = config.motionStopDuration;
    json["currentEnabled"] = config.currentEnabled;
    json["currentThreshold"] = config.currentThreshold;
    json["currentStartDuration"] = config.currentStartDuration;
    json["motionStopDuration"] = config.motionStopDuration;
    json["startupNotification"] = config.startupNotification;
    json["cycleStartNotification"] = config.cycleStartNotification;
    json["cycleStopNotification"] = config.cycleStopNotification;
    json["pushoverEnabled"] = config.pushoverEnabled;
    json["pushoverAppToken"] = config.pushoverAppToken;
    json["pushoverUserToken"] = config.pushoverUserToken;
    json["debugMode"] = config.debugMode;

    File configFile = SPIFFS.open(CONFIG_FILE_LOCATION, "w");
    if (!configFile) {
        Serial.println("Failed to open config file for writing");
    } else {
        serializeJsonPretty(json, Serial);
        Serial.println("\n");
        serializeJsonPretty(json, configFile);
    }
    configFile.close();

    writeToCenterOfOled("Rebooting...");
    ESP.restart();
}