#include "notification.h"
#include "config.h"
#include "oled.h"
#include <twilio.hpp>
#include <WiFiClientSecure.h>

const char* PUSHOVER_ROOT_CA =
    "-----BEGIN CERTIFICATE-----\n" \
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n" \
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n" \
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n" \
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n" \
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n" \
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n" \
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n" \
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n" \
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n" \
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n" \
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n" \
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n" \
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n" \
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n" \
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n" \
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n" \
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n" \
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n" \
    "-----END CERTIFICATE-----\n";

Twilio *twilio;
WiFiClientSecure pushoverClient;
struct config notificationConfig;

bool notificationConfigLoaded = false,
    pushoverNotificationSent = false,
    twilioNotificationSent = false;
int pushoverAttempts = 0,
    twilioAttempts = 0;

void loadNotificationConfig() {
    if (!notificationConfigLoaded) {
        notificationConfig = getConfig();
        notificationConfigLoaded = true;
    }
}

String getDeviceName() {
    return notificationConfig.deviceName.length() == 0 ? "Smart Laundry" : notificationConfig.deviceName;
}

void sendPushoverNotification(String message) {
    if (notificationConfig.pushoverEnabled) {
        if (notificationConfig.pushoverAppToken.length() == 0 || notificationConfig.pushoverUserToken.length() == 0) {
            writeToCenterOfOled("Configure Pushover", true, 5000);
        } else {
            String pushoverParameters = "token=" + notificationConfig.pushoverAppToken + 
                                    "&user=" + notificationConfig.pushoverUserToken + 
                                    "&message=" + message;

            pushoverClient.setCACert(PUSHOVER_ROOT_CA);

            while (!pushoverNotificationSent && pushoverAttempts < 3) {
                if (pushoverClient.connect("api.pushover.net", 443)) {
                    pushoverClient.println("POST /1/messages.json HTTP/1.1");
                    pushoverClient.println("Host: api.pushover.net");
                    pushoverClient.println("Connection: close\r\nContent-Type: application/x-www-form-urlencoded");
                    pushoverClient.print("Content-Length: ");
                    pushoverClient.print(pushoverParameters.length());
                    pushoverClient.println("\r\n");
                    pushoverClient.print(pushoverParameters);

                    while (pushoverClient.connected()) {
                        while (pushoverClient.available()) {
                            Serial.write(pushoverClient.read());
                        }
                    }
                    
                    pushoverClient.stop();
                    pushoverNotificationSent = true;
                } else {
                    String pushoverFailedMessages[] = {"Pushover failed", "Attempt " + String(pushoverAttempts + 1)};
                    writeToCenterOfOled(pushoverFailedMessages, 2, true, 5000);
                    pushoverAttempts += 1;
                }
            }

            pushoverNotificationSent = false;
            pushoverAttempts = 0;
        }
    }
}

void sendTwilioNotification(String message) {
    if (notificationConfig.twilioEnabled) {
        if (notificationConfig.twilioAccountSid.length() == 0 || 
                notificationConfig.twilioAuthToken.length() == 0 ||
                notificationConfig.twilioFromNumber.length() == 0 ||
                notificationConfig.twilioToNumbers.length() == 0) {
            writeToCenterOfOled("Configure Twilio", true, 5000);
        } else {
            twilio = new Twilio(notificationConfig.twilioAccountSid.c_str(), notificationConfig.twilioAuthToken.c_str());

            char toNumbers[notificationConfig.twilioToNumbers.length()];
            notificationConfig.twilioToNumbers.toCharArray(toNumbers, notificationConfig.twilioToNumbers.length() + 1);
            char *currentToNumber = NULL;
            currentToNumber = strtok(toNumbers, ",");
            
            while (currentToNumber != NULL) {
                while (!twilioNotificationSent && twilioAttempts < 3) {
                    String response;
                    if (twilio->send_message(currentToNumber, notificationConfig.twilioFromNumber, message, response)) {
                        Serial.println(response);
                        twilioNotificationSent = true;
                    } else {
                        String twilioFailedMessages[] = {"Twilio failed", currentToNumber, "Attempt " + String(twilioAttempts + 1)};
                        writeToCenterOfOled(twilioFailedMessages, 3, true, 5000);
                        twilioAttempts += 1;
                    }
                }

                currentToNumber = strtok(NULL, ",");
                twilioNotificationSent = false;
                twilioAttempts = 0;
            }
        }
    }
}

void sendStartupNotification() {
    loadNotificationConfig();

    String message = getDeviceName() + " has started and is online!";
    sendPushoverNotification(message);
    sendTwilioNotification(message);
}

void sendCycleNotification(String cycleStatus) {
    loadNotificationConfig();

    String message = getDeviceName() + " cycle " + cycleStatus + "!";
    sendPushoverNotification(message);
    sendTwilioNotification(message);
}