/*
*   Project: SmartDesk 0.1.1
*   Description: The code needed to run the protoType SmartDesk
*   Author: Russell Brazell
*   Date: 8-6-2021
*/

#include "touchScreen.h"

SYSTEM_MODE(MANUAL);

void setup() {
    setUpTouchScreen();
}

void loop() {
   runTouchScreen();
}
