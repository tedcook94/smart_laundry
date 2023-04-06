#include "oled.h"
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <CircularBuffer.h>

const int SCREEN_WIDTH = 128,
    SCREEN_HEIGHT = 64,
    SERIAL_ROWS = 7;

Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

bool oledConnected;
CircularBuffer<String, SERIAL_ROWS> serialMessages;

void setupOled() {
    oledConnected = oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    if (!oledConnected) {
        Serial.println("Error starting OLED");
        return;
    }

    oled.setTextSize(1);
    oled.setTextColor(WHITE);

    serialMessages.push("");

    writeToCenterOfOled("Starting up...");
}

void writeSerialToOled(String message) {
    writeSerialToOled(message, true);
}

void writeSerialToOled(String message, bool printToSerial) {
    if (!oledConnected) {
        Serial.println("OLED not connected");
        return;
    }

    if (printToSerial) {
        Serial.println(message);
    }

    int cursorX = 0;
    int cursorY = SCREEN_HEIGHT - 1 - SCREEN_HEIGHT / SERIAL_ROWS;

    oled.clearDisplay();
    oled.setCursor(cursorX, cursorY);

    serialMessages.unshift(message);
    
    for (int i = 0; i < SERIAL_ROWS; i++) {
        oled.setCursor(cursorX, cursorY);
        oled.print(serialMessages[i]);
        cursorY -= SCREEN_HEIGHT/SERIAL_ROWS;
    }

    oled.display();
}

void writeToCenterOfOled(String message) {
    writeToCenterOfOled(message, true);
}

void writeToCenterOfOled(String message, bool printToSerial) {
    if (!oledConnected) {
        Serial.println("OLED not connected");
        return;
    }

    if (printToSerial) {
        Serial.println(message);
    }

    GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
    int16_t x1, y1;
    uint16_t textWidth, textHeight;
    canvas.getTextBounds(message, 0, 0, &x1, &y1, &textWidth, &textHeight);

    int cursorX = (SCREEN_WIDTH - textWidth) / 2;
    int cursorY = (SCREEN_HEIGHT - textHeight) / 2;

    oled.clearDisplay();
    oled.setCursor(cursorX, cursorY);
    oled.print(message);
    oled.display();
    delay(2000);
}

void writeToCenterOfOled(String messages[], int messagesCount) {
    writeToCenterOfOled(messages, messagesCount, true);
}

void writeToCenterOfOled(String messages[], int messagesCount, bool printToSerial) {
    if (!oledConnected) {
        Serial.println("OLED not connected");
        return;
    }

    uint16_t textWidths[messagesCount], textHeights[messagesCount];
    uint16_t totalTextHeight = 0;

    for (int i = 0; i < messagesCount; i++) {
        String message = messages[i];

        if (printToSerial) {
            Serial.println(message);
        }

        GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
        int16_t x1, y1;
        uint16_t textWidth, textHeight;
        canvas.getTextBounds(message, 0, 0, &x1, &y1, &textWidth, &textHeight);

        textWidths[i] = textWidth;
        textHeights[i] = textHeight;
        totalTextHeight += textHeight;
    }

    oled.clearDisplay();

    int cursorY = (SCREEN_HEIGHT - totalTextHeight) / 2;

    for (int i = 0; i < messagesCount; i++) {
        String message = messages[i];

        int cursorX = (SCREEN_WIDTH - textWidths[i]) / 2;

        oled.setCursor(cursorX, cursorY);
        oled.print(message);

        cursorY += textHeights[i];
    }

    oled.display();
    delay(2000);
}