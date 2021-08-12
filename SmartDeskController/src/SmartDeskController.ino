/*
*   Project: SmartDesk 1.0.1
*   Description: The code needed to run the protoType SmartDesk
*   Author: Russell Brazell
*   Date: 8-6-2021
*/

#include "Particle.h"
#include <SPI.h>
#include <Adafruit_mfGFX.h>
#include <Adafruit_ILI9341.h>
#include <Wire.h>
#include <Adafruit_FT6206_Library.h>
#include <HX711.h>
#include "credentials.h"
#include "IOTTimer.h"
#include <Adafruit_MQTT.h>
#include "Adafruit_MQTT/Adafruit_MQTT.h"
#include "Adafruit_MQTT/Adafruit_MQTT_SPARK.h"
#include <neopixel.h>

//  PINS
const int touchScreenDisplayCS = D4;
const int touchScreenDisplayDataCommand = D5;
const int NP_CALENDAR_STRIP_PIN = D7;
const int LOADCELL_DATA_PIN = A5;
const int LOADCELL_SCK_PIN = D2;
const int NP_BACKGROUND_STRIP_PIN = A2;
const int AMB_LIGHT_PIN = A3;

const int TOUCH_SENSITIVITY = 128;
const int TRANS_DELAY = 250;
const int NP_CALENDAR_STRIP_COUNT = 62;
const int NP_BACKGROUND_STRIP_COUNT = 30;
const int BRIGHTNESS_INCRAMENT = 50;

// Starting brightness
static int NPBrightness = 200;

// Flag to corrdinate all the colors 
static int shownColor;

unsigned int _timerStart;
unsigned int _timerTarget;

int dayOfMonth;
int timeOfDay;
int ambentBrightness;
int mappedAmbentBrightness;

// Arrays for the displaied colors

uint16_t colorArray[8] = {ILI9341_NAVY, ILI9341_LIGHTGREY, ILI9341_CYAN, ILI9341_GREEN, ILI9341_YELLOW, ILI9341_ORANGE,
                          ILI9341_RED, ILI9341_MAGENTA};
// TODO FIX COLORS TO BE RIGHT FOR NP's
uint32_t NPColorArray[8] = {RGB_COLOR_BLUE, RGB_COLOR_GRAY, RGB_COLOR_CYAN, RGB_COLOR_GREEN, RGB_COLOR_YELLOW,
                            RGB_COLOR_ORANGE, RGB_COLOR_RED, RGB_COLOR_MAGENTA};

// LOAD CELL

const int CAL_FACTOR = 1123; // This is calibrated to grams on the H2O scale
const int SAMPLE = 10;
const int WAIT_TIME = 3000;

const float WATER_GRAMS = 29.5735295625f;

static float weight;
float lastWeight;

float tareOffset;
float rawData;
float scaleCalibration;

bool calDone = false;

// Button presses

bool homeButtonPressed = true;
bool lightButtonPressed = false;
bool waterButtonPressed = false;
bool calendarButtonPressed = false;
bool fingerPrintButtonPressed = false;
bool newButtonPressed = false;
bool onButtonPressed = true;
bool offButtonPressed = false;
bool colorButtonPressed = false;
bool brightPlusButtonPressed = false;
bool brightLessButtonPressed = false;
bool waterScaleCalButtonPressed = false;
bool waterScaleSetButtonPressed = false;
bool setCalButtonPressed = false;

// Screen showing

bool homeButtonShowing = false;
bool lightButtonShowing = false;
bool waterButtonShowing = false;
bool calendarButtonShowing = false;
bool fingerPrintButtonShowing = false;
bool newButtonShowing = false;
bool onButtonShowing = false;
bool offButtonShowing = false;
bool colorButtonShowing = false;
bool brightPlusButtonShowing = false;
bool brightLessButtonShowing = false;
bool waterScaleCalButtonShowing = false;
bool waterScaleSetButtonShowing = false;
bool waterVolumeShowing = false;
bool setCalButtonShowing = false;

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

const int FINGERPRINT_BUTTON_X_ORIGIN = (FRAME_Y_ORIGIN + WATER_BUTTON_WIDTH);
const int FINGERPRINT_BUTTON_Y_ORIGIN = (FRAME_Y_ORIGIN + WATER_BUTTON_HEIGHT);
const int FINGERPRINT_BUTTON_WIDTH = FRAME_WIDTH;
const int FINGERPRINT_BUTTON_HEIGHT = FRAME_HEIGHT;

// ON BUTTON = top left

const int ON_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int ON_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN;
const int ON_BUTTON_WIDTH = (SCREEN_WIDTH / 3);
const int ON_BUTTON_HEIGHT = (SCREEN_HEIGHT / 2) - 30;

// OFF BUTTON = bottom left

const int OFF_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int OFF_BUTTON_Y_ORIGIN = (FRAME_Y_ORIGIN + ON_BUTTON_HEIGHT);
const int OFF_BUTTON_WIDTH = (SCREEN_WIDTH / 3);
const int OFF_BUTTON_HEIGHT = (SCREEN_HEIGHT / 2) - 30;

// COLOR BUTTON = centered

const int COLOR_BUTTON_X_ORIGIN = (SCREEN_WIDTH / 3);
const int COLOR_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN;
const int COLOR_BUTTON_WIDTH = (SCREEN_WIDTH / 3);
// SEE HOME_BUTTON_HEIGHT to set offset
const int COLOR_BUTTON_HEIGHT = (SCREEN_HEIGHT - 60);

// BRIGHT PLUS BUTTON = top right

const int BRIGHT_PLUS_X_ORIGIN = ((SCREEN_WIDTH / 3) + (SCREEN_WIDTH / 3));
const int BRIGHT_PLUS_Y_ORIGIN = FRAME_Y_ORIGIN;
const int BRIGHT_PLUS_WIDTH = (SCREEN_WIDTH / 3);
const int BRIGHT_PLUS_HEIGHT = (SCREEN_HEIGHT / 2) - 30;

// BRIGHT LESS BUTTON = bottom right

const int BRIGHT_LESS_X_ORIGIN = ((SCREEN_WIDTH / 3) + (SCREEN_WIDTH / 3));
const int BRIGHT_LESS_Y_ORIGIN = (FRAME_Y_ORIGIN + BRIGHT_PLUS_HEIGHT);
const int BRIGHT_LESS_WIDTH = (SCREEN_WIDTH / 3);
const int BRIGHT_LESS_HEIGHT = (SCREEN_HEIGHT / 2) - 30;

// HOME BUTTON = bottom

const int HOME_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int HOME_BUTTON_Y_ORIGIN = (SCREEN_HEIGHT - 60);
const int HOME_BUTTON_WIDTH = SCREEN_WIDTH;
const int HOME_BUTTON_HEIGHT = 60;

// WATER CAL BUTTON = left

const int CAL_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int CAL_BUTTON_Y_ORIGIN = FRAME_Y_ORIGIN;
const int CAL_BUTTON_WIDTH = (SCREEN_WIDTH / 3);
const int CAL_BUTTON_HEIGHT = (SCREEN_HEIGHT - HOME_BUTTON_HEIGHT);

// WATER SET BUTTON = middle bottom

const int SET_CAL_BUTTON_X_ORIGIN = FRAME_X_ORIGIN;
const int SET_CAL_BUTTON_Y_ORIGIN = SCREEN_HEIGHT / 2;
const int SET_CAL_BUTTON_WIDTH = SCREEN_WIDTH;
const int SET_CAL_BUTTON_HEIGHT = 60;

// WATER SCALE CALIBRATION INSTRUCTIONS = top

const int INSTRUCTIONS_X_ORIGIN = FRAME_X_ORIGIN;
const int INSTRUCTIONS_Y_ORIGIN = FRAME_Y_ORIGIN;
const int INSTRUCTIONS_WIDTH = SCREEN_WIDTH;
const int INSTRUCTIONS_HEIGHT = (SCREEN_HEIGHT - SET_CAL_BUTTON_HEIGHT - HOME_BUTTON_HEIGHT);

// MQTT vars
unsigned long last;
unsigned long lastTime;

String onlyDate;
String onlyTime;

//  Display Screen uses SPI --SEE PINS--
Adafruit_ILI9341 touchScreenDisplay(touchScreenDisplayCS, touchScreenDisplayDataCommand);

// Touch Screen uses hardware I2C (SCL/SDA)
Adafruit_FT6206 capacitiveTouchScreen = Adafruit_FT6206();

// Loadcell
HX711 H2Oscale(LOADCELL_DATA_PIN, LOADCELL_SCK_PIN);

// Timers
IOTTimer connectTimer;
IOTTimer publishTimer;
IOTTimer brightnessTimer;

// MQTT Constructors 
TCPClient TheClient;

Adafruit_MQTT_SPARK mqtt(&TheClient, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

// Publish
Adafruit_MQTT_Publish mqttPubWaterWeight = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME
"/feeds/waterweight");

// NeoPixel
Adafruit_NeoPixel NPCalendarStrip(NP_CALENDAR_STRIP_COUNT, NP_CALENDAR_STRIP_PIN, WS2812);
Adafruit_NeoPixel NPBackGroundStrip(NP_BACKGROUND_STRIP_COUNT, NP_BACKGROUND_STRIP_PIN, WS2812);

// Starting the desk up
void setup() {
    Serial.begin(115200);
    waitFor(Serial.isConnected, 3000);
    timeSetUp();
    Serial.println("Time Setup");
    NPSetUp();
    Serial.println("NP's Setup");
    displaySetUp();
    Serial.println("Display Setup");
    scaleSetUp();
    Serial.println("Scale Setup");
    setUpTouchScreen();
    Serial.println("TouchScreen Setup");
    outLineCalendarNP();
    Serial.println("Calendar NP's Setup");
    // Uncomment to be able to read the prints
    // delay(5000);
}

// Largely controlled by the touch screen menus
void loop() {
    menuSelect();
    nightLighting();
}

// Funtion to determin what menu the user is on
void menuSelect() {
    if (homeButtonPressed) {
        homeMenu();
    }
    if (lightButtonPressed) {
        lightButtonMenu();
    }
    if (waterButtonPressed) {
        waterButtonMenu();
    }
    if (waterScaleCalButtonPressed) {
        setCalMenu();
    }
}

// Function to ensure the time function of Particle to connected and to get the day
void timeSetUp() {
    dayOfMonth = Time.day(-6);
    Serial.printf("The day is %i\n", dayOfMonth);
}

// Function to start the touch part of the touch screen display
void setUpTouchScreen() {
    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    capacitiveTouchScreen.begin(TOUCH_SENSITIVITY);
    delay(100);
    // origin = left, top landscape (Reset button left upper)
    goToHomeMenu();
    newButtonPressed = false;
}

// Function to start the dislpay part of the touch screen display
void displaySetUp() {
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

    Serial.println("Done with diagnostics!");
    //    End diagnostics
    touchScreenDisplay.setRotation(1);
    //    testFillScreen();
    // testRects(ILI9341_PINK);
    //    Welcome message

    touchScreenDisplay.fillScreen(ILI9341_BLACK);
    touchScreenDisplay.setCursor(0, 80);
    touchScreenDisplay.setTextColor(ILI9341_GREEN);
    touchScreenDisplay.setTextSize(4);
    touchScreenDisplay.printf("Welcome to\nSmart Desk\n1.0.1\n");

    // delay(5000);
}

// Function to start the water scale
void scaleSetUp() {
    WiFi.connect();
    setWaterScaleCal();
    MQTT_connect();
    publishTimer.startTimer(30000);
}

// Function to start the NPs
void NPSetUp() {
    pinMode(AMB_LIGHT_PIN, INPUT);
    brightnessTimer.startTimer(100);
    // Background Strip
    NPBackGroundStrip.setBrightness(NPBrightness);
    NPBackGroundStrip.begin();
    NPBackGroundStrip.show();
    delay(100);
    NPCalendarStrip.setBrightness(NPBrightness);
    NPCalendarStrip.begin();
    NPCalendarStrip.show();
    fillBackGroundStrip(NPColorArray[0]);
    // Calendar Strip
    // Set to green  
    NPCalendarStrip.clear();
    NPCalendarStrip.show();
}

// Function to set the calendar NPs correctly for the given day
void outLineCalendarNP() {
    NPCalendarStrip.clear();
    for (int i = 0; i < NP_CALENDAR_STRIP_COUNT; i += 2) {
        NPCalendarStrip.setPixelColor(i, NPColorArray[0]);
    }
    // TODO map to the right day because its every 2nd one
    NPCalendarStrip.setPixelColor((12 * 2) - 1, NPColorArray[6]);
    NPCalendarStrip.show();
}

// Function to determin what to do on the home menu
void homeMenu() {
    Serial.printf("Checking if home menu touched anywhere\n");
    homeButtonMenuSelect();
    if (newButtonPressed) {
        if (!lightButtonShowing) {
            lightButtonShowing = true;
            lightButton();
        }
        if (!waterButtonShowing) {
            waterButtonShowing = true;
            waterButton();
        }
        if (!calendarButtonShowing) {
            calendarButtonShowing = true;
            calendarButton();
        }
        if (!fingerPrintButtonShowing) {
            fingerPrintButtonShowing = true;
            fingerPrintButton();
        }
        newButtonPressed = false;
    }
}

// How to show the lightButton on the screen
void lightButton() {
    if (lightButtonPressed) {
        touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                    LIGHT_BUTTON_Y_ORIGIN,
                                    LIGHT_BUTTON_WIDTH,
                                    LIGHT_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
        touchScreenDisplay.setCursor(((LIGHT_BUTTON_WIDTH / 2) - 35), (LIGHT_BUTTON_HEIGHT / 2) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Lights\n");
        lightButtonShowing = false;
        delay(TRANS_DELAY);
        homeButtonPressed = false;
        onButtonShowing = false;
        offButtonShowing = false;
        colorButtonShowing = false;
        brightPlusButtonShowing = false;
        brightLessButtonShowing = false;
        homeButtonShowing = false;
        lightButtonMenu();
    } else {
        touchScreenDisplay.fillRect(LIGHT_BUTTON_X_ORIGIN,
                                    LIGHT_BUTTON_Y_ORIGIN,
                                    LIGHT_BUTTON_WIDTH,
                                    LIGHT_BUTTON_HEIGHT,
                                    ILI9341_RED);
        //        TODO make a color array to glow the button
        //    Show text
        touchScreenDisplay.setCursor(((LIGHT_BUTTON_WIDTH / 2) - 35), (LIGHT_BUTTON_HEIGHT / 2) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Lights\n");
    }
}

// How to show the waterButton on the screen
void waterButton() {
    if (waterButtonPressed) {
        touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                    WATER_BUTTON_Y_ORIGIN,
                                    WATER_BUTTON_WIDTH,
                                    WATER_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
        //    Show text
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     ((WATER_BUTTON_HEIGHT / 2) - 12));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Water\n");
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     ((WATER_BUTTON_HEIGHT / 2) + 6));
        touchScreenDisplay.printf("Scale\n");
        waterButtonShowing = false;
        delay(TRANS_DELAY);
        homeButtonPressed = false;
        homeButtonShowing = false;
        lightButtonPressed = false;
        fingerPrintButtonPressed = false;
        calendarButtonPressed = false;
        waterScaleCalButtonShowing = false;
        waterVolumeShowing = false;
        waterButtonMenu();
    } else {
        touchScreenDisplay.fillRect(WATER_BUTTON_X_ORIGIN,
                                    WATER_BUTTON_Y_ORIGIN,
                                    WATER_BUTTON_WIDTH,
                                    WATER_BUTTON_HEIGHT,
                                    ILI9341_BLUE);
        //    Show text
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     ((WATER_BUTTON_HEIGHT / 2) - 12));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Water\n");
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     ((WATER_BUTTON_HEIGHT / 2) + 6));
        touchScreenDisplay.printf("Scale\n");
    }
}

// How to show the calendarButton on the screen
void calendarButton() {
    if (calendarButtonPressed) {
        touchScreenDisplay.fillRect(CALENDAR_BUTTON_X_ORIGIN,
                                    CALENDAR_BUTTON_Y_ORIGIN,
                                    CALENDAR_BUTTON_WIDTH,
                                    CALENDAR_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
        //    Show text
        touchScreenDisplay.setCursor(((CALENDAR_BUTTON_WIDTH / 2) - 45),
                                     ((CALENDAR_BUTTON_Y_ORIGIN / 2) + LIGHT_BUTTON_HEIGHT) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Calendar\n");
        delay(TRANS_DELAY);
    } else {
        touchScreenDisplay.fillRect(CALENDAR_BUTTON_X_ORIGIN,
                                    CALENDAR_BUTTON_Y_ORIGIN,
                                    CALENDAR_BUTTON_WIDTH,
                                    CALENDAR_BUTTON_HEIGHT,
                                    ILI9341_MAROON);
        //    Show text
        touchScreenDisplay.setCursor(((CALENDAR_BUTTON_WIDTH / 2) - 45),
                                     ((CALENDAR_BUTTON_Y_ORIGIN / 2) + LIGHT_BUTTON_HEIGHT) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Calendar\n");
    }
}

// How to show the fingerPrintButton on the screen
void fingerPrintButton() {
    if (fingerPrintButtonPressed) {
        touchScreenDisplay.fillRect(FINGERPRINT_BUTTON_X_ORIGIN,
                                    FINGERPRINT_BUTTON_Y_ORIGIN,
                                    FINGERPRINT_BUTTON_WIDTH,
                                    FRAME_HEIGHT,
                                    ILI9341_PURPLE);
        //    Show text
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     (((WATER_BUTTON_HEIGHT / 2) - 12) + FINGERPRINT_BUTTON_Y_ORIGIN));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Finger\n");
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     (((WATER_BUTTON_HEIGHT / 2) + 6) + FINGERPRINT_BUTTON_Y_ORIGIN));
        touchScreenDisplay.printf("Print\n");
        delay(TRANS_DELAY);
    } else {
        touchScreenDisplay.fillRect(FINGERPRINT_BUTTON_X_ORIGIN,
                                    FINGERPRINT_BUTTON_Y_ORIGIN,
                                    FINGERPRINT_BUTTON_WIDTH,
                                    FINGERPRINT_BUTTON_HEIGHT,
                                    ILI9341_ORANGE);
        //    Show text
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     (((WATER_BUTTON_HEIGHT / 2) - 12) + FINGERPRINT_BUTTON_Y_ORIGIN));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Finger\n");
        touchScreenDisplay.setCursor((((WATER_BUTTON_WIDTH / 2) - 25) + LIGHT_BUTTON_WIDTH),
                                     (((WATER_BUTTON_HEIGHT / 2) + 6) + FINGERPRINT_BUTTON_Y_ORIGIN));
        touchScreenDisplay.printf("Print\n");
    }
}

// Function to determin which button is pressed
void homeButtonMenuSelect() {
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
        lightButtonShowing = false;
        waterButtonPressed = false;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > WATER_BUTTON_X_ORIGIN)
               && (x < (WATER_BUTTON_X_ORIGIN + WATER_BUTTON_WIDTH))
               && (y > WATER_BUTTON_Y_ORIGIN)
               && (y < (WATER_BUTTON_Y_ORIGIN + WATER_BUTTON_HEIGHT))) {
        Serial.printf("Water Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = true;
        waterButtonShowing = false;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > CALENDAR_BUTTON_X_ORIGIN)
               && (x < CALENDAR_BUTTON_X_ORIGIN + CALENDAR_BUTTON_WIDTH)
               && (y > CALENDAR_BUTTON_Y_ORIGIN)
               && (y < (CALENDAR_BUTTON_Y_ORIGIN + CALENDAR_BUTTON_HEIGHT))) {
        Serial.printf("Calendar Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = false;
        calendarButtonPressed = true;
        calendarButtonShowing = false;
        fingerPrintButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > FINGERPRINT_BUTTON_X_ORIGIN)
               && (x < FINGERPRINT_BUTTON_X_ORIGIN + FINGERPRINT_BUTTON_WIDTH)
               && (y > FINGERPRINT_BUTTON_Y_ORIGIN)
               && (y < (FINGERPRINT_BUTTON_Y_ORIGIN + FINGERPRINT_BUTTON_HEIGHT))) {
        Serial.printf("Finger Print Button Pressed\n");
        lightButtonPressed = false;
        waterButtonPressed = false;
        calendarButtonPressed = false;
        fingerPrintButtonPressed = true;
        fingerPrintButtonShowing = false;
        newButtonPressed = true;
    }
}

// Function to get back to the home menu from any other menu
void goToHomeMenu() {
    homeButtonPressed = true;

    lightButtonPressed = false;
    lightButtonShowing = false;

    waterButtonPressed = false;
    waterButtonShowing = false;

    calendarButtonPressed = false;
    calendarButtonShowing = false;

    fingerPrintButtonPressed = false;
    fingerPrintButtonShowing = false;

    newButtonPressed = true;
    setCalButtonPressed = false;
    waterScaleCalButtonPressed = false;
    homeMenu();
}

// How to show the homeButton on the screen
void homeButton() {
    if (homeButtonPressed) {
        touchScreenDisplay.fillRect(HOME_BUTTON_X_ORIGIN,
                                    HOME_BUTTON_Y_ORIGIN,
                                    HOME_BUTTON_WIDTH,
                                    HOME_BUTTON_HEIGHT,
                                    ILI9341_RED);
        touchScreenDisplay.setCursor((HOME_BUTTON_WIDTH / 2) - 20,
                                     ((HOME_BUTTON_HEIGHT / 2) - 4) + COLOR_BUTTON_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Home\n");
        delay(TRANS_DELAY);
        goToHomeMenu();
    } else if (calDone) {
        touchScreenDisplay.fillRect(HOME_BUTTON_X_ORIGIN,
                                    HOME_BUTTON_Y_ORIGIN,
                                    HOME_BUTTON_WIDTH,
                                    HOME_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
        touchScreenDisplay.setCursor((HOME_BUTTON_WIDTH / 2) - 20,
                                     ((HOME_BUTTON_HEIGHT / 2) - 4) + COLOR_BUTTON_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Home\n");
    } else {
        touchScreenDisplay.fillRect(HOME_BUTTON_X_ORIGIN,
                                    HOME_BUTTON_Y_ORIGIN,
                                    HOME_BUTTON_WIDTH,
                                    HOME_BUTTON_HEIGHT,
                                    ILI9341_LIGHTGREY);
        touchScreenDisplay.setCursor((HOME_BUTTON_WIDTH / 2) - 20,
                                     ((HOME_BUTTON_HEIGHT / 2) - 4) + COLOR_BUTTON_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Home\n");
    }
}

// Function to determin what to do in the light menu
void lightButtonMenu() {
    Serial.printf("Light menu waiting for touch\n");
    lightButtonMenuSelect();
    if (newButtonPressed) {
        if (!onButtonShowing) {
            onButtonShowing = true;
            onButton();
        }
        if (!offButtonShowing) {
            offButtonShowing = true;
            offButton();
        }
        if (!colorButtonShowing) {
            colorButtonShowing = true;
            colorButton();
        }
        if (!brightPlusButtonShowing) {
            brightPlusButtonShowing = true;
            brightPlusButton();
        }
        if (!brightLessButtonShowing) {
            brightLessButtonShowing = true;
            brightLessButton();
        }
        if (!homeButtonShowing) {
            homeButtonShowing = true;
            homeButton();
        }
        newButtonPressed = false;
    }
}

// Function to determin what button is pressed on the light menu
void lightButtonMenuSelect() {
    // Retrieve a point
    TS_Point touchedPoint = capacitiveTouchScreen.getPoint();
    // flip it around to match the screen.
    touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
    touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
    int y = touchScreenDisplay.height() - touchedPoint.x;
    int x = touchedPoint.y;
    if ((x > ON_BUTTON_X_ORIGIN)
        && (x < ON_BUTTON_X_ORIGIN + ON_BUTTON_WIDTH)
        && (y > ON_BUTTON_Y_ORIGIN)
        && (y < ON_BUTTON_Y_ORIGIN + ON_BUTTON_HEIGHT)) {
        onButtonPressed = true;
        onButtonShowing = false;
        offButtonPressed = false;
        colorButtonPressed = false;
        brightPlusButtonPressed = false;
        brightLessButtonPressed = false;
        homeButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > OFF_BUTTON_X_ORIGIN)
               && (x < OFF_BUTTON_X_ORIGIN + ON_BUTTON_WIDTH)
               && (y > OFF_BUTTON_Y_ORIGIN)
               && (y < OFF_BUTTON_Y_ORIGIN + OFF_BUTTON_HEIGHT)) {
        onButtonPressed = false;
        offButtonPressed = true;
        offButtonShowing = false;
        colorButtonPressed = false;
        brightPlusButtonPressed = false;
        brightLessButtonPressed = false;
        homeButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > COLOR_BUTTON_X_ORIGIN)
               && (x < COLOR_BUTTON_X_ORIGIN + COLOR_BUTTON_WIDTH)
               && (y > COLOR_BUTTON_Y_ORIGIN)
               && (y < COLOR_BUTTON_Y_ORIGIN + COLOR_BUTTON_HEIGHT)) {
        onButtonPressed = false;
        offButtonPressed = false;
        colorButtonPressed = true;
        colorButtonShowing = false;
        brightPlusButtonPressed = false;
        brightLessButtonPressed = false;
        homeButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > BRIGHT_PLUS_X_ORIGIN)
               && (x < BRIGHT_PLUS_X_ORIGIN + BRIGHT_PLUS_WIDTH)
               && (y > BRIGHT_PLUS_Y_ORIGIN)
               && (y < BRIGHT_PLUS_Y_ORIGIN + BRIGHT_PLUS_HEIGHT)) {
        onButtonPressed = false;
        offButtonPressed = false;
        colorButtonPressed = false;
        brightPlusButtonPressed = true;
        brightPlusButtonShowing = false;
        brightLessButtonPressed = false;
        homeButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > BRIGHT_LESS_X_ORIGIN)
               && (x < BRIGHT_LESS_X_ORIGIN + BRIGHT_LESS_WIDTH)
               && (y > BRIGHT_LESS_Y_ORIGIN)
               && (y < BRIGHT_LESS_Y_ORIGIN + BRIGHT_LESS_HEIGHT)) {
        onButtonPressed = false;
        offButtonPressed = false;
        colorButtonPressed = false;
        brightPlusButtonPressed = false;
        brightLessButtonPressed = true;
        brightLessButtonShowing = false;
        homeButtonPressed = false;
        newButtonPressed = true;
    } else if ((x > HOME_BUTTON_X_ORIGIN)
               && (x < HOME_BUTTON_X_ORIGIN + HOME_BUTTON_WIDTH)
               && (y > HOME_BUTTON_Y_ORIGIN)
               && (y < HOME_BUTTON_Y_ORIGIN + HOME_BUTTON_HEIGHT)) {
        onButtonPressed = false;
        offButtonPressed = false;
        colorButtonPressed = false;
        brightPlusButtonPressed = false;
        brightLessButtonPressed = true;
        homeButtonPressed = true;
        homeButtonShowing = false;
        newButtonPressed = true;
    }
}

// How to show the onButton on the screen
void onButton() {
    if (onButtonPressed) {
        touchScreenDisplay.fillRect(ON_BUTTON_X_ORIGIN,
                                    ON_BUTTON_Y_ORIGIN,
                                    ON_BUTTON_WIDTH,
                                    ON_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
        //    Show text
        touchScreenDisplay.setCursor((ON_BUTTON_WIDTH / 2) - 20, (ON_BUTTON_HEIGHT / 2) - 4);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("ON\n");
        delay(TRANS_DELAY);
        turnOnBackGroundNP();
        offButton();
    } else {
        touchScreenDisplay.fillRect(ON_BUTTON_X_ORIGIN,
                                    ON_BUTTON_Y_ORIGIN,
                                    ON_BUTTON_WIDTH,
                                    ON_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
        //    Show text
        touchScreenDisplay.setCursor((ON_BUTTON_WIDTH / 2) - 20, (ON_BUTTON_HEIGHT / 2) - 4);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("ON\n");
    }
}

// How to show the offButton on the screen
void offButton() {
    if (offButtonPressed) {
        touchScreenDisplay.fillRect(OFF_BUTTON_X_ORIGIN,
                                    OFF_BUTTON_Y_ORIGIN,
                                    OFF_BUTTON_WIDTH,
                                    OFF_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
        //        Show text
        touchScreenDisplay.setCursor((OFF_BUTTON_WIDTH / 2) - 20, ((OFF_BUTTON_HEIGHT / 2) - 4) + ON_BUTTON_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("OFF\n");
        delay(TRANS_DELAY);
        turnOffBackGroundNP();
        onButton();
    } else {
        touchScreenDisplay.fillRect(OFF_BUTTON_X_ORIGIN,
                                    OFF_BUTTON_Y_ORIGIN,
                                    OFF_BUTTON_WIDTH,
                                    OFF_BUTTON_HEIGHT,
                                    ILI9341_RED);
        //        Show text
        touchScreenDisplay.setCursor((OFF_BUTTON_WIDTH / 2) - 20, ((OFF_BUTTON_HEIGHT / 2) - 4) + ON_BUTTON_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("OFF\n");
    }
}

// How to show the colorButton on the screen
void colorButton() {
    if (colorButtonPressed) {
        touchScreenDisplay.fillRect(COLOR_BUTTON_X_ORIGIN,
                                    COLOR_BUTTON_Y_ORIGIN,
                                    COLOR_BUTTON_WIDTH,
                                    COLOR_BUTTON_HEIGHT,
                                    colorArray[shownColor]);
        //        Show text
        touchScreenDisplay.setCursor(((COLOR_BUTTON_WIDTH / 2) - 30) + ON_BUTTON_WIDTH, (COLOR_BUTTON_HEIGHT / 2) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Color\n");
        backGroundNP();
        delay(TRANS_DELAY);
        shownColor < 7 ? shownColor++ : shownColor = 0;
    } else {
        touchScreenDisplay.fillRect(COLOR_BUTTON_X_ORIGIN,
                                    COLOR_BUTTON_Y_ORIGIN,
                                    COLOR_BUTTON_WIDTH,
                                    COLOR_BUTTON_HEIGHT,
                                    colorArray[shownColor]);
        //        Show text
        touchScreenDisplay.setCursor(((COLOR_BUTTON_WIDTH / 2) - 30) + ON_BUTTON_WIDTH, (COLOR_BUTTON_HEIGHT / 2) - 6);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Color\n");
    }
}

// How to show the button that is a "+" to increase the NP brightness
void brightPlusButton() {
    if (brightPlusButtonPressed) {
        touchScreenDisplay.fillRect(BRIGHT_PLUS_X_ORIGIN,
                                    BRIGHT_PLUS_Y_ORIGIN,
                                    BRIGHT_PLUS_WIDTH,
                                    BRIGHT_PLUS_HEIGHT,
                                    ILI9341_DARKGREY);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_PLUS_WIDTH / 2) - 12) + ON_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     (BRIGHT_PLUS_HEIGHT / 2) - 20);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("+\n");
        plusBrightnessNP();
        backGroundNP();
        delay(TRANS_DELAY);
        brightLessButton();
        //        TODO add function to change brightness
        touchScreenDisplay.fillRect(BRIGHT_PLUS_X_ORIGIN,
                                    BRIGHT_PLUS_Y_ORIGIN,
                                    BRIGHT_PLUS_WIDTH,
                                    BRIGHT_PLUS_HEIGHT,
                                    ILI9341_CYAN);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_PLUS_WIDTH / 2) - 12) + ON_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     (BRIGHT_PLUS_HEIGHT / 2) - 20);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("+\n");
    } else {
        touchScreenDisplay.fillRect(BRIGHT_PLUS_X_ORIGIN,
                                    BRIGHT_PLUS_Y_ORIGIN,
                                    BRIGHT_PLUS_WIDTH,
                                    BRIGHT_PLUS_HEIGHT,
                                    ILI9341_CYAN);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_PLUS_WIDTH / 2) - 12) + ON_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     (BRIGHT_PLUS_HEIGHT / 2) - 20);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("+\n");
    }
}

// How to show the button that is a "-" to decrease the NP brightness
void brightLessButton() {
    if (brightLessButtonPressed) {
        touchScreenDisplay.fillRect(BRIGHT_LESS_X_ORIGIN,
                                    BRIGHT_LESS_Y_ORIGIN,
                                    BRIGHT_LESS_WIDTH,
                                    BRIGHT_LESS_HEIGHT,
                                    ILI9341_DARKGREY);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_LESS_WIDTH / 2) - 12) + OFF_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     ((BRIGHT_LESS_HEIGHT / 2) - 20) + BRIGHT_PLUS_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("-\n");
        lessBrightnessNP();
        backGroundNP();
        delay(TRANS_DELAY);
        brightPlusButton();
        //         TODO add function to change brightness
        touchScreenDisplay.fillRect(BRIGHT_LESS_X_ORIGIN,
                                    BRIGHT_LESS_Y_ORIGIN,
                                    BRIGHT_LESS_WIDTH,
                                    BRIGHT_LESS_HEIGHT,
                                    ILI9341_CYAN);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_LESS_WIDTH / 2) - 12) + OFF_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     ((BRIGHT_LESS_HEIGHT / 2) - 20) + BRIGHT_PLUS_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("-\n");
    } else {
        touchScreenDisplay.fillRect(BRIGHT_LESS_X_ORIGIN,
                                    BRIGHT_LESS_Y_ORIGIN,
                                    BRIGHT_LESS_WIDTH,
                                    BRIGHT_LESS_HEIGHT,
                                    ILI9341_CYAN);
        //        Show text
        touchScreenDisplay.setCursor(((BRIGHT_LESS_WIDTH / 2) - 12) + OFF_BUTTON_WIDTH + COLOR_BUTTON_WIDTH,
                                     ((BRIGHT_LESS_HEIGHT / 2) - 20) + BRIGHT_PLUS_HEIGHT);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(5);
        touchScreenDisplay.printf("-\n");
    }
}

// Function to determin what to do on the water menu
void waterButtonMenu() {
    Serial.printf("Water menu waiting for touch\n");
    waterButtonMenuSelect();
    if (newButtonPressed) {
        if (!waterScaleCalButtonShowing) {
            waterScaleCalButtonShowing = true;
            waterScaleCalButton();
        }
        if (!homeButtonShowing) {
            homeButtonShowing = true;
            homeButton();
        }
        if (!waterVolumeShowing) {
            waterVolumeShowing = true;
            waterVolume();
        }
        newButtonPressed = false;
    } else {
        weight = H2Oscale.get_units(SAMPLE);
        // rawData = H2Oscale.get_value(SAMPLE);
        tareOffset = H2Oscale.get_offset();
        scaleCalibration = H2Oscale.get_scale();
        Serial.printf("OZ's: %0.2f\n", getWaterOZ(weight));
        if (publishTimer.isTimerReady()) {
            publishReadings();
            waterVolume();
            publishTimer.startTimer(30000);
        }
    }
}

// Function to determin what button is pressed on the water menu
void waterButtonMenuSelect() {
    // Retrieve a point
    TS_Point touchedPoint = capacitiveTouchScreen.getPoint();
    // flip it around to match the screen.
    touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
    touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
    int y = touchScreenDisplay.height() - touchedPoint.x;
    int x = touchedPoint.y;
    if ((x > CAL_BUTTON_X_ORIGIN)
        && (x < CAL_BUTTON_WIDTH)
        && (y > CAL_BUTTON_Y_ORIGIN)
        && (y < CAL_BUTTON_HEIGHT)) {
        Serial.printf("Cal Button Pressed\n");
        waterScaleCalButtonPressed = true;
        waterScaleCalButtonShowing = false;
        setCalButtonShowing = false;
        waterButtonPressed = false;
        waterVolumeShowing = true;
        homeButtonPressed = false;
        homeButtonShowing = false;
        newButtonPressed = true;
        calDone = false;
    } else if ((x > HOME_BUTTON_X_ORIGIN)
               && (x < HOME_BUTTON_WIDTH)
               && (y > HOME_BUTTON_Y_ORIGIN)
               && (y < (HOME_BUTTON_Y_ORIGIN + HOME_BUTTON_HEIGHT))) {
        Serial.printf("Home Button Pressed\n");
        waterScaleCalButtonPressed = false;
        homeButtonShowing = false;
        homeButtonPressed = true;
        newButtonPressed = true;
    }
}

// How to show the waterScaleCalButton on the screen
void waterScaleCalButton() {
    if (waterScaleCalButtonPressed) {
        touchScreenDisplay.fillRect(CAL_BUTTON_X_ORIGIN,
                                    CAL_BUTTON_Y_ORIGIN,
                                    CAL_BUTTON_WIDTH,
                                    CAL_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
        //    Show text
        touchScreenDisplay.setCursor(0, (WATER_BUTTON_HEIGHT / 2));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Calibrate\n");
        touchScreenDisplay.setCursor(0, ((WATER_BUTTON_HEIGHT / 2) + 16));
        touchScreenDisplay.printf("Scale\n");
        delay(TRANS_DELAY);
        waterButtonPressed = false;
        setCalButtonPressed = false;
        setCalButtonShowing = false;
        waterVolumeShowing = true;
        homeButtonShowing = false;
        homeButtonPressed = false;
        showSetCalInstruction();
        setCalMenu();
    } else {
        touchScreenDisplay.fillRect(CAL_BUTTON_X_ORIGIN,
                                    CAL_BUTTON_Y_ORIGIN,
                                    CAL_BUTTON_WIDTH,
                                    CAL_BUTTON_HEIGHT,
                                    ILI9341_DARKGREEN);
        //    Show text
        touchScreenDisplay.setCursor(0, ((WATER_BUTTON_HEIGHT / 2) - 4));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Calibrate\n");
        touchScreenDisplay.setCursor(0, ((WATER_BUTTON_HEIGHT / 2) + 16));
        touchScreenDisplay.printf("Scale\n");
    }
}

// How to show the water volume left on the screen
void waterVolume() {
    touchScreenDisplay.fillRect(
            CAL_BUTTON_WIDTH,
            CAL_BUTTON_Y_ORIGIN,
            (SCREEN_WIDTH - CAL_BUTTON_WIDTH),
            CAL_BUTTON_HEIGHT,
            ILI9341_CYAN);
    touchScreenDisplay.setCursor(CAL_BUTTON_WIDTH + 12, CAL_BUTTON_Y_ORIGIN + 12);
    touchScreenDisplay.setTextColor(ILI9341_BLACK);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf("Fluid OZ's Left");
    touchScreenDisplay.setCursor((SCREEN_WIDTH / 2) - 4, (SCREEN_HEIGHT / 2) - 16);
    touchScreenDisplay.setTextColor(ILI9341_BLACK);
    touchScreenDisplay.setTextSize(6);
    touchScreenDisplay.printf("%0.1f", getWaterOZ(weight) + .01f);
}

// Function to convert from the load cell to Fluid OZ's
float getWaterOZ(float _scaleWeight) {
    float _waterVol;
    _waterVol = _scaleWeight / WATER_GRAMS;
    return _waterVol;
}

// Function to recalibrate the scale for a new water bottle
void setWaterScaleCal() {
    H2Oscale.set_scale();
    delay(WAIT_TIME);
    H2Oscale.tare(10);
    H2Oscale.set_scale(CAL_FACTOR);
    calDone = true;
}

// How to show the instructions for setting up a new water bottle
void showSetCalInstruction() {
    touchScreenDisplay.fillRect(INSTRUCTIONS_X_ORIGIN,
                                INSTRUCTIONS_Y_ORIGIN,
                                INSTRUCTIONS_WIDTH,
                                INSTRUCTIONS_HEIGHT,
                                ILI9341_BLACK);
    //    Show text
    touchScreenDisplay.setCursor(0, 5);
    touchScreenDisplay.setTextColor(ILI9341_WHITE);
    touchScreenDisplay.setTextSize(2);
    touchScreenDisplay.printf(
            "1.Place an empty water\n  bottle on the scale\n"
            "2.Press the Set button\n"
            "3.Wait for the home button  to go green\n"
            "4.All done!");
}

// Function to determin what to do on the calibration menu
void setCalMenu() {
    Serial.printf("Water Set Cal waiting for touch\n");
    waterScaleCalButtonMenuSelect();
    if (newButtonPressed) {
        if (!setCalButtonShowing) {
            setCalButtonShowing = true;
            SetCalButton();
        }
        if (!homeButtonShowing) {
            homeButtonShowing = true;
            homeButton();
        }
        newButtonPressed = false;
    }
}

// Function to determin what button is pressed on the calibration menu
void waterScaleCalButtonMenuSelect() {
    // Retrieve a point
    TS_Point touchedPoint = capacitiveTouchScreen.getPoint();
    // flip it around to match the screen.
    touchedPoint.x = map(touchedPoint.x, 0, 240, 240, 0);
    touchedPoint.y = map(touchedPoint.y, 0, 320, 320, 0);
    int y = touchScreenDisplay.height() - touchedPoint.x;
    int x = touchedPoint.y;
    if ((x > SET_CAL_BUTTON_X_ORIGIN)
        && (x < SET_CAL_BUTTON_WIDTH)
        && (y > SET_CAL_BUTTON_Y_ORIGIN)
        && (y < SET_CAL_BUTTON_Y_ORIGIN + SET_CAL_BUTTON_HEIGHT)) {
        Serial.printf("Set Button Pressed\n");
        setCalButtonPressed = true;
        setCalButtonShowing = false;
        waterVolumeShowing = true;
        homeButtonPressed = false;
        homeButtonShowing = false;
        calDone = false;
        newButtonPressed = true;
    } else if ((x > HOME_BUTTON_X_ORIGIN)
               && (x < HOME_BUTTON_WIDTH)
               && (y > HOME_BUTTON_Y_ORIGIN)
               && (y < (HOME_BUTTON_Y_ORIGIN + HOME_BUTTON_HEIGHT))) {
        Serial.printf("Home Button Pressed\n");
        setCalButtonPressed = false;
        setCalButtonShowing = true;
        homeButtonPressed = true;
        homeButtonShowing = false;
        calDone = false;
        newButtonPressed = true;
    }
}

// How to show the calibration button on the screen
void SetCalButton() {
    if (setCalButtonPressed) {
        touchScreenDisplay.fillRect(SET_CAL_BUTTON_X_ORIGIN,
                                    SET_CAL_BUTTON_Y_ORIGIN,
                                    SET_CAL_BUTTON_WIDTH,
                                    SET_CAL_BUTTON_HEIGHT,
                                    ILI9341_DARKGREY);
        touchScreenDisplay.setCursor((SET_CAL_BUTTON_WIDTH / 2) - 70,
                                     SET_CAL_BUTTON_Y_ORIGIN + (SET_CAL_BUTTON_HEIGHT / 2) - 4);
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Set Calibration\n");
        setWaterScaleCal();
        delay(TRANS_DELAY);
    } else {
        touchScreenDisplay.fillRect(SET_CAL_BUTTON_X_ORIGIN,
                                    SET_CAL_BUTTON_Y_ORIGIN,
                                    SET_CAL_BUTTON_WIDTH,
                                    SET_CAL_BUTTON_HEIGHT,
                                    ILI9341_GREEN);
        touchScreenDisplay.setCursor((SET_CAL_BUTTON_WIDTH / 2) - 60,
                                     SET_CAL_BUTTON_Y_ORIGIN + (SET_CAL_BUTTON_HEIGHT / 2));
        touchScreenDisplay.setTextColor(ILI9341_WHITE);
        touchScreenDisplay.setTextSize(2);
        touchScreenDisplay.printf("Set Calibration\n");
    }
}

// Funtion to fill the NP's with one soild color
void fillBackGroundStrip(uint16_t _HexColor) {
    NPBackGroundStrip.clear();
    for (int i = 0; i < NP_BACKGROUND_STRIP_COUNT; i++) {
        NPBackGroundStrip.setPixelColor(i, _HexColor);
    }
    NPBackGroundStrip.show();
}

// Function to update the background NP's color or brightness
void backGroundNP() {
    nightLighting();
    NPBackGroundStrip.clear();
    fillBackGroundStrip(NPColorArray[shownColor]);
}

// Function to increase the brightness of the background NPs
void plusBrightnessNP() {
    Serial.println("PLUS * PLUS");
    NPBackGroundStrip.setBrightness(NPBrightness += BRIGHTNESS_INCRAMENT);
    NPBackGroundStrip.show();
}

// Function to decrease the brightness of the background NPs
void lessBrightnessNP() {
    Serial.println("LESS * LESS");
    NPBackGroundStrip.setBrightness(NPBrightness -= BRIGHTNESS_INCRAMENT);
    NPBackGroundStrip.show();
}

// Function to turn off the background NPs
void turnOffBackGroundNP() {
    Serial.println("OFF * OFF");
    NPBackGroundStrip.setBrightness(0);
    NPBackGroundStrip.show();
}

// Function to turn on the background NPs
void turnOnBackGroundNP() {
    Serial.println("ON * ON");
    NPBackGroundStrip.setBrightness(NPBrightness);
    fillBackGroundStrip(NPColorArray[shownColor]);
    NPBackGroundStrip.show();
}

// Function to adjust the background NPs for the given light conditions and time of day
void  nightLighting() {
    timeOfDay = Time.hour();
    if (!lightButtonPressed && brightnessTimer.isTimerReady()) {
    ambentBrightness = analogRead(AMB_LIGHT_PIN);
    mappedAmbentBrightness = map(ambentBrightness, 0, 2200, 1, 254);  
    NPBackGroundStrip.setBrightness(mappedAmbentBrightness);
    NPBrightness = mappedAmbentBrightness;
        if (timeOfDay > 18) {
            NPBackGroundStrip.setBrightness(NPBrightness / 2);
            fillBackGroundStrip(NPColorArray[shownColor]);
            NPBackGroundStrip.show();
        }
    // fillBackGroundStrip(NPColorArray[shownColor]);
    NPBackGroundStrip.show();  
    brightnessTimer.startTimer(100);
    }
}

// Function to send the readings from the water scale to the adafruit dashboard
void publishReadings() {
    MQTT_connect();
    if ((millis() - last) > 120000) {
        Serial.printf("Pinging MQTT \n");
        if (!mqtt.ping()) {
            Serial.printf("Disconnecting \n");
            mqtt.disconnect();
        }
        last = millis();
    }
    if ((millis() - lastTime > 30000)) {
        if (mqtt.Update()) {
            mqttPubWaterWeight.publish(getWaterOZ(weight));
        }
        lastTime = millis();
    }
}

// Function to ensure connection to the adafruit dashboard **Credit to Brian Rashap**
void MQTT_connect() {
    int8_t ret;
    // Stop if already connected.
    if (mqtt.connected()) {
        return;
    }
    Serial.print("Connecting to MQTT... ");
    while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
        Serial.printf("%s\n", (char *) mqtt.connectErrorString(ret));
        Serial.printf("Retrying MQTT connection in 5 seconds..\n");
        mqtt.disconnect();
        connectTimer.startTimer(5000);
        while (!connectTimer.isTimerReady());
    }
    Serial.printf("MQTT Connected!\n");
}

// Function to check that the screen is working by showing a few solid colors
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

// Function to draw a fram around the soild colors
void drawFrame() {
    touchScreenDisplay.drawRect(FRAME_X_ORIGIN, FRAME_Y_ORIGIN, FRAME_WIDTH, FRAME_HEIGHT, ILI9341_BLACK);
}

// Function to draw reactangles in side each other to test the screen, also looks pretty neat
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

// Function to draw reactangles in side each other to test the screen, also looks pretty neat 
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
