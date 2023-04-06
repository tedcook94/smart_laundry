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
        if (json.containsKey("currentEnabled")) {
            configStruct.currentEnabled = json["currentEnabled"].as<bool>();
        }
        if (json.containsKey("pushoverAppToken")) {
            configStruct.pushoverAppToken = json["pushoverAppToken"].as<String>();
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
    json["currentEnabled"] = config.currentEnabled;
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