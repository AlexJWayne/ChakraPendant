
                                                       /* Blinker Demo */

// ------- Preamble -------- //
#include <avr/io.h>                        /* Defines pins, ports, etc */
#include <util/delay.h>                     /* Functions to waste time */
#include <avr/interrupt.h>
#include "light_ws2812.h"
// #include "FastLED/FastLED.h"

#define BRIGHTNESS 64
#define NUM_LEDS    7

#define LED_ANGLE 255 / 6

#define HUE_RED      0
#define HUE_ORANGE  20
#define HUE_YELLOW  40
#define HUE_GREEN   85
#define HUE_BLUE   135
#define HUE_INDIGO 190
#define HUE_VIOLET 205

struct cRGB leds[NUM_LEDS];
uint8_t mode = 0;

uint8_t colors[7] = {
  HUE_RED, HUE_ORANGE, HUE_YELLOW, HUE_GREEN, HUE_BLUE, HUE_INDIGO, HUE_VIOLET,
};

uint16_t sadd8(uint8_t a, uint8_t b) {
  return (a > 0xFF - b) ? 0xFF : a + b;
  }

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
  return bit_is_clear(PINB, PB1);
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


#define RAINBOW_SPEED 40
uint16_t rainbowHue = 0;
void rainbow(void) {
  rainbowHue += RAINBOW_SPEED;

  leds[0].b = BRIGHTNESS * 1/4;
  leds[0].g = BRIGHTNESS * 1/4;
  leds[0].r = BRIGHTNESS * 1/4;

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    setHue(
      i,
      (rainbowHue >> 8) + (255 / (NUM_LEDS - 1)) * i,
      BRIGHTNESS
    );
  }
}


#define CHAKRA1_DELAY 16
uint8_t chakra1_shadow = 0;
void chakra1(void) {
  chakra1_shadow++;

  uint8_t fadeAmount = chakra1_shadow;
  if (fadeAmount > 127) { fadeAmount = 255 - fadeAmount; }
  fadeAmount *= 509/255;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    setHue(i,
      HUE_RED,
      BRIGHTNESS - BRIGHTNESS * fadeAmount / 255
    );
  }

  leds[0].r /= 2;
  leds[0].g /= 2;
  leds[0].b /= 2;

  _delay_ms(CHAKRA1_DELAY);
}



// #define CHAKRA1_DELAY 20
// uint8_t chakra1_shadow = 0;
// void chakra1(void) {
//   chakra1_shadow++;
//
//   uint8_t angle = 0;
//
//   for (uint8_t i = 0; i < NUM_LEDS; i++) {
//     setHue(i, HUE_RED, BRIGHTNESS);
//
//     angle = i * LED_ANGLE;
//     if (angle >= chakra1_shadow && angle <= chakra1_shadow + LED_ANGLE) {
//       leds[i].r -= (chakra1_shadow - angle);
//     }
//   }
//
//   leds[0].r /= 2;
//   leds[0].g /= 2;
//   leds[0].b /= 2;
//
//   _delay_ms(CHAKRA1_DELAY);
// }



int main(void) {
  // -------- Inits --------- //
  DDRB  = 0x00;
  PORTB = 0x00;

  // ------ Event loop ------ //
  while (1) {
    // PORTB = 0xFF;
    // _delay_ms(1000);
    // PORTB = 0x00;
    // _delay_ms(1000);

    setMode();

    switch(mode) {
      case 0:
        rainbow(); break;
      case 1:
        chakra1(); break;
      default:
        leds[0].r = BRIGHTNESS / 4;
        leds[0].g = BRIGHTNESS / 4;
        leds[0].b = BRIGHTNESS / 4;

        for (uint8_t i = 1; i < NUM_LEDS; i++) {
          setHue(i, colors[mode - 1], BRIGHTNESS);
        }
    }

    ws2812_setleds(leds, NUM_LEDS);
  }
  return (0);
}
