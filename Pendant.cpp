#include "Pendant.h"

// No-op interrupt. The interrupt is only used to wake from sleep.
ISR (PCINT0_vect) {}

Pendant::Pendant(uint8_t _modes) {
  modes = _modes;

  ADCSRA = 0;              // turn off ADC to save power
  GIMSK  |= _BV(PCIE0);    // enable pin change interrupts

  setBrightness(brightness);

  // Create the LED array.
  FastLED.addLeds<
    APA102,
    PENDANT_LED_DATA_PIN,
    PENDANT_LED_CLOCK_PIN
  >(leds, PENDANT_NUM_LEDS);

  // Enable an internal pullup resistor on the button pin.
  // HIGH is normal, LOW is pressed
  pinMode(PENDANT_BUTTON_PIN, INPUT_PULLUP);

  // Turn on the power switch for the LEDs.
  pinMode(PENDANT_LED_PWR_PIN, OUTPUT);
  enableLEDs();
}

void Pendant::sleepNow() {
  disableLEDs();

  PCMSK0 |= _BV(PCINT0);  // Watch pin PB1
  GIFR   |= _BV(PCIE0);   // clear any outstanding interrupts

  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_all_disable();  // power off ADC, Timer 0 and 1, serial interface
  sleep_enable();
  sleep_cpu();


  //
  // ... SLEEPING ...
  //


  // resumes here on wake
  sleep_disable();
  power_all_enable();    // power everything back on
  PCMSK0 &= ~_BV(PCINT0); // Turn off PB1 as interrupt pin

  sleepOnRelease = false;
  changeModeOnRelease = false;
  digitalWrite(PENDANT_LED_PWR_PIN, HIGH);
}

bool Pendant::update() {
  now = millis();

  if (buttonPressedAt) {
    // Cycle modes when held for a long time
    if (now > (buttonPressedAt + PENDANT_CYCLE_DELAY)) {
      toggleCycleMode();
    } else if (now > (buttonPressedAt + PENDANT_SLEEP_DELAY)) {
      sleepOnRelease = true;
    }
  }

  // Update pushed button state.
  updateButtonState();

  // Handle button release.
  if (buttonReleased()) {
    if (sleepOnRelease) {
      sleepNow();
    } else if (changeModeOnRelease) {
      mode++;
      if (cyclingMode) lastCycle = now;
    } else {
      changeModeOnRelease = true;
    }
  }

  // Cycle mode if we are cycling, and it's time.
  if (cyclingMode && (now > (lastCycle + PENDANT_CYCLE_DURATION))) {
    lastCycle = now;
    mode++;
  }

  // Go black to indicate power off.
  if (sleepOnRelease) {
    FastLED.showColor(CRGB::Black);
  }

  // Do the animations!
  // YAY!
  else {
    if (mode >= modes) mode = 0;
    return true;
  }

  return false;
}

// Was the button just pushed?
bool Pendant::updateButtonState() {
  bool pressed = digitalRead(PENDANT_BUTTON_PIN) == LOW;

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

// Was the button just released?
bool Pendant::buttonReleased() {
  bool released = digitalRead(PENDANT_BUTTON_PIN) == HIGH;

  if (released && !buttonPrevReleased) {
    buttonPrevReleased = true;
    return true;
  }

  if (!released) {
    buttonPrevReleased = false;
  }
  return false;
}


void Pendant::enableLEDs() {
  digitalWrite(PENDANT_LED_PWR_PIN, HIGH);
}

void Pendant::disableLEDs() {
  FastLED.showColor(CRGB::Black); // Turn off LEDs
  digitalWrite(PENDANT_LED_PWR_PIN,   LOW);
  digitalWrite(PENDANT_LED_DATA_PIN,  LOW);
  digitalWrite(PENDANT_LED_CLOCK_PIN, LOW);
  delay(100);
}

void Pendant::toggleCycleMode() {
  if (sleepOnRelease) {
    cyclingMode = !cyclingMode;
    sleepOnRelease = false;
    changeModeOnRelease = false;

    if (cyclingMode) {
      for (uint8_t i = 0; i < 255; i++) {
        FastLED.showColor(CHSV(i, 255, 255));
        FastLED.delay(1);
      }
    } else {
      for (uint8_t i = 255; i > 0; i--) {
        FastLED.showColor(CHSV(0, 0, i));
        FastLED.delay(1);
      }
    }

    lastCycle = now;
  }
}

void Pendant::enableCycleMode() {
  if (!cyclingMode) toggleCycleMode();
}

void Pendant::disableCycleMode() {
  if (cyclingMode) toggleCycleMode();
}

void Pendant::setBrightness(uint8_t _brightness) {
  brightness = _brightness;
  FastLED.setBrightness(brightness);
}

void Pendant::show() {
  FastLED.show();
}
