#include "adc.h"
#include "config.h"
#include "oled.h"
#include "util.h"
#include <Adafruit_ADS1X15.h>

const float BITS_TO_VOLTS = 0.00005, // GAIN_FOUR -> +/- 1.024V -> 1bit = 0.5mV
    VOLTS_TO_AMPS = 30, // for 30A/1V current transformer
    BITS_TO_AMPS = BITS_TO_VOLTS * VOLTS_TO_AMPS,
    COMPENSATION_FACTOR = 5.75;
const int POLLING_COUNT = 50;

Adafruit_ADS1115 adc;
struct config adcConfig;

bool adcConnected;

void setupAdc() {
    adcConfig = getConfig();

    if (adcConfig.currentEnabled) {
        adc.setGain(GAIN_FOUR);
        adcConnected = adc.begin(0x48);

        if (!adcConnected) {
            writeSerialToOled("Error starting ADC");
            return;
        }

        writeSerialToOled("ADC started");
    } else {
        adcConnected = false;
        writeSerialToOled("Current disabled");
    }
}

float getAdcVoltage() {
    if (!adcConfig.currentEnabled) {
        Serial.println("Current disabled");
        return 0;
    }

    if (!adcConnected) {
        Serial.println("ADC not connected");
        return 0;
    }

    float readings[POLLING_COUNT];

    for (int i = 0; i < POLLING_COUNT; i++) {
        readings[i] = adc.readADC_Differential_0_1() * BITS_TO_VOLTS;
    }

    return getRootMeanSquare(readings, POLLING_COUNT);
}

float getAdcCurrent() {
    if (!adcConfig.currentEnabled) {
        Serial.println("Current disabled");
        return 0;
    }
    
    if (!adcConnected) {
        Serial.println("ADC not connected");
        return 0;
    }

    float readings[POLLING_COUNT];

    for (int i = 0; i < POLLING_COUNT; i++) {
        readings[i] = adc.readADC_Differential_0_1() * BITS_TO_AMPS * COMPENSATION_FACTOR;
    }

    return getRootMeanSquare(readings, POLLING_COUNT);
}