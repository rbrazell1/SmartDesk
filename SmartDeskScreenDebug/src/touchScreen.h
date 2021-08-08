/*
*   Project: SmartDesk 0.1.1 - TOUCH SCREEN & MENU
*   Description: Screen layout and control
*   Author: Russell Brazell
*   Date: 8-6-2021
*/

#include <SPI.h>
#include <Adafruit_mfGFX.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206_Library.h>

//  PINS
const int touchScreenDisplayDataCommand = D5;
const int touchScreenDisplayCS = D4;
const int SD_CS = D2;

const int TOUCH_SENSITIVITY = 128;

bool lightButtonPressed = false;
bool waterButtonPressed = false;

// Screen size is 320 x 240
const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 240;

// BOX SIZES
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
const int CALENDAR_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN + LIGHT_BUTTON_HEIGHT;
const int CALENDAR_BUTTON_WIDTH = FRAME_WIDTH;
const int CALENDAR_BUTTON_HEIGHT = FRAME_HEIGHT;

// FINGERPRINT BUTTON = bottom right

const int FINGERPRINT_BUTTON_X_ORIGIN = FRAME_Y_ORIGIN + WATER_BUTTON_WIDTH;
const int FINGERPRINT_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN + LIGHT_BUTTON_HEIGHT;
const int FINGERPRINT_BUTTON_WIDTH = FRAME_WIDTH;
const int FINGERPRINT_BUTTON_HEIGHT = FRAME_HEIGHT;

//  Display Screen uses SPI --SEE PINS--
Adafruit_ILI9341 touchScreenDisplay(touchScreenDisplayCS, touchScreenDisplayDataCommand);

// Touch Screen uses hardware I2C (SCL/SDA)
Adafruit_FT6206 capacitiveTouchScreen = Adafruit_FT6206();

void setUpTouchScreen() {
    displaySetUp();
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    capacitiveTouchScreen.begin(TOUCH_SENSITIVITY);
    // origin = left,top landscape (USB left upper)
    touchScreenDisplay.setRotation(1);
    redBtn();
}

void runTouchScreen() {
    delay(100);
    Serial.printf("Checking if you touched anywhere\n");
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

void lightButton() {
    touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                LIGHT_BUTTON_Y_ORIGIN,
                                LIGHT_BUTTON_HEIGHT,
                                LIGHT_BUTTON_HEIGHT,
                                ILI9341_RED);
    if (lightButtonPressed) {
        touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                    LIGHT_BUTTON_Y_ORIGIN,
                                    LIGHT_BUTTON_WIDTH,
                                    LIGHT_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
    }
//    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((LIGHT_BUTTON_WIDTH / 2) - 3), (LIGHT_BUTTON_HEIGHT / 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Lights\n");
    lightButtonPressed = false;
}

void waterButton() {
    touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                WATER_BUTTON_Y_ORIGIN,
                                WATER_BUTTON_WIDTH,
                                WATER_BUTTON_HEIGHT,
                                ILI9341_BLUE);
    if (waterButtonPressed) {
        touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                    WATER_BUTTON_Y_ORIGIN,
                                    WATER_BUTTON_WIDTH,
                                    WATER_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
    }
//    Show text, The cursor x is set with the len of the word / 2
    touchScreenDisplay.setCursor(((WATER_BUTTON_WIDTH / 2) + 2)), ((WATER_BUTTON_Y_ORIGIN / 2) - 2));
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Water\nScale\n");
    waterButtonPressed = true;
}

void calendarButton() {

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
