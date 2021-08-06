/* Adafruit_FT6206_Library library by Adafruit
 */

#include "Adafruit_FT6206_Library.h"

/**
 * Constructor.
 */
Adafruit_FT6206_Library::Adafruit_FT6206_Library()
{
  // be sure not to call anything that requires hardware be initialized here, put those in begin()
}

/**
 * Example method.
 */
void Adafruit_FT6206_Library::begin()
{
    // initialize hardware
    Serial.println("called begin");
}

/**
 * Example method.
 */
void Adafruit_FT6206_Library::process()
{
    // do something useful
    Serial.println("called process");
    doit();
}

/**
* Example private method
*/
void Adafruit_FT6206_Library::doit()
{
    Serial.println("called doit");
}
