/**
 * @file SmartDesk
 * @author Russell Brazell 
 * @brief The code needed to run the non-sit/stand version of the smart desk protype
 * @version 0.1
 * @date 2021-08-03
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include <Adafruit_Fingerprint.h>

SYSTEM_MODE(MANUAL);

int enrollmentCounter;

Adafruit_Fingerprint FPScanner = Adafruit_Fingerprint(&Serial1);

void setup() {
  Serial.begin(57600);
  waitFor(Serial.isConnected, 3000);
  FPScanner.begin(57600);
  delay(3000);

}

void loop() {
  while (Serial.available()) {
    Serial1.write(Serial.read());
  }
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
}