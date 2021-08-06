/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreenDebug/src/SmartDeskScreenDebug.ino"
/*
*   Project: SmartDesk 0.1.1
*   Description: The code needed to run the protoType SmartDesk
*   Author: Russell Brazell
*   Date: 8-6-2021
*/

#include <SPI.h>
#include <Adafruit_mfGFX.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206_Library.h>

void setup();
void loop(void);
void displaySetUp();
unsigned long testFillScreen();
void drawFrame();
void redBtn();
void greenBtn();
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
#line 14 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreenDebug/src/SmartDeskScreenDebug.ino"
SYSTEM_MODE(MANUAL);

const int touchScreenDisplayDataCommand = D5;
const int touchScreenDisplayCS = D4;
const int SD_CS = D2;

Adafruit_ILI9341 touchScreenDisplay(touchScreenDisplayCS, touchScreenDisplayDataCommand);

boolean RecordOn = false;

#define FRAME_X 10
#define FRAME_Y 10
#define FRAME_W 200
#define FRAME_H 100

#define REDBUTTON_X FRAME_X
#define REDBUTTON_Y FRAME_Y
#define REDBUTTON_W (FRAME_W/2)
#define REDBUTTON_H FRAME_H

#define GREENBUTTON_X (REDBUTTON_X + REDBUTTON_W)
#define GREENBUTTON_Y FRAME_Y
#define GREENBUTTON_W (FRAME_W/2)
#define GREENBUTTON_H FRAME_H

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 capacitiveTouchScreen = Adafruit_FT6206();

void setup() {
    displaySetUp();
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    capacitiveTouchScreen.begin(128);
    // origin = left,top landscape (USB left upper)
    touchScreenDisplay.setRotation(1);
    redBtn();
}


void loop(void) {
    // testFilledRects(ILI9341_CYAN, ILI9341_DARKGREY);
    // testRects(ILI9341_DARKGREEN);
    // See if there's any  touch data for us
    delay(100);
    Serial.printf("Checking if you touched\n");
    // if (capacitiveTouchScreen.touched()) {
        // Retrieve a point  
        TS_Point touchedPoint = capacitiveTouchScreen.getPoint();
        // rotate coordinate system
        // flip it around to match the screen.
        touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
        touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
        int y = touchScreenDisplay.height() - touchedPoint.x;
        int x = touchedPoint.y;

        if (RecordOn) {
            if ((x > REDBUTTON_X) && (x < (REDBUTTON_X + REDBUTTON_W))) {
                if ((y > REDBUTTON_Y) && (y <= (REDBUTTON_Y + REDBUTTON_H))) {
                    Serial.println("Red btn hit");
                    redBtn();
                }
            }
        } else {
            if ((x > GREENBUTTON_X) && (x < (GREENBUTTON_X + GREENBUTTON_W))) {
                if ((y > GREENBUTTON_Y) && (y <= (GREENBUTTON_Y + GREENBUTTON_H))) {
                    Serial.println("Green btn hit");
                    greenBtn();
                }
            }
        }

        Serial.println(RecordOn);
    // }
}

void displaySetUp() {
    Serial.begin(115200);

    delay(10);
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

    Serial.println(F("Done!"));
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
    touchScreenDisplay.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
}

void redBtn() {
    touchScreenDisplay.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_RED);
    touchScreenDisplay.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_BLUE);
    drawFrame();
    touchScreenDisplay.setCursor(GREENBUTTON_X + 6, GREENBUTTON_Y + (GREENBUTTON_H / 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.println("ON");
    RecordOn = false;
}

void greenBtn() {
    touchScreenDisplay.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_GREEN);
    touchScreenDisplay.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_BLUE);
    drawFrame();
    touchScreenDisplay.setCursor(REDBUTTON_X + 6, REDBUTTON_Y + (REDBUTTON_H / 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.println("OFF");
    RecordOn = true;
}


/*
unsigned long testText() {
  touchScreenDisplay.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  touchScreenDisplay.setCursor(0, 0);
  touchScreenDisplay.setTextColor(ILI9341_WHITE);  touchScreenDisplay.setTextSize(1);
  touchScreenDisplay.println("Hello World!");
  touchScreenDisplay.setTextColor(ILI9341_YELLOW); touchScreenDisplay.setTextSize(2);
  touchScreenDisplay.println(1234.56);
  touchScreenDisplay.setTextColor(ILI9341_RED);    touchScreenDisplay.setTextSize(3);
  touchScreenDisplay.println(0xDEADBEEF, HEX);
  touchScreenDisplay.println();
  touchScreenDisplay.setTextColor(ILI9341_GREEN);
  touchScreenDisplay.setTextSize(5);
  touchScreenDisplay.println("Groop");
  touchScreenDisplay.setTextSize(2);
  touchScreenDisplay.println("I implore thee,");
  touchScreenDisplay.setTextSize(1);
  touchScreenDisplay.println("my foonting turlingdromes.");
  touchScreenDisplay.println("And hooptiously drangle me");
  touchScreenDisplay.println("with crinkly bindlewurdles,");
  touchScreenDisplay.println("Or I will rend thee");
  touchScreenDisplay.println("in the gobberwarts");
  touchScreenDisplay.println("with my blurglecruncheon,");
  touchScreenDisplay.println("see if I don't!");
  return micros() - start;
}
*/

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
