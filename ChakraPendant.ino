#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

#include "FastLED.h"
#define LED_DATA_PIN 4
#define LED_CLOCK_PIN 2
#define BUTTON_PIN 0
#define LED_PWR_PIN 3

#define NUM_LEDS 7
#define BRIGHTNESS 16
#define LED_ANGLE 255 / 6
#define START_MODE 0
#define MODES 8

#define HUE_RED      0
#define HUE_ORANGE  30
#define HUE_YELLOW  50
#define HUE_GREEN   96
#define HUE_BLUE   160
#define HUE_INDIGO 185
#define HUE_VIOLET 205

CRGB leds[NUM_LEDS];

uint8_t mode = START_MODE;

bool sleepOnRelease = false;
bool changeModeOnRelease = true;

// button state variables
bool buttonPrevPressed    = false;
bool buttonPrevReleased   = true;
uint32_t buttonPressedAt  = 0;
/*uint32_t buttonReleasedAt = 0;*/



void setup() {
  ADCSRA = 0;             // turn off ADC
  GIMSK  |= _BV(PCIE);    // enable pin change interrupts

  FastLED.setBrightness(BRIGHTNESS);
  FastLED.addLeds<APA102, LED_DATA_PIN, LED_CLOCK_PIN>(leds, NUM_LEDS);
//  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
//  FastLED.setDither(0);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  pinMode(LED_PWR_PIN, OUTPUT);
  digitalWrite(LED_PWR_PIN, HIGH);



  /*delay(1000);
  digitalWrite(LED_PWR_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PWR_PIN, LOW);
  delay(1000);*/
}

void loop() {
  if ((buttonPressedAt) && (millis() > (buttonPressedAt + 2000))) {
    sleepOnRelease = true;
  }
  buttonPushed(); // update pushed button state


  if (buttonReleased()) {
    if (sleepOnRelease) {
      sleepNow();
    } else if (changeModeOnRelease) {
      mode++;
      if (mode >= MODES) mode = 0;
    } else {
      changeModeOnRelease = true;
    }
  }

  if (sleepOnRelease) {
    FastLED.showColor(CRGB::Black);
  } else {
    // normal operation
    switch(mode) {
      case 0: rainbow(); break;
      case 1: chakra1(); break;
      case 2: chakra2(); break;
      case 3: chakra3(); break;
      case 4: chakra4(); break;
      case 5: chakra5(); break;
      case 6: chakra6(); break;
      case 7: chakra7(); break;
      default: mode = 0;
    }
    FastLED.show();
  }

}

void sleepNow() {
  FastLED.showColor(CRGB::Black); // Turn off LEDs
  digitalWrite(LED_PWR_PIN,   LOW);
  digitalWrite(LED_DATA_PIN,  LOW);
  digitalWrite(LED_CLOCK_PIN, LOW);
  delay(100);

  PCMSK  |= _BV(PCINT0);  // Watch pin PB1
  GIFR   |= _BV(PCIF);    // clear any outstanding interrupts

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_all_disable();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();

  // wake
  sleep_disable();
  power_all_enable();    // power everything back on
  PCMSK &= ~_BV(PCINT0); // Turn off PB1 as interrupt pin

  sleepOnRelease = false;
  changeModeOnRelease = false;
  digitalWrite(LED_PWR_PIN, HIGH);
}

ISR (PCINT0_vect) {}

// -
// - BUTTON
// -

// Was the button just pushed?
bool buttonPushed() {
  bool pressed = digitalRead(BUTTON_PIN) == LOW;

  if (pressed && !buttonPrevPressed) {
    buttonPressedAt = millis();
    buttonPrevPressed = true;
    return true;
  }

  if (!pressed) {
    buttonPressedAt = 0;
    buttonPrevPressed = false;
  }
  return false;
}

// Was the button jsut released?
bool buttonReleased() {
  bool released = digitalRead(BUTTON_PIN) == HIGH;

  if (released && !buttonPrevReleased) {
    /*buttonReleasedAt = millis();*/
    buttonPrevReleased = true;
    return true;
  }

  if (!released) {
    /*buttonReleasedAt = 0;*/
    buttonPrevReleased = false;
  }
  return false;
}

// -
// - PROGRAMS
// -

#define RAINBOW_SPEED 20
uint16_t rainbow_hue = 0;
void rainbow(void) {
  rainbow_hue += RAINBOW_SPEED;

  leds[0] = CRGB(92, 92, 92);

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    leds[i] = CHSV((rainbow_hue >> 8) + LED_ANGLE * i, 0xFF, 0xFF);
  }
}

#define CHAKRA1_SPEED 80
uint16_t chakra1_brightness = 0;
void chakra1() {
  chakra1_brightness += CHAKRA1_SPEED;
  uint8_t bright = scale8(quadwave8(chakra1_brightness >> 8), 160) + 95;

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    uint8_t swirlVal = swirl(true, 1, i, 30, 255);

    leds[i] = CHSV(
      HUE_RED,
      255 - scale8(swirlVal, 48),
      qadd8(bright, scale8(swirlVal, 92))
    );
    FastLED.show();
  }
  leds[0].r = dim8_raw(leds[0].r);
}

#define CHAKRA2_SPEED 80
uint16_t chakra2_brightness = 0;
void chakra2() {
  chakra2_brightness += CHAKRA2_SPEED;
  uint8_t outer = quadwave8(chakra2_brightness >> 8);
  uint8_t inner = quadwave8(outer);

  inner = scale8(inner, 160) + 95;
  outer = scale8(outer, 160) + 95;

  leds[0] = CHSV(HUE_ORANGE, 0xFF, inner) + CRGB(16, 16, 16);

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    leds[i] = CHSV(
      HUE_ORANGE,
      255,
      qadd8(outer, swirl(false, 1, i, 30, 64))
    );
    FastLED.show();
  }
}

#define CHAKRA3_FADE 12
#define CHAKRA3_THRESHOLD 12000
void chakra3() {

  if (random16() < CHAKRA3_THRESHOLD) {
    leds[random8(NUM_LEDS)] = CHSV(
      HUE_YELLOW + random8(20) - 5,
      random8(215, 255),
      255
    );
    FastLED.show();
  }

  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i].fadeLightBy(i == 0 ? CHAKRA3_FADE/4 : CHAKRA3_FADE);
    FastLED.show();
  }
  FastLED.delay(30);
}

#define CHAKRA4_SPEED 100
#define CHAKRA4_DARKEST 75
#define CHAKRA4_MID_BEAT 48
#define CHAKRA4_END_BEAT 192
uint16_t chakra4_phase = 0;
void chakra4() {
  chakra4_phase += CHAKRA4_SPEED;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
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
    if (i > 0) brightness -= swirl(false, 2, i, 20, 48);

    leds[i] = CHSV(HUE_GREEN, (i == 0 ? 0xBB : 0xFF), brightness);
    FastLED.show();
  }
}

#define CHAKRA5_SPEED 300
#define CHAKRA5_SHIMMER 30
#define CHAKRA5_MIN 92

uint16_t chakra5_phase = 0;
void chakra5() {
  chakra5_phase -= CHAKRA5_SPEED;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    uint8_t brightness = scale8(quadwave8((LED_ANGLE * i) + (chakra5_phase >> 4)), CHAKRA5_SHIMMER);

    if (i > 0) {
      brightness += scale8(
        quadwave8(
          (LED_ANGLE * i) +
          (chakra5_phase >> 7)
        ),
        255 - CHAKRA5_SHIMMER - CHAKRA5_MIN
      ) + CHAKRA5_MIN;
    } else {
      brightness += 255 - CHAKRA5_SHIMMER;
    }

    leds[i] = CHSV(HUE_BLUE, i == 0 ? 128 : 255, brightness);
    FastLED.show();
  }
}

#define CHAKRA6_SPEED 240
uint16_t chakra6_phase = 0;
void chakra6() {
  chakra6_phase += CHAKRA6_SPEED;

  leds[0] = CHSV(HUE_INDIGO, 64, scale8(quadwave8(chakra6_phase >> 8), 255-92) + 92);
  FastLED.show();

  for (uint8_t i = 1; i < NUM_LEDS; i++) {
    uint8_t brightness = scale8(
      quadwave8((2 * LED_ANGLE * i) + (chakra6_phase >> 7))
    , 255-92) + 92;
    leds[i] = CHSV(HUE_INDIGO, 0xFF, brightness);
    FastLED.show();
  }
}

#define CHAKRA7_SPEED 200
uint16_t chakra7_phase = 0;
void chakra7() {
  chakra7_phase += CHAKRA7_SPEED;
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(
      HUE_VIOLET,
      i > 0 ? scale8(quadwave8(((chakra7_phase >> 8) + (LED_ANGLE * i)) * 3), 200) + 255-200 : 255,
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
      (LED_ANGLE * idx) + (swirlPhase >> 8)
    ) * spokes)
  , scale);
}
