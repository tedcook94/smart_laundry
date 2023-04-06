#include "wifi.h"
#include "config.h"
#include "oled.h"
#include <WiFiManager.h>


const char* MENU[] = {"wifi", "param", "sep", "info", "update", "sep", "exit"};
const char CHECKED_BOX_HTML[] = "type=\"checkbox\" onchange=\"this.value = this.checked ? 't' : ''\" checked",
    UNCHECKED_BOX_HTML[] = "type=\"checkbox\" onchange=\"this.value = this.checked ? 't' : ''\"",
    BR_HTML[] = "<br/>";
const int CONFIG_PIN = 4,
    CONFIG_WAIT_DURATION = 5000;

WiFiManager wm;
struct config wifiConfig;

bool shouldSaveConfig = false, 
    configButtonPushed = false;
long configStartMillis;
String wmName;
WiFiManagerParameter deviceNameParameter, 
    motionEnabledParameter, 
    currentEnabledParameter,
    pushoverAppTokenParameter,
    pushoverUserTokenParameter;

void saveConfigCallback() {
  Serial.println("saveConfigCallback called");
  shouldSaveConfig = true;
}

void setupWifi() {
    pinMode(CONFIG_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    wm.setTitle("SmartLaundry");
    wm.setMenu(MENU, 7);
    wm.setDarkMode(true);
    wm.setDebugOutput(false);
    wm.setConfigPortalTimeout(300);
    wm.setSaveConfigCallback(saveConfigCallback);
    wm.setPreSaveConfigCallback(saveConfigCallback);
    wm.setPreSaveParamsCallback(saveConfigCallback);

    wifiConfig = getConfig();

    new (&deviceNameParameter) WiFiManagerParameter("device_name", "Device Name", 
        wifiConfig.deviceName.c_str(), 20);
    new (&motionEnabledParameter) WiFiManagerParameter("motion_enabled", "Motion Enabled?", 
        wifiConfig.motionEnabled ? "t" : "", 1, 
        wifiConfig.motionEnabled ? CHECKED_BOX_HTML : UNCHECKED_BOX_HTML, WFM_LABEL_AFTER);
    new (&currentEnabledParameter) WiFiManagerParameter("current_enabled", "Current Enabled?", 
        wifiConfig.currentEnabled ? "t" : "", 1, 
        wifiConfig.currentEnabled ? CHECKED_BOX_HTML : UNCHECKED_BOX_HTML, WFM_LABEL_AFTER);
    new (&pushoverAppTokenParameter) WiFiManagerParameter("pushover_app_token", "Pushover App Token", 
        wifiConfig.pushoverAppToken.c_str(), 30);
    new (&pushoverUserTokenParameter) WiFiManagerParameter("pushover_user_token", "Pushover User Token", 
        wifiConfig.pushoverUserToken.c_str(), 30);
    
    wm.addParameter(&deviceNameParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML));
    wm.addParameter(&motionEnabledParameter);
    wm.addParameter(&currentEnabledParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML));
    wm.addParameter(new WiFiManagerParameter(BR_HTML));
    wm.addParameter(&pushoverAppTokenParameter);
    wm.addParameter(&pushoverUserTokenParameter);

    writeSerialToOled("Connecting to wifi...");
    wmName = "SmartLaundry_" + (wifiConfig.deviceName.length() == 0 ? WiFi.macAddress() : wifiConfig.deviceName);
    if (!wm.autoConnect(wmName.c_str())) {
        writeToCenterOfOled("Wifi failed");
        wm.reboot();
    } else {
        writeSerialToOled("Wifi connected");
    }

    saveConfigFile();
}

void checkForConfigPortal() {
    if (digitalRead(CONFIG_PIN) == LOW) {
        if (!configButtonPushed) {
            configStartMillis = millis();
            configButtonPushed = true;
        } else if (millis() >= configStartMillis + CONFIG_WAIT_DURATION) {
            writeToCenterOfOled("Configuration mode");
            wm.startConfigPortal(wmName.c_str());
            saveConfigFile();
        }
    } else {
        configButtonPushed = false;
    }
}

void saveConfigFile() {
    if (shouldSaveConfig) {
        wifiConfig.deviceName = deviceNameParameter.getValue();
        wifiConfig.motionEnabled = strncmp(motionEnabledParameter.getValue(), "t", 1) == 0;
        wifiConfig.currentEnabled = strncmp(currentEnabledParameter.getValue(), "t", 1) == 0;
        wifiConfig.pushoverAppToken = pushoverAppTokenParameter.getValue();
        wifiConfig.pushoverUserToken = pushoverUserTokenParameter.getValue();

        saveConfig(wifiConfig);
    }
}