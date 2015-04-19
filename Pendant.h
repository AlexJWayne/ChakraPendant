/*

Pendant LED mapping

* 7 LEDs
* first (0) is the center
* second (1) is the top
* sprials clockwise around until the seventh (6)

        \         /
         \       /
          \     /
           \   /
           _\_/_
         /       \
        /    1    \
       |  6     2  |
       |     0     |
       |  5     3  |
        \    4    /
         \_______/
           |USB|

*/

#ifndef Pendant_h
#define Pendant_h

#include "Arduino.h"
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "FastLED.h"

// milliseconds you have to hold the button down to power off.
#define PENDANT_SLEEP_DELAY 1000

// milliseconds you have to hold the button down to toggle mode cycling.
#define PENDANT_CYCLE_DELAY 3000

// milliseconds each mode is shown for during cycling.
#define PENDANT_CYCLE_DURATION 10000

// How many LEDs
#define PENDANT_NUM_LEDS 7

// 8 bit fraction of the portion of a circle each LED represents to its neighbors
#define PENDANT_LED_ANGLE 255 / 6


/* ATTINY84
             +-\/-+
       VCC  1|    |14  GND
(D10)  PB0  2|    |13  AREF (D 0)
(D 9)  PB1  3|    |12  PA1  (D 1)
       PB3  4|    |11  PA2  (D 2)
(D 8)  PB2  5|    |10  PA3  (D 3)
(D 7)  PA7  6|    |9   PA4  (D 4)
(D 6)  PA6  7|    |8   PA5  (D 5)
             +----+            */

// Pin mappings
#define PENDANT_BUTTON_PIN    0
#define PENDANT_LED_PWR_PIN   1
#define PENDANT_LED_CLOCK_PIN 2
#define PENDANT_LED_DATA_PIN  3

class Pendant {
  unsigned long now = 0;

  // What to do on button release
  bool sleepOnRelease = false;
  bool changeModeOnRelease = true;

  // How long it's been since the last mode change in cycling mode
  unsigned long lastCycle = 0;

  // button state variables
  bool buttonPrevPressed        = false;
  bool buttonPrevReleased       = true;
  unsigned long buttonPressedAt = 0;

  public:

    // The LEDs. What it's all about!
    CRGB leds[PENDANT_NUM_LEDS];

    const uint8_t ledCount = PENDANT_NUM_LEDS;
    const uint8_t ledAngle = PENDANT_LED_ANGLE;

    // Maximimum brightness (0-255)
    uint8_t brightness = 20;

    // track the current mode
    uint8_t modes;
    uint8_t mode = 0;
    bool cyclingMode = false;

    //
    // METHODS
    //
    Pendant(uint8_t modes);
    void sleepNow();
    bool update();

    bool updateButtonState();
    bool buttonReleased();

    void toggleCycleMode();
    void enableCycleMode();
    void disableCycleMode();

    void enableLEDs();
    void disableLEDs();

    void setBrightness(uint8_t brightness);
    void show();
};

#endif
