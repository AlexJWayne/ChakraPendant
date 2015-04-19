#include "FastLED.h"
#include "Pendant.h"

// The pendant instance
Pendant pendant = Pendant(8); // pass in the the total number of modes

// Define some colors to use in animations
#define HUE_RED      0
#define HUE_ORANGE  30
#define HUE_YELLOW  50
#define HUE_GREEN   96
#define HUE_BLUE   160
#define HUE_INDIGO 180
#define HUE_VIOLET 205

void setup() {
  // Nothing to do here.
}

// On each main loop, update the pendant, and if update() returns true, then do
// some animating based on the current mode.
void loop() {
  if (pendant.update()) {
    switch(pendant.mode) {
      case 0: rainbow(); break;
      case 1: chakra1(); break;
      case 2: chakra2(); break;
      case 3: chakra3(); break;
      case 4: chakra4(); break;
      case 5: chakra5(); break;
      case 6: chakra6(); break;
      case 7: chakra7(); break;
    }
    pendant.show();
  }
}

// -
// - ANIMATIONS
// -

#define RAINBOW_SPEED 25
uint16_t rainbow_hue = 0;
void rainbow(void) {
  rainbow_hue += RAINBOW_SPEED;

  pendant.leds[0] = CRGB(92, 92, 92);

  for (uint8_t i = 1; i < pendant.ledCount; i++) {
    pendant.leds[i] = CHSV((rainbow_hue >> 8) + pendant.ledAngle * i, 0xFF, 0xFF);
  }
}

#define CHAKRA1_SPEED 300
uint16_t chakra1_brightness = 0;
void chakra1() {
  chakra1_brightness += CHAKRA1_SPEED;
  uint8_t bright = scale8(quadwave8(chakra1_brightness >> 8), 160) + 95;

  for (uint8_t i = 0; i < pendant.ledCount; i++) {
    uint8_t swirlVal = swirl(true, 1, i, 70, 255);

    pendant.leds[i] = CHSV(
      HUE_RED,
      255 - scale8(swirlVal, 48),
      qadd8(bright, scale8(swirlVal, 92))
    );
    FastLED.show();
  }
  pendant.leds[0].r = dim8_raw(pendant.leds[0].r);
}

#define CHAKRA2_SPEED 240
uint16_t chakra2_brightness = 0;
void chakra2() {
  chakra2_brightness += CHAKRA2_SPEED;
  uint8_t outer = quadwave8(chakra2_brightness >> 8);
  uint8_t inner = quadwave8(outer);

  inner = scale8(inner, 160) + 95;
  outer = scale8(outer, 160) + 95;

  pendant.leds[0] = CHSV(HUE_ORANGE, 0xFF, inner) + CRGB(16, 16, 16);

  for (uint8_t i = 1; i < pendant.ledCount; i++) {
    pendant.leds[i] = CHSV(
      HUE_ORANGE,
      255,
      qadd8(outer, swirl(false, 1, i, 90, 64))
    );
    FastLED.show();
  }
}

#define CHAKRA3_FADE 14
#define CHAKRA3_THRESHOLD 24000
void chakra3() {

  if (random16() < CHAKRA3_THRESHOLD) {
    pendant.leds[random8(pendant.ledCount)] = CHSV(
      HUE_YELLOW + random8(20) - 5,
      random8(215, 255),
      255
    );
    FastLED.show();
  }

  for (uint8_t i = 0; i < pendant.ledCount; i++) {
    pendant.leds[i].fadeLightBy(i == 0 ? CHAKRA3_FADE/4 : CHAKRA3_FADE);
    FastLED.show();
  }
  FastLED.delay(30);
}

#define CHAKRA4_SPEED 300
#define CHAKRA4_DARKEST 75
#define CHAKRA4_MID_BEAT 48
#define CHAKRA4_END_BEAT 192
uint16_t chakra4_phase = 0;
void chakra4() {
  chakra4_phase += CHAKRA4_SPEED;
  for (uint8_t i = 0; i < pendant.ledCount; i++) {
    uint8_t progress = chakra4_phase >> 8;
    uint8_t brightness = 0;

    // Heartbeat comes from the center beating first!
    if (i == 0) progress += 40;

    if (progress < CHAKRA4_MID_BEAT) {
      progress = map(progress, 0, CHAKRA4_MID_BEAT, 0, 127);
    } else if (progress < CHAKRA4_END_BEAT) {
      progress = map(progress, CHAKRA4_MID_BEAT, CHAKRA4_END_BEAT, 128, 255);
    } else {
      progress = 255;
    }

    brightness = quadwave8(progress);
    brightness = CHAKRA4_DARKEST + scale8(brightness, 255-CHAKRA4_DARKEST);
    if (i > 0) brightness -= swirl(false, 2, i, 70, 48);

    pendant.leds[i] = CHSV(HUE_GREEN, (i == 0 ? 0xBB : 0xFF), brightness);
    FastLED.show();
  }
}

#define CHAKRA5_SPEED 300
#define CHAKRA5_SHIMMER 25
#define CHAKRA5_MIN 92
uint16_t chakra5_phase = 0;
void chakra5() {
  chakra5_phase -= CHAKRA5_SPEED;
  for (uint8_t i = 0; i < pendant.ledCount; i++) {
    uint8_t brightness = scale8(
      quadwave8((pendant.ledAngle * i) + (chakra5_phase >> 2)),
      CHAKRA5_SHIMMER
    );

    if (i > 0) {
      brightness += scale8(
        quadwave8(
          (pendant.ledAngle * i) +
          (chakra5_phase >> 7)
        ),
        255 - CHAKRA5_SHIMMER - CHAKRA5_MIN
      ) + CHAKRA5_MIN;
    } else {
      brightness += 255 - CHAKRA5_SHIMMER;
    }

    pendant.leds[i] = CHSV(HUE_BLUE, i == 0 ? 128 : 255, brightness);
    FastLED.show();
  }
}

#define CHAKRA6_SPEED 320
uint16_t chakra6_phase = 0;
void chakra6() {
  chakra6_phase += CHAKRA6_SPEED;

  pendant.leds[0] = CHSV(HUE_INDIGO, 64, scale8(
    quadwave8(chakra6_phase >> 8),
    255-92
  ) + 92);
  FastLED.show();

  for (uint8_t i = 1; i < pendant.ledCount; i++) {
    uint8_t brightness = scale8(
      quadwave8((2 * pendant.ledAngle * i) + (chakra6_phase >> 7))
    , 255-92) + 92;
    pendant.leds[i] = CHSV(HUE_INDIGO, 0xFF, brightness);
    FastLED.show();
  }
}

#define CHAKRA7_SPEED 250
uint16_t chakra7_phase = 0;
void chakra7() {
  chakra7_phase += CHAKRA7_SPEED;
  for (uint8_t i = 0; i < pendant.ledCount; i++) {
    pendant.leds[i] = CHSV(
      HUE_VIOLET,
      i > 0 ? scale8(quadwave8(((chakra7_phase >> 8) + (pendant.ledAngle * i)) * 3), 200) + 255-200 : 255,
      i > 0 ? swirl(true, 1, i, 30, 128) + 127                                               : 255
    );
    FastLED.show();
  }
}


//
// lib
//

uint16_t swirlPhase = 0;
uint8_t swirl(bool clockwise, uint8_t spokes, uint8_t idx, uint8_t speed, uint8_t scale) {
  swirlPhase += clockwise ? -speed : speed;
  return scale8(
    quadwave8((
      (pendant.ledAngle * idx) + (swirlPhase >> 8)
    ) * spokes)
  , scale);
}
