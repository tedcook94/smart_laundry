#include "wifi.h"
#include "config.h"
#include "notification.h"
#include "oled.h"
#include <WiFiManager.h>

#ifdef ESP32DEV
#define CONFIG_PIN 4
#endif

#ifdef QTPY
#define CONFIG_PIN 18
#endif

const char* MENU[] = {"wifi", "param", "sep", "info", "update", "sep", "exit"};
const char* STYLE_CSS = "<style>" \
                        "   #device_name, #motion_stop_duration, #current_stop_duration, #cycle_stop_notification, #pushover_user_token, #twilio_to_numbers" \
                        "       { margin-bottom: 25px; }" \
                        "   #motion_enabled, #current_enabled, #pushover_enabled, #twilio_enabled" \
                        "       { margin-bottom: 10px; }" \
                        "   label[for='motion_enabled']::after, label[for='current_enabled']::after, label[for='pushover_enabled']::after, label[for='twilio_enabled']::after" \
                        "       { content: ''; display: block; }" \
                        "</style>";
const String UNCHECKED_BOX_HTML = "type=\"checkbox\" onchange=\"this.value = this.checked ? 't' : ''\"",
    CHECKED_BOX_HTML = UNCHECKED_BOX_HTML + " checked";
const int CONFIG_WAIT_DURATION = 5000;

WiFiManager wm;
struct config wifiConfig;

bool configButtonPushed = false;
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
    startupNotificationParameter,
    cycleStartNotificationParameter,
    cycleStopNotificationParameter,
    pushoverEnabledParameter,
    pushoverAppTokenParameter,
    pushoverUserTokenParameter,
    twilioEnabledParameter,
    twilioAccountSidParameter,
    twilioAuthTokenParameter,
    twilioFromNumberParameter,
    twilioToNumbersParameter,
    debugModeParameter;

void setupWifi() {
    pinMode(CONFIG_PIN, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    wm.setTitle("Smart Laundry");
    wm.setMenu(MENU, 7);
    wm.setDarkMode(true);
    wm.setCustomHeadElement(STYLE_CSS);
    wm.setDebugOutput(false);
    wm.setConfigPortalTimeout(300);
    wm.setSaveConfigCallback(saveWifiConfig);
    wm.setSaveParamsCallback(saveWifiConfig);

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
    new (&startupNotificationParameter) WiFiManagerParameter("startup_notification", "Startup Notification", 
        wifiConfig.startupNotification ? "t" : "", 1, 
        wifiConfig.startupNotification ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&cycleStartNotificationParameter) WiFiManagerParameter("cycle_start_notification", "Cycle Start Notification", 
        wifiConfig.cycleStartNotification ? "t" : "", 1, 
        wifiConfig.cycleStartNotification ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&cycleStopNotificationParameter) WiFiManagerParameter("cycle_stop_notification", "Cycle Stop Notification", 
        wifiConfig.cycleStopNotification ? "t" : "", 1, 
        wifiConfig.cycleStopNotification ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&pushoverEnabledParameter) WiFiManagerParameter("pushover_enabled", "Pushover Enabled?", 
        wifiConfig.pushoverEnabled ? "t" : "", 1, 
        wifiConfig.pushoverEnabled ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&pushoverAppTokenParameter) WiFiManagerParameter("pushover_app_token", "Pushover App Token", 
        wifiConfig.pushoverAppToken.c_str(), 30);
    new (&pushoverUserTokenParameter) WiFiManagerParameter("pushover_user_token", "Pushover User Token", 
        wifiConfig.pushoverUserToken.c_str(), 30);
    new (&twilioEnabledParameter) WiFiManagerParameter("twilio_enabled", "Twilio Enabled?", 
        wifiConfig.twilioEnabled ? "t" : "", 1, 
        wifiConfig.twilioEnabled ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    new (&twilioAccountSidParameter) WiFiManagerParameter("twilio_account_sid", "Twilio Account SID", 
        wifiConfig.twilioAccountSid.c_str(), 34);
    new (&twilioAuthTokenParameter) WiFiManagerParameter("twilio_auth_token", "Twilio Auth Token", 
        wifiConfig.twilioAuthToken.c_str(), 32);
    new (&twilioFromNumberParameter) WiFiManagerParameter("twilio_from_number", "Twilio From Number", 
        wifiConfig.twilioFromNumber.c_str(), 16);
    new (&twilioToNumbersParameter) WiFiManagerParameter("twilio_to_numbers", "Twilio To Numbers", 
        wifiConfig.twilioToNumbers.c_str(), 100);
    new (&debugModeParameter) WiFiManagerParameter("debug_mode", "Debug Mode", 
        wifiConfig.debugMode ? "t" : "", 1, 
        wifiConfig.debugMode ? CHECKED_BOX_HTML.c_str() : UNCHECKED_BOX_HTML.c_str(), WFM_LABEL_AFTER);
    
    wm.addParameter(&deviceNameParameter);

    wm.addParameter(&motionEnabledParameter);
    wm.addParameter(&motionThresholdParameter);
    wm.addParameter(&motionStartDurationParameter);
    wm.addParameter(&motionStopDurationParameter);

    wm.addParameter(&currentEnabledParameter);
    wm.addParameter(&currentThresholdParameter);
    wm.addParameter(&currentStartDurationParameter);
    wm.addParameter(&currentStopDurationParameter);

    wm.addParameter(&startupNotificationParameter);
    wm.addParameter(&cycleStartNotificationParameter);
    wm.addParameter(&cycleStopNotificationParameter);

    wm.addParameter(&pushoverEnabledParameter);
    wm.addParameter(&pushoverAppTokenParameter);
    wm.addParameter(&pushoverUserTokenParameter);

    wm.addParameter(&twilioEnabledParameter);
    wm.addParameter(&twilioAccountSidParameter);
    wm.addParameter(&twilioAuthTokenParameter);
    wm.addParameter(&twilioFromNumberParameter);
    wm.addParameter(&twilioToNumbersParameter);

    wm.addParameter(&debugModeParameter);

    writeSerialToOled("Connecting to wifi...");
    wmName = "SmartLaundry_" + (wifiConfig.deviceName.length() == 0 ? WiFi.macAddress() : wifiConfig.deviceName);
    if (!wm.autoConnect(wmName.c_str())) {
        writeToCenterOfOled("Wifi failed", true, 5000);
        wm.reboot();
    } else {
        writeSerialToOled("Wifi connected");
        if (wifiConfig.startupNotification) {
            sendStartupNotification();
        }
    }
}

void checkForConfigPortal() {
    if (digitalRead(CONFIG_PIN) == LOW) {
        if (!configButtonPushed) {
            configStartMillis = millis();
            configButtonPushed = true;
        } else if (millis() >= configStartMillis + CONFIG_WAIT_DURATION) {
            String configModeMessages[] = {"Configuration mode", WiFi.localIP().toString()};
            writeToCenterOfOled(configModeMessages, 2, true, 0);
            wm.startConfigPortal(wmName.c_str());
        }
    } else {
        configButtonPushed = false;
    }
}

void saveWifiConfig() {
    wifiConfig.deviceName = deviceNameParameter.getValue();
    wifiConfig.motionEnabled = strncmp(motionEnabledParameter.getValue(), "t", 1) == 0;
    wifiConfig.motionThreshold = atof(motionThresholdParameter.getValue());
    wifiConfig.motionStartDuration = atoi(motionStartDurationParameter.getValue());
    wifiConfig.motionStopDuration = atoi(motionStopDurationParameter.getValue());
    wifiConfig.currentEnabled = strncmp(currentEnabledParameter.getValue(), "t", 1) == 0;
    wifiConfig.currentThreshold = atof(currentThresholdParameter.getValue());
    wifiConfig.currentStartDuration = atoi(currentStartDurationParameter.getValue());
    wifiConfig.currentStopDuration = atoi(currentStopDurationParameter.getValue());
    wifiConfig.startupNotification = strncmp(startupNotificationParameter.getValue(), "t", 1) == 0;
    wifiConfig.cycleStartNotification = strncmp(cycleStartNotificationParameter.getValue(), "t", 1) == 0;
    wifiConfig.cycleStopNotification = strncmp(cycleStopNotificationParameter.getValue(), "t", 1) == 0;
    wifiConfig.pushoverEnabled = strncmp(pushoverEnabledParameter.getValue(), "t", 1) == 0;
    wifiConfig.pushoverAppToken = pushoverAppTokenParameter.getValue();
    wifiConfig.pushoverUserToken = pushoverUserTokenParameter.getValue();
    wifiConfig.twilioEnabled = strncmp(twilioEnabledParameter.getValue(), "t", 1) == 0;
    wifiConfig.twilioAccountSid = twilioAccountSidParameter.getValue();
    wifiConfig.twilioAuthToken = twilioAuthTokenParameter.getValue();
    wifiConfig.twilioFromNumber = twilioFromNumberParameter.getValue();
    wifiConfig.twilioToNumbers = twilioToNumbersParameter.getValue();
    wifiConfig.debugMode = strncmp(debugModeParameter.getValue(), "t", 1) == 0;

    saveConfig(wifiConfig);
}