// Example usage for Adafruit_FT6206_Library library by Adafruit.

#include "Adafruit_FT6206_Library.h"

// Initialize objects from the lib
Adafruit_FT6206_Library adafruit_FT6206_Library;

void setup() {
    // Call functions on initialized library objects that require hardware
    adafruit_FT6206_Library.begin();
}

void loop() {
    // Use the library's initialized objects and functions
    adafruit_FT6206_Library.process();
}
