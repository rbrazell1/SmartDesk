/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreenDebug/src/SmartDeskScreenDebug.ino"
/***************************************************
  This is our GFX example for the Adafruit ILI9341 TFT FeatherWing
  ----> http://www.adafruit.com/products/3315

  Check out the links above for our tutorials and wiring diagrams

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

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
#line 21 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreenDebug/src/SmartDeskScreenDebug.ino"
SYSTEM_MODE(MANUAL);

#define TFT_DC   D5
#define TFT_CS   D4
// #define STMPE_CS D3
// #define SD_CS    D2

Adafruit_ILI9341 tft(TFT_CS, TFT_DC);

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
Adafruit_FT6206 ts = Adafruit_FT6206();

void setup() {
    displaySetUp();
    tft.fillScreen(ILI9341_BLACK);
    ts.begin(128);
    // origin = left,top landscape (USB left upper)
    tft.setRotation(1);
    redBtn();
}


void loop(void) {
    // testFilledRects(ILI9341_CYAN, ILI9341_DARKGREY);
    // testRects(ILI9341_DARKGREEN);
    // See if there's any  touch data for us
    delay(100);
    Serial.printf("Checking if you touched\n");
    // if (ts.touched()) {
        // Retrieve a point  
        TS_Point touchedPoint = ts.getPoint();
        // rotate coordinate system
        // flip it around to match the screen.
        touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
        touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
        int y = tft.height() - touchedPoint.x;
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

    tft.begin();

    // read diagnostics (optional but can help debug problems)
    uint8_t x = tft.readcommand8(ILI9341_RDMODE);
    Serial.printf("Display Power Mode: 0x%x\n", x);

    x = tft.readcommand8(ILI9341_RDMADCTL);
    Serial.printf("MADCTL Mode: 0x%x\n", x);

    x = tft.readcommand8(ILI9341_RDPIXFMT);
    Serial.printf("Pixel Format: 0x%x\n", x);

    x = tft.readcommand8(ILI9341_RDIMGFMT);
    Serial.printf("Image Format: 0x%x\n", x);

    x = tft.readcommand8(ILI9341_RDSELFDIAG);
    Serial.printf("Self Diagnostic: 0x%x\n", x);

    // Serial.println(F("Benchmark                Time (microseconds)"));
    // delay(10);
    // Serial.print(F("Screen fill              "));
    // Serial.println(testFillScreen());
    // delay(100);

    // Serial.print(F("Rectangles (outline)     "));
    // Serial.println(testRects(ILI9341_GREEN));
    // delay(100);

    // Serial.print(F("Rectangles (filled)      "));
    // Serial.println(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
    // delay(100);

    Serial.println(F("Done!"));
}

unsigned long testFillScreen() {
    unsigned long start = micros();
    tft.fillScreen(ILI9341_BLACK);
    yield();
    tft.fillScreen(ILI9341_RED);
    yield();
    tft.fillScreen(ILI9341_GREEN);
    yield();
    tft.fillScreen(ILI9341_BLUE);
    yield();
    tft.fillScreen(ILI9341_BLACK);
    yield();
    return micros() - start;
}

void drawFrame() {
    tft.drawRect(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, ILI9341_BLACK);
}

void redBtn() {
    tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_RED);
    tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_BLUE);
    drawFrame();
    tft.setCursor(GREENBUTTON_X + 6, GREENBUTTON_Y + (GREENBUTTON_H / 2));
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("ON");
    RecordOn = false;
}

void greenBtn() {
    tft.fillRect(GREENBUTTON_X, GREENBUTTON_Y, GREENBUTTON_W, GREENBUTTON_H, ILI9341_GREEN);
    tft.fillRect(REDBUTTON_X, REDBUTTON_Y, REDBUTTON_W, REDBUTTON_H, ILI9341_BLUE);
    drawFrame();
    tft.setCursor(REDBUTTON_X + 6, REDBUTTON_Y + (REDBUTTON_H / 2));
    tft.setTextColor(ILI9341_WHITE);
    tft.setTextSize(2);
    tft.println("OFF");
    RecordOn = true;
}


/*
unsigned long testText() {
  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("Hello World!");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(1234.56);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(0xDEADBEEF, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");
  return micros() - start;
}
*/

unsigned long testRects(uint16_t color) {
    unsigned long start;
    int n, i, i2,
            cx = tft.width() / 2,
            cy = tft.height() / 2;

    tft.fillScreen(ILI9341_BLACK);
    n = min(tft.width(), tft.height());
    start = micros();
    for (i = 2; i < n; i += 6) {
        i2 = i / 2;
        tft.drawRect(cx - i2, cy - i2, i, i, color);
    }

    return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
    unsigned long start, t = 0;
    int n, i, i2,
            cx = tft.width() / 2 - 1,
            cy = tft.height() / 2 - 1;

    tft.fillScreen(ILI9341_BLACK);
    n = min(tft.width(), tft.height());
    for (i = n; i > 0; i -= 6) {
        i2 = i / 2;
        start = micros();
        tft.fillRect(cx - i2, cy - i2, i, i, color1);
        t += micros() - start;
        // Outlines are not included in timing results
        tft.drawRect(cx - i2, cy - i2, i, i, color2);
        yield();
    }

    return t;
}
