#include "defines.h"
#include "led.h"

void Led::init(int pin)
{
    FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, NUM_LEDS);

    for (int i = 0; i < NUM_LEDS; i++) {
        _leds[i] = CRGB::Black;
    }

    FastLED.show();
}
