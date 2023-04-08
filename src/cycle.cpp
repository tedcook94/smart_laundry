#include "cycle.h"
#include "accelerometer.h"
#include "adc.h"
#include "config.h"
#include "notification.h"
#include "oled.h"

const float CURRENT_THRESHOLD = 1.0;
const int CURRENT_START_DURATION = 0 * 1000,
    CURRENT_STOP_DURATION = 180 * 1000,
    LOOP_MIN_DURATION = 2000;

struct config cycleConfig;

bool cycleConfigLoaded = false, 
    inCycle = false, 
    currentDetected = false, 
    currentInTransition = false, 
    motionDetected = false, 
    motionInTransition = false;
float motionThreshold;
int motionStartDuration,
    motionStopDuration;
long currentTransitionStartTime, 
    motionTransitionStartTime;
String progressBar = ".......";

void updateCycleStatus() {
    long loopStartTime = millis();

    if (!cycleConfigLoaded) {
        cycleConfig = getConfig();
        cycleConfigLoaded = true;

        motionThreshold = cycleConfig.motionThreshold / 100.0;
        motionStartDuration = cycleConfig.motionStartDuration * 1000;
        motionStopDuration = cycleConfig.motionStopDuration * 1000;
    }

    if (!cycleConfig.motionEnabled && !cycleConfig.currentEnabled) {
        writeToCenterOfOled("No detectors enabled", true, LOOP_MIN_DURATION);
        return;
    }

    String currentReadout, currentDetectedMessage, accelerometerReadout, motionDetectedMessage;

    if (cycleConfig.currentEnabled) {
        float current = getAdcCurrent();
        bool currentAboveThreshold = current >= CURRENT_THRESHOLD;

        currentReadout = "I: " + String(current);
        currentDetectedMessage = currentAboveThreshold ? "C: true" : "C: false";

        if (inCycle) {
            if (currentDetected && !currentAboveThreshold) {
                if (!currentInTransition) {
                    currentInTransition = true;
                    currentTransitionStartTime = millis();
                }

                if (millis() >= currentTransitionStartTime + CURRENT_STOP_DURATION) {
                    currentDetected = false;
                    currentInTransition = false;
                }
            } else if (!currentDetected && currentAboveThreshold) {
                currentDetected = true;
                currentInTransition = false;
            }
        } else {
            if (currentAboveThreshold) {
                if (!currentInTransition) {
                    currentInTransition = true;
                    currentTransitionStartTime = millis();
                }

                if (millis() >= currentTransitionStartTime + CURRENT_START_DURATION) {
                    currentDetected = true;
                    currentInTransition = false;
                }
            } else {
                currentInTransition = false;
            }
        }
    }

    if (cycleConfig.motionEnabled) {
        struct accelerometer_deviation accelerometerDeviation = getAccelerometerDeviation();
        bool motionAboveThreshold = getMaxAccelerometerDeviation(accelerometerDeviation) >= motionThreshold;

        accelerometerReadout = "A: " + String(accelerometerDeviation.x) + ", " + 
                                    String(accelerometerDeviation.y) + ", " + 
                                    String(accelerometerDeviation.z);
        motionDetectedMessage = motionAboveThreshold ? "M: true" : "M: false";

        if (inCycle) {
            if (motionDetected && !motionAboveThreshold) {
                if (!motionInTransition) {
                    motionInTransition = true;
                    motionTransitionStartTime = millis();
                }

                if (millis() >= motionTransitionStartTime + motionStopDuration) {
                    motionDetected = false;
                    motionInTransition = false;
                }
            } else if (!motionDetected && motionAboveThreshold) {
                motionDetected = true;
                motionInTransition = false;
            }
        } else {
            if (motionAboveThreshold) {
                if (!motionInTransition) {
                    motionInTransition = true;
                    motionTransitionStartTime = millis();
                }

                if (millis() >= motionTransitionStartTime + motionStartDuration) {
                    motionDetected = true;
                    motionInTransition = false;
                }
            } else {
                motionInTransition = false;
            }
        }
    }

    if (!inCycle && ((cycleConfig.currentEnabled && currentDetected) || (cycleConfig.motionEnabled && motionDetected))) {
        inCycle = true;
        progressBar = ".......";
        if (cycleConfig.startNotification) {
            sendNotification("started");
        }
    } else if (inCycle && (!cycleConfig.currentEnabled || !currentDetected) && (!cycleConfig.motionEnabled || !motionDetected)) {
        inCycle = false;
        progressBar = ".......";
        if (cycleConfig.stopNotification) {
            sendNotification("done");
        }
    }

    if (cycleConfig.debugMode) {
        writeSerialToOled("");
        if (cycleConfig.currentEnabled) {
            writeSerialToOled(currentReadout);
            writeSerialToOled(currentDetectedMessage);
        }
        if (cycleConfig.motionEnabled) {
            writeSerialToOled(accelerometerReadout);
            writeSerialToOled(motionDetectedMessage);
        }
    } else {
        String cycleMessage = inCycle ? "Cycle ongoing" : "Waiting for cycle";
        if (progressBar.length() == 7) {
            progressBar = ".";
        } else {
            progressBar += ".";
        }
        String cycleMessages[] = {cycleMessage, progressBar};
        writeToCenterOfOled(cycleMessages, 2, false, 0);
    }

    while (millis() < loopStartTime + LOOP_MIN_DURATION) {
        delay(1);
    }
}