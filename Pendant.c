
                                                       /* Blinker Demo */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/interrupt.h>
#include "light_ws2812.h"

#define NUM_LEDS 1
struct cRGB leds[NUM_LEDS];

uint8_t h = 0;
uint16_t animatedHue = 0;
#define SPEED 3;

uint8_t mode = 0;

uint8_t colors[7] = {
  0,   // red
  20,  // orange
  40,  // yellow
  85,  // green
  135, // blue
  190, // indigo
  205  // violet
};

void setHue(uint8_t i, uint8_t hue, uint8_t brightness) {
  hue = 255 - hue;
  if (hue < 85) {
    leds[i].r = 255 - hue * 3;
    leds[i].g = 0;
    leds[i].b = hue * 3;
 } else if (hue < 170) {
    hue -= 85;
    leds[i].r = 0;
    leds[i].g = hue * 3;
    leds[i].b = 255 - hue * 3;
  } else {
    hue -= 170;
    leds[i].r = hue * 3;
    leds[i].g = 255 - hue * 3;
    leds[i].b = 0;
  }
  leds[i].r = leds[i].r * brightness / 255;
  leds[i].g = leds[i].g * brightness / 255;
  leds[i].b = leds[i].b * brightness / 255;
}

int pressed(void) {
  return bit_is_clear(PINB, PB0);
}

int prevPressed = 0;
int pushed(void) {
  if (pressed() && !prevPressed) {
    prevPressed = 1;
    return 1;
  }

  if (!pressed()) {
    prevPressed = 0;
  }

  return 0;
}

void setMode(void) {
  if (pushed()) {
    mode++;
    if (mode > 7) mode = 0;
  }
}

int main(void) {
  // -------- Inits --------- //
  DDRB  = 0x00;
  PORTB = 0x00;

  // ------ Event loop ------ //
  while (1) {
    setMode();

    if (mode == 0) {
      animatedHue += SPEED;
      h = animatedHue >> 8;
    } else {
      h = colors[mode - 1];
    }

    setHue(0, h, 64);
    ws2812_setleds(leds, NUM_LEDS);
  }
  return (0);
}
