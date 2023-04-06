#include "wifi.h"
#include "config.h"
#include "oled.h"
#include <WiFiManager.h>


const char* MENU[] = {"wifi", "param", "sep", "info", "update", "sep", "exit"};
const String UNCHECKED_BOX_HTML = "type=\"checkbox\" onchange=\"this.value = this.checked ? 't' : ''\"",
    CHECKED_BOX_HTML = UNCHECKED_BOX_HTML + " checked",
    BR_HTML = "<br/>",
    HR_HTML = "<hr/>";
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
    motionThresholdParameter,
    motionStartDurationParameter,
    motionStopDurationParameter,
    currentEnabledParameter,
    currentThresholdParameter,
    currentStartDurationParameter,
    currentStopDurationParameter,
    startNotificationParameter,
    stopNotificationParameter,
    pushoverEnabledParameter,
    pushoverAppTokenParameter,
    pushoverUserTokenParameter,
    debugModeParameter;

void saveConfigCallback() {
  Serial.println("saveConfigCallback called");
  shouldSaveConfig = true;
}

void setupWifi() {
    pinMode(CONFIG_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    wm.setTitle("Smart Laundry");
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
        wifiConfig.motionEnabled ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&motionThresholdParameter) WiFiManagerParameter("motion_threshold", "Motion Threshold", 
        String(wifiConfig.motionThreshold).c_str(), 4);
    new (&motionStartDurationParameter) WiFiManagerParameter("motion_start_duration", "Motion Start Duration", 
        String(wifiConfig.motionStartDuration).c_str(), 4);
    new (&motionStopDurationParameter) WiFiManagerParameter("motion_stop_duration", "Motion Stop Duration", 
        String(wifiConfig.motionStopDuration).c_str(), 4);
    new (&currentEnabledParameter) WiFiManagerParameter("current_enabled", "Current Enabled?", 
        wifiConfig.currentEnabled ? "t" : "", 1, 
        wifiConfig.currentEnabled ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&currentThresholdParameter) WiFiManagerParameter("current_threshold", "Current Threshold", 
        String(wifiConfig.currentThreshold).c_str(), 4);
    new (&currentStartDurationParameter) WiFiManagerParameter("current_start_duration", "Current Start Duration", 
        String(wifiConfig.currentStartDuration).c_str(), 4);
    new (&currentStopDurationParameter) WiFiManagerParameter("current_stop_duration", "Current Stop Duration", 
        String(wifiConfig.currentStopDuration).c_str(), 4);
    new (&startNotificationParameter) WiFiManagerParameter("start_notification", "Start Notification", 
        wifiConfig.startNotification ? "t" : "", 1, 
        wifiConfig.startNotification ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&stopNotificationParameter) WiFiManagerParameter("stop_notification", "Stop Notification", 
        wifiConfig.stopNotification ? "t" : "", 1, 
        wifiConfig.stopNotification ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&pushoverEnabledParameter) WiFiManagerParameter("pushover_enabled", "Pushover Enabled?", 
        wifiConfig.pushoverEnabled ? "t" : "", 1, 
        wifiConfig.pushoverEnabled ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&pushoverAppTokenParameter) WiFiManagerParameter("pushover_app_token", "Pushover App Token", 
        wifiConfig.pushoverAppToken.c_str(), 30);
    new (&pushoverUserTokenParameter) WiFiManagerParameter("pushover_user_token", "Pushover User Token", 
        wifiConfig.pushoverUserToken.c_str(), 30);
    new (&debugModeParameter) WiFiManagerParameter("debug_mode", "Debug Mode", 
        wifiConfig.debugMode ? "t" : "", 1, 
        wifiConfig.debugMode ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    
    wm.addParameter(&deviceNameParameter);
    wm.addParameter(new WiFiManagerParameter(HR_HTML.c_str()));

    wm.addParameter(&motionEnabledParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(&motionThresholdParameter);
    wm.addParameter(&motionStartDurationParameter);
    wm.addParameter(&motionStopDurationParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(HR_HTML.c_str()));

    wm.addParameter(&currentEnabledParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(&currentThresholdParameter);
    wm.addParameter(&currentStartDurationParameter);
    wm.addParameter(&currentStopDurationParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(HR_HTML.c_str()));

    wm.addParameter(&startNotificationParameter);
    wm.addParameter(&stopNotificationParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(HR_HTML.c_str()));

    wm.addParameter(&pushoverEnabledParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(&pushoverAppTokenParameter);
    wm.addParameter(&pushoverUserTokenParameter);
    wm.addParameter(new WiFiManagerParameter(BR_HTML.c_str()));
    wm.addParameter(new WiFiManagerParameter(HR_HTML.c_str()));

    wm.addParameter(&debugModeParameter);

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
            String configModeMessages[] = {"Configuration mode", WiFi.localIP().toString()};
            writeToCenterOfOled(configModeMessages, 2);
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
        wifiConfig.motionThreshold = atof(motionThresholdParameter.getValue());
        wifiConfig.motionStartDuration = atoi(motionStartDurationParameter.getValue());
        wifiConfig.motionStopDuration = atoi(motionStopDurationParameter.getValue());
        wifiConfig.currentEnabled = strncmp(currentEnabledParameter.getValue(), "t", 1) == 0;
        wifiConfig.currentThreshold = atof(currentThresholdParameter.getValue());
        wifiConfig.currentStartDuration = atoi(currentStartDurationParameter.getValue());
        wifiConfig.currentStopDuration = atoi(currentStopDurationParameter.getValue());
        wifiConfig.startNotification = strncmp(startNotificationParameter.getValue(), "t", 1) == 0;
        wifiConfig.stopNotification = strncmp(stopNotificationParameter.getValue(), "t", 1) == 0;
        wifiConfig.pushoverEnabled = strncmp(pushoverEnabledParameter.getValue(), "t", 1) == 0;
        wifiConfig.pushoverAppToken = pushoverAppTokenParameter.getValue();
        wifiConfig.pushoverUserToken = pushoverUserTokenParameter.getValue();
        wifiConfig.debugMode = strncmp(debugModeParameter.getValue(), "t", 1) == 0;

        saveConfig(wifiConfig);
    }
}