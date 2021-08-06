/******************************************************/
//       THIS IS A GENERATED FILE - DO NOT EDIT       //
/******************************************************/

#include "Particle.h"
#line 1 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreen/src/SmartDeskScreen.ino"
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
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341_RK.h>

void setup();
void loop(void);
void displaySetUp();
unsigned long testFillScreen();
unsigned long testRects(uint16_t color);
unsigned long testFilledRects(uint16_t color1, uint16_t color2);
#line 19 "c:/Users/Russell/Desktop/IoT/projects/SmartDesk/SmartDeskScreen/src/SmartDeskScreen.ino"
#define TFT_DC   D5
#define TFT_CS   D4
// #define STMPE_CS D3
// #define SD_CS    D2

Adafruit_ILI9341 tft1(TFT_CS, TFT_DC); 

void setup() {
displaySetUp();
}


void loop(void) {
  testFilledRects(ILI9341_CYAN, ILI9341_DARKGREY);
  testRects(ILI9341_DARKGREEN);
}

void displaySetUp() {
  Serial.begin(115200);

  delay(10);
  Serial.println("FeatherWing TFT Test!");

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
  
  Serial.println(F("Benchmark                Time (microseconds)"));
  delay(10);
  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(ILI9341_GREEN));
  delay(500);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(ILI9341_YELLOW, ILI9341_MAGENTA));
  delay(500);

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
  int           n, i, i2,
                cx = tft.width()  / 2,
                cy = tft.height() / 2;

  tft.fillScreen(ILI9341_BLACK);
  n     = min(tft.width(), tft.height());
  start = micros();
  for(i=2; i<n; i+=6) {
    i2 = i / 2;
    tft.drawRect(cx-i2, cy-i2, i, i, color);
  }

  return micros() - start;
}

unsigned long testFilledRects(uint16_t color1, uint16_t color2) {
  unsigned long start, t = 0;
  int           n, i, i2,
                cx = tft.width()  / 2 - 1,
                cy = tft.height() / 2 - 1;

  tft.fillScreen(ILI9341_BLACK);
  n = min(tft.width(), tft.height());
  for(i=n; i>0; i-=6) {
    i2    = i / 2;
    start = micros();
    tft.fillRect(cx-i2, cy-i2, i, i, color1);
    t    += micros() - start;
    // Outlines are not included in timing results
    tft.drawRect(cx-i2, cy-i2, i, i, color2);
    yield();
  }

  return t;
}
