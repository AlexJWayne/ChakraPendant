#include "FastLED.h"
#define NUM_LEDS 7
#define LED_PIN 0
#define BUTTON_PIN 1
#define BRIGHTNESS 64
#define LED_ANGLE 255 / 6
#define START_MODE 0
#define MODES 3

#define HUE_RED      0
#define HUE_ORANGE  30
#define HUE_YELLOW  64
#define HUE_GREEN   96
#define HUE_BLUE   160
#define HUE_INDIGO 190
#define HUE_VIOLET 205

CRGB leds[NUM_LEDS];

uint8_t mode = START_MODE;

void setup() {
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);

  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH); // internal pull up resitor for button
}

void loop() {
  if (buttonPushed()) {
    mode++;
    if (mode >= MODES) mode = 0;
  }

  switch(mode) {
    case 0: rainbow(); break;
    case 1: chakra1(); break;
    case 2: chakra2(); break;
  }

  FastLED.show();
}


// -
// - BUTTON
// -
bool buttonPrevPressed = false;

// Was the button just pushed?
bool buttonPushed() {
  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  if (pressed && !buttonPrevPressed) {
    buttonPrevPressed = true;
    return true;
  }

  if (!pressed) buttonPrevPressed = false;
  return false;
}


// -
// - PROGRAMS
// -

#define RAINBOW_SPEED 10
uint16_t rainbow_hue = 0;
void rainbow(void) {
  rainbow_hue += RAINBOW_SPEED;

  leds[0] = CRGB(92, 92, 92);

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    leds[i] = CHSV((rainbow_hue >> 8) + LED_ANGLE * i, 0xFF, 0xFF);
  }
}

#define CHAKRA1_SPEED 25
uint16_t chakra1_brightness = 0;
void chakra1() {
  chakra1_brightness += CHAKRA1_SPEED;
  uint8_t bright = scale8(quadwave8(chakra1_brightness >> 8), 160) + 95;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(HUE_RED, 0xFF, bright);
    FastLED.show();
  }
  leds[0].r = dim8_raw(leds[0].r);
}

#define CHAKRA2_SPEED 25
uint16_t chakra2_brightness = 0;
void chakra2() {
  chakra2_brightness += CHAKRA2_SPEED;
  uint8_t outer = quadwave8(chakra2_brightness >> 8);
  uint8_t inner = quadwave8(outer);

  inner = scale8(inner, 160) + 95;
  outer = scale8(outer, 160) + 95;

  leds[0] = CHSV(HUE_ORANGE, 0xFF, inner) + CRGB(16, 16, 16);

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    leds[i] = CHSV(HUE_ORANGE, 0xFF, outer);
    FastLED.show();
  }
}
