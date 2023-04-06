#include "accelerometer.h"
#include "config.h"
#include "oled.h"
#include "util.h"
#include <Adafruit_ADXL345_U.h>

const int POLLING_COUNT = 500;

Adafruit_ADXL345_Unified accelerometer = Adafruit_ADXL345_Unified();
struct config accelerometerConfig;

bool accelerometerConnected;

void setupAccelerometer() {
    accelerometerConfig = getConfig();

    if (accelerometerConfig.motionEnabled) {
        accelerometerConnected = accelerometer.begin();

        if (!accelerometerConnected) {
            writeSerialToOled("Error starting accelerometer");
            return;
        }

        accelerometer.setRange(ADXL345_RANGE_2_G);
        writeSerialToOled("Accelerometer started");
    } else {
        accelerometerConnected = false;
        writeSerialToOled("Motion disabled");
    }
}

struct accelerometer_deviation getAccelerometerDeviation() {
    struct accelerometer_deviation accelerometer_deviation;

    if (!accelerometerConfig.motionEnabled) {
        Serial.println("Motion disabled");
        return accelerometer_deviation;
    }

    if (!accelerometerConnected) {
        Serial.println("Accelerometer not connected");
        return accelerometer_deviation;
    }

    float readings_x[POLLING_COUNT];
    float readings_y[POLLING_COUNT];
    float readings_z[POLLING_COUNT];
    sensors_event_t event;

    for (int i = 0; i < POLLING_COUNT; i++) {
        accelerometer.getEvent(&event);
        readings_x[i] = event.acceleration.x;
        readings_y[i] = event.acceleration.y;
        readings_z[i] = event.acceleration.z;
    }

    accelerometer_deviation.x = getStandardDeviation(readings_x, POLLING_COUNT);
    accelerometer_deviation.y = getStandardDeviation(readings_y, POLLING_COUNT);
    accelerometer_deviation.z = getStandardDeviation(readings_z, POLLING_COUNT);
    
    return accelerometer_deviation;
}

float getMaxAccelerometerDeviation(struct accelerometer_deviation accelerometerDeviation) {
    float maxDeviation;

    maxDeviation = max(accelerometerDeviation.x, accelerometerDeviation.y);
    maxDeviation = max(maxDeviation, accelerometerDeviation.z);

    return maxDeviation;
}