#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H
#include <Arduino.h>

struct accelerometer_deviation {
    float x;
    float y;
    float z;
};

void setupAccelerometer();

struct accelerometer_deviation getAccelerometerDeviation();

float getMaxAccelerometerDeviation(struct accelerometer_deviation accelerometerDeviation);

#endif