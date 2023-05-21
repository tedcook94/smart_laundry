#include "util.h"

float getRootMeanSquare(float values[], int valueCount) {
    float sum = 0;

    for (int i = 0; i < valueCount; i++) {
        sum += sq(values[i]);
    }

    return sqrt(sum / valueCount);
}

float getStandardDeviation(float values[], int valueCount) {
    float sum = 0;

    for (int i = 0; i < valueCount; i++) {
        sum += values[i];
    }

    float mean = sum / valueCount;
    float sdSum = 0;

    for (int i = 0; i < valueCount; i++) {
        sdSum += pow(mean - values[i], 2);
    }

    return sqrt(sdSum / valueCount);
}

String boolToString(bool value) {
    return value ? "true" : "false";
}