/*
*   Project: SmartDesk 0.1.1
*   Description: The code needed to run the protoType SmartDesk
*   Author: Russell Brazell
*   Date: 8-6-2021
*/

// #include "touchScreen.h"
#include <SPI.h>
#include <Adafruit_mfGFX.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206_Library.h>

SYSTEM_MODE(MANUAL);

//  PINS
const int touchScreenDisplayDataCommand = D5;
const int touchScreenDisplayCS = D4;
const int SD_CS = D2;

const int TOUCH_SENSITIVITY = 128;

bool homeButtonPressed = true;
bool lightButtonPressed = false;
bool waterButtonPressed = false;
bool calendarButtonPressed = false;
bool fingerPrintButtonPressed = false;

// Screen size is 320 x 240
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

// HOME MENU FRAME SIZES

const int FRAME_X_ORIGIN = 0;
const int FRAME_Y_ORIGIN = 0;
const int FRAME_WIDTH = SCREEN_WIDTH / 2;
const int FRAME_HEIGHT = SCREEN_HEIGHT / 2;

// LIGHT BUTTON = top left

const int LIGHT_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int LIGHT_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN;
const int LIGHT_BUTTON_WIDTH = FRAME_WIDTH;
const int LIGHT_BUTTON_HEIGHT = FRAME_HEIGHT;

// WATER BUTTON = top right

const int WATER_BUTTON_X_ORIGIN = (LIGHT_BUTTON_X_ORIGIN + LIGHT_BUTTON_WIDTH);
const int WATER_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN;
const int WATER_BUTTON_WIDTH = FRAME_WIDTH;
const int WATER_BUTTON_HEIGHT = FRAME_HEIGHT;

// CALENDAR BUTTON = bottom left

const int CALENDAR_BUTTON_X_ORIGIN = FRAME_Y_ORIGIN;
const int CALENDAR_BUTTON_Y_ORIGIN = (FRAME_Y_ORIGIN + LIGHT_BUTTON_HEIGHT);
const int CALENDAR_BUTTON_WIDTH = FRAME_WIDTH;
const int CALENDAR_BUTTON_HEIGHT = FRAME_HEIGHT;

// FINGERPRINT BUTTON = bottom right

const int FINGERPRINT_BUTTON_X_ORIGIN = FRAME_Y_ORIGIN + WATER_BUTTON_WIDTH;
const int FINGERPRINT_BUTTON_Y_ORIGIN = (FRAME_Y_ORIGIN + WATER_BUTTON_HEIGHT);
const int FINGERPRINT_BUTTON_WIDTH = FRAME_WIDTH;
const int FINGERPRINT_BUTTON_HEIGHT = FRAME_HEIGHT;

//  Display Screen uses SPI --SEE PINS--
Adafruit_ILI9341 touchScreenDisplay(touchScreenDisplayCS, touchScreenDisplayDataCommand);

// Touch Screen uses hardware I2C (SCL/SDA)
Adafruit_FT6206 capacitiveTouchScreen = Adafruit_FT6206();

void setup() {
    setUpTouchScreen();
}

void loop() {
   runTouchScreen();
}

void setUpTouchScreen() {
    displaySetUp();
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    capacitiveTouchScreen.begin(TOUCH_SENSITIVITY);
    // origin = left, top landscape (Reset button left upper)
    touchScreenDisplay.setRotation(1);
    goToHomeMenu();
}

void displaySetUp() {
    Serial.begin(115200);
    waitFor(Serial.isConnected, 3000);
    Serial.println("Touch Screen Test!");
    touchScreenDisplay.begin();
// read diagnostics (optional but can help debug problems)
    uint8_t x = touchScreenDisplay.readcommand8(ILI9341_RDMODE);
    Serial.printf("Display Power Mode: 0x%x\n", x);

    x = touchScreenDisplay.readcommand8(ILI9341_RDMADCTL);
    Serial.printf("MADCTL Mode: 0x%x\n", x);

    x = touchScreenDisplay.readcommand8(ILI9341_RDPIXFMT);
    Serial.printf("Pixel Format: 0x%x\n", x);

    x = touchScreenDisplay.readcommand8(ILI9341_RDIMGFMT);
    Serial.printf("Image Format: 0x%x\n", x);

    x = touchScreenDisplay.readcommand8(ILI9341_RDSELFDIAG);
    Serial.printf("Self Diagnostic: 0x%x\n", x);

    Serial.println("Done!");
//    End diagnostics

    testFillScreen();
    testRects(ILI9341_PINK);
//    Welcome message

    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    touchScreenDisplay.setCursor(((SCREEN_WIDTH / 2) - 5), ((SCREEN_HEIGHT / 2) - 3));
    touchScreenDisplay.setTextColor(ILI9341_GREEN);
    touchScreenDisplay.setTextSize(4);
    touchScreenDisplay.printf("Welcome to\nSmart Desk\n1.0.1\n");
}

void runTouchScreen() {
    Serial.printf("Checking if you touched anywhere\n");
    homeButtonSelect();
    lightButton();
    waterButton();
    calendarButton();
    fingerPrintButton();
}

void lightButton() {
    if (lightButtonPressed) {
        touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                    LIGHT_BUTTON_Y_ORIGIN,
                                    LIGHT_BUTTON_WIDTH,
                                    LIGHT_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
    } else {
        touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                    LIGHT_BUTTON_Y_ORIGIN,
                                    LIGHT_BUTTON_HEIGHT,
                                    LIGHT_BUTTON_HEIGHT,
                                    ILI9341_RED);
//        TODO make a color array to glow the button
    }
//    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((LIGHT_BUTTON_WIDTH / 2) - 3), (LIGHT_BUTTON_HEIGHT / 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Lights\n");
}

void waterButton() {
    if (waterButtonPressed) {
        touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                    WATER_BUTTON_Y_ORIGIN,
                                    WATER_BUTTON_WIDTH,
                                    WATER_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
    } else {
        touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                    WATER_BUTTON_Y_ORIGIN,
                                    WATER_BUTTON_WIDTH,
                                    WATER_BUTTON_HEIGHT,
                                    ILI9341_BLUE);
    }
//    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((WATER_BUTTON_WIDTH / 2) + 2), ((WATER_BUTTON_Y_ORIGIN / 2) - 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Water\nScale\n");
}

void calendarButton() {
    if (calendarButtonPressed) {
        touchScreenDisplay.fillRect(CALENDAR_BUTTON_X_ORIGIN,
                                    CALENDAR_BUTTON_Y_ORIGIN,
                                    CALENDAR_BUTTON_WIDTH,
                                    CALENDAR_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
    } else {
        touchScreenDisplay.fillRect(CALENDAR_BUTTON_X_ORIGIN,
                                    CALENDAR_BUTTON_Y_ORIGIN,
                                    CALENDAR_BUTTON_WIDTH,
                                    CALENDAR_BUTTON_HEIGHT,
                                    ILI9341_BLUE);
    }
//    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((CALENDAR_BUTTON_WIDTH / 2) + 4), (CALENDAR_BUTTON_Y_ORIGIN / 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Calendar\n");
}

void fingerPrintButton() {
    if (fingerPrintButtonPressed) {
        touchScreenDisplay.fillRect(FINGERPRINT_BUTTON_X_ORIGIN,
                                    FINGERPRINT_BUTTON_Y_ORIGIN,
                                    FINGERPRINT_BUTTON_WIDTH,
                                    FRAME_HEIGHT,
                                    ILI9341_PURPLE);
    } else {
        touchScreenDisplay.fillRect(FINGERPRINT_BUTTON_X_ORIGIN,
                                    FINGERPRINT_BUTTON_Y_ORIGIN,
                                    FINGERPRINT_BUTTON_WIDTH,
                                    FINGERPRINT_BUTTON_HEIGHT,
                                    ILI9341_BLUE);
    }
    //    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((FINGERPRINT_BUTTON_WIDTH / 2) + 2), ((FINGERPRINT_BUTTON_Y_ORIGIN / 2) - 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Finger\nPrint\n");
}

void homeButtonSelect() {
    // Retrieve a point
    TS_Point touchedPoint = capacitiveTouchScreen.getPoint();
    // flip it around to match the screen.
    touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
    touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
    int y = touchScreenDisplay.height() - touchedPoint.x;
    int x = touchedPoint.y;
    if ((x > LIGHT_BUTTON_X_ORIGIN)
        && (x < (LIGHT_BUTTON_X_ORIGIN + LIGHT_BUTTON_WIDTH))
        && (y > LIGHT_BUTTON_Y_ORIGIN)
        && (y < (LIGHT_BUTTON_Y_ORIGIN + LIGHT_BUTTON_HEIGHT))) {
        Serial.printf("Light Button Pressed\n");
        lightButtonPressed = true;
        waterButtonPressed = false;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = false;
    } else if ((x > WATER_BUTTON_X_ORIGIN)
               && (x < (WATER_BUTTON_X_ORIGIN + WATER_BUTTON_WIDTH))
               && (y > WATER_BUTTON_Y_ORIGIN)
               && (y < (WATER_BUTTON_Y_ORIGIN + WATER_BUTTON_HEIGHT))) {
        Serial.printf("Water Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = true;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = false;
    } else if ((x > CALENDAR_BUTTON_X_ORIGIN)
               && (x < CALENDAR_BUTTON_X_ORIGIN + CALENDAR_BUTTON_WIDTH)
               && (y > CALENDAR_BUTTON_Y_ORIGIN)
               && (y < (CALENDAR_BUTTON_Y_ORIGIN + CALENDAR_BUTTON_HEIGHT))) {
        Serial.printf("Calendar Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = false;
        calendarButtonPressed = true;
        fingerPrintButtonPressed = false;
    } else if ((x > FINGERPRINT_BUTTON_X_ORIGIN)
               && (x < FINGERPRINT_BUTTON_X_ORIGIN + FINGERPRINT_BUTTON_WIDTH)
               && (y > FINGERPRINT_BUTTON_Y_ORIGIN)
               && (y < (FINGERPRINT_BUTTON_Y_ORIGIN + FINGERPRINT_BUTTON_HEIGHT))) {
        Serial.printf("Finger Print Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = false;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = true;
    }
}

void goToHomeMenu() {
    homeButtonPressed = true;
    lightButtonPressed = false;
    waterButtonPressed = false;
    calendarButtonPressed = false;
    fingerPrintButtonPressed = false;
}

unsigned long testFillScreen() {
    unsigned long start = micros();
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    yield();
    touchScreenDisplay.fillScreen(ILI9341_RED);
    yield();
    touchScreenDisplay.fillScreen(ILI9341_GREEN);
    yield();
    touchScreenDisplay.fillScreen(ILI9341_BLUE);
    yield();
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    yield();
    return micros() - start;
}

void drawFrame() {
    touchScreenDisplay.drawRect(FRAME_X_ORIGIN, FRAME_Y_ORIGIN, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
}

unsigned long testRects(uint16_t color) {
    unsigned long start;
    int n, i, i2,
            cx = touchScreenDisplay.width() / 2,
            cy = touchScreenDisplay.height() / 2;
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    n = min(touchScreenDisplay.width(), touchScreenDisplay.height());
    start = micros();
    for (i = 2; i < n; i += 6) {
        i2 = i / 2;
        touchScreenDisplay.drawRect(cx - i2, cy - i2, i, i, color);
    }
    return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
    unsigned long start, t = 0;
    int n, i, i2,
            cx = touchScreenDisplay.width() / 2 - 1,
            cy = touchScreenDisplay.height() / 2 - 1;
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    n = min(touchScreenDisplay.width(), touchScreenDisplay.height());
    for (i = n; i > 0; i -= 6) {
        i2 = i / 2;
        start = micros();
        touchScreenDisplay.fillRect(cx - i2, cy - i2, i, i, color1);
        t += micros() - start;
        // Outlines are not included in timing results
        touchScreenDisplay.drawRect(cx - i2, cy - i2, i, i, color2);
        yield();
    }
    return t;
}
